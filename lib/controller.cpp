/*
 * Copyright 2021 Aditya Mehra <aix.m@outlook.com>
 * Copyright 2018 by Marco Martin <mart@kde.org>
 * Copyright 2018 David Edmundson <davidedmundson@kde.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QQmlPropertyMap>
#include <QQmlEngine>
#include <QQmlContext>
#include <QWebSocket>
#include "controller.h"

Controller *Controller::instance()
{
    static Controller* s_self = nullptr;
    if (!s_self) {
        s_self = new Controller;
    }
    return s_self;
}

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    connect(&m_mainWebSocket, &QWebSocket::connected, this,
            [this] () {
        m_reconnectTimer.stop();
        emit socketStatusChanged();
    });
    connect(&m_mainWebSocket, &QWebSocket::disconnected, this, &Controller::closed);
    connect(&m_mainWebSocket, &QWebSocket::stateChanged, this,
            [this] (QAbstractSocket::SocketState state) {
        emit socketStatusChanged();
        if (state == QAbstractSocket::ConnectedState) {
            qWarning() << "Main Socket Connected";
            sendRequest(QStringLiteral("mycroft.skills.all_loaded"), QVariantMap());
        } else {
            if (m_serverReady) {
                m_serverReady = false;
                emit serverReadyChanged();
            }
        }
    });

    connect(&m_mainWebSocket, &QWebSocket::textMessageReceived, this, &Controller::onMainSocketMessageReceived);

    m_reconnectTimer.setInterval(1000);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this]() {
        QString socket = webSocketAddress();
        m_mainWebSocket.open(QUrl(socket));
    });
}

void Controller::start()
{
    qDebug() << "Trying To Start Connection";
    QString socket = webSocketAddress();
    m_mainWebSocket.open(QUrl(socket));
    connect(&m_mainWebSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, [this] (const QAbstractSocket::SocketError &error) {

        if (error != QAbstractSocket::HostNotFoundError && error != QAbstractSocket::ConnectionRefusedError) {
            qWarning() << "Mycroft is running but the connection failed for some reason. Kill Mycroft manually.";

            return;
        }

        m_reconnectTimer.start();
        emit socketStatusChanged();
    });

    emit socketStatusChanged();
}


void Controller::disconnectSocket()
{
    qDebug() << "in reconnect";
    m_mainWebSocket.close();
    m_reconnectTimer.stop();

    emit socketStatusChanged();
}

void Controller::reconnect()
{
    qDebug() << "in reconnect";
    m_mainWebSocket.close();
    m_reconnectTimer.start();
    emit socketStatusChanged();
}

void Controller::onMainSocketMessageReceived(const QString &message)
{
    emit messageReceived(message);
    auto doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isEmpty()) {
        qWarning() << "Empty or invalid JSON message arrived on the main socket:" << message;
        return;
    }

    auto type = doc[QStringLiteral("type")].toString();

    if (type.isEmpty()) {
        qWarning() << "Empty type in the JSON message on the main socket";
        return;
    }

    emit intentRecevied(type, doc[QStringLiteral("data")].toVariant().toMap());

    if (type == QLatin1String("complete_intent_failure")) {
        m_isListening = false;
        emit isListeningChanged();
        emit notUnderstood();
    }
    if (type == QLatin1String("recognizer_loop:audio_output_start")) {
        m_isSpeaking = true;
        emit isSpeakingChanged();
        return;
    }
    if (type == QLatin1String("recognizer_loop:audio_output_end")) {
        m_isSpeaking = false;
        emit isSpeakingChanged();
        return;
    }
    if (type == QLatin1String("recognizer_loop:wakeword")) {
        m_isListening = true;
        emit isListeningChanged();
        return;
    }
    if (type == QLatin1String("recognizer_loop:record_begin") && !m_isListening) {
        m_isListening = true;
        emit isListeningChanged();
        return;
    }
    if (type == QLatin1String("recognizer_loop:record_end")) {
        m_isListening = false;
        emit isListeningChanged();
        return;
    }
    if (type == QLatin1String("mycroft.speech.recognition.unknown")) {
        emit notUnderstood();
        return;
    }

    if (type == QLatin1String("mycroft.skill.handler.start")) {
        m_currentSkill = doc[QStringLiteral("data")][QStringLiteral("name")].toString();
        qDebug() << "Current intent:" << m_currentIntent;
        emit currentIntentChanged();
    } else if (type == QLatin1String("mycroft.skill.handler.complete")) {
        m_currentSkill = QString();
        emit currentSkillChanged();
    } else if (type == QLatin1String("speak")) {
        emit fallbackTextRecieved(m_currentSkill, doc[QStringLiteral("data")].toVariant().toMap());
    } else if (type == QLatin1String("mycroft.stop.handled") || type == QLatin1String("mycroft.stop")) {
        emit stopped();
    } else if (type == QLatin1String("mycroft.skills.all_loaded.response")) {
        if (doc[QStringLiteral("data")][QStringLiteral("status")].toBool() == true) {
            m_serverReady = true;
            emit serverReadyChanged();
        }
    } else if (type == QLatin1String("mycroft.ready")) {
        m_serverReady = true;
        emit serverReadyChanged();
    }

    // Check if it's an utterance recognized as an intent
    if (type.contains(QLatin1Char(':')) && !doc[QStringLiteral("data")][QStringLiteral("utterance")].toString().isEmpty()) {
        const QString skill = type.split(QLatin1Char(':')).first();
        if (skill.contains(QLatin1Char('.'))) {
            m_currentSkill = skill;
            qDebug() << "Current skill:" << m_currentSkill;
            emit utteranceManagedBySkill(m_currentSkill);
            emit currentSkillChanged();
        }
    }
}

void Controller::sendRequest(const QString &type, const QVariantMap &data)
{
    if (m_mainWebSocket.state() != QAbstractSocket::ConnectedState) {
        qWarning() << "mycroft connection not open!";
        return;
    }
    QJsonObject root;

    root[QStringLiteral("type")] = type;
    root[QStringLiteral("data")] = QJsonObject::fromVariantMap(data);

    QJsonDocument doc(root);
    m_mainWebSocket.sendTextMessage(QString::fromUtf8(doc.toJson()));
}

void Controller::sendBinary(const QString &type, const QJsonObject &data)
{
    if (m_mainWebSocket.state() != QAbstractSocket::ConnectedState) {
        qWarning() << "mycroft connection not open!";
        return;
    }
    QJsonObject socketObject;
    socketObject[QStringLiteral("type")] = type;
    socketObject[QStringLiteral("data")] = data;

    QJsonDocument doc;
    doc.setObject(socketObject);
    QByteArray docbin = doc.toJson(QJsonDocument::Compact);
    m_mainWebSocket.sendBinaryMessage(docbin);
}

void Controller::sendText(const QString &message)
{
    sendRequest(QStringLiteral("recognizer_loop:utterance"), QVariantMap({{QStringLiteral("utterances"), QStringList({message})}}));
}

Controller::Status Controller::status() const
{
    if (m_reconnectTimer.isActive()) {
        return Connecting;
    }

    switch(m_mainWebSocket.state())
    {
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::HostLookupState:
        return Connecting;
    case QAbstractSocket::UnconnectedState:
        return Closed;
    case QAbstractSocket::ConnectedState:
        return Open;
    case QAbstractSocket::ClosingState:
        return Closing;
    default:
        return Connecting;
    }
}

QString Controller::currentSkill() const
{
    return m_currentSkill;
}

QString Controller::currentIntent() const
{
    return m_currentIntent;
}

bool Controller::isSpeaking() const
{
    return m_isSpeaking;
}

bool Controller::isListening() const
{
    return m_isListening;
}

bool Controller::serverReady() const
{
    return m_serverReady;
}

QString Controller::webSocketAddress() const
{
    if (Controller::useHivemind() == false){
        return m_settings.value(QStringLiteral("webSocketAddress"), QStringLiteral("ws://0.0.0.0:8181/core")).toString();
    } else {
        return m_settings.value(QStringLiteral("webSocketAddress"), QStringLiteral("ws://0.0.0.0::5678?accessKey=R1VJOmFzZGZnaGprbDEyMzQ1Njc=")).toString();
    }
}

void Controller::setWebSocketAddress(QString webSocketAddress)
{
    if (Controller::webSocketAddress() == webSocketAddress) {
        return;
    }
    m_settings.setValue(QStringLiteral("webSocketAddress"), webSocketAddress);
    emit webSocketChanged();
}

bool Controller::useHivemind() const
{
    return m_settings.value(QStringLiteral("useHivemind")).toBool();
}

void Controller::setUseHivemind(bool useHivemind)
{
    if (Controller::useHivemind() == useHivemind) {
        return;
    }

    m_settings.setValue(QStringLiteral("useHivemind"), useHivemind);
    emit useHivemindChanged();
}

#include "moc_controller.cpp"
