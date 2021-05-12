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

#pragma once

#include <QObject>
#include <QWebSocket>
#include <QPointer>
#include <QQuickItem>
#include <QTimer>
#include <QSettings>
#include "skillmanager.h"

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY socketStatusChanged)

    Q_PROPERTY(bool speaking READ isSpeaking NOTIFY isSpeakingChanged)
    Q_PROPERTY(bool listening READ isListening NOTIFY isListeningChanged)

    Q_PROPERTY(QString currentSkill READ currentSkill NOTIFY currentSkillChanged)
    Q_PROPERTY(QString currentIntent READ currentIntent NOTIFY currentIntentChanged)

    Q_PROPERTY(bool serverReady READ serverReady NOTIFY serverReadyChanged)
    
    Q_PROPERTY(QString webSocketAddress READ webSocketAddress WRITE setWebSocketAddress NOTIFY webSocketChanged)
    Q_PROPERTY(bool useHivemind READ useHivemind WRITE setUseHivemind NOTIFY useHivemindChanged)

    Q_ENUMS(Status)

public:
    enum Status {
        Connecting,
        Open,
        Closing,
        Closed,
        Error
    };

    static Controller* instance();
    bool isSpeaking() const;
    bool isListening() const;
    bool serverReady() const;
    Status status() const;
    QString currentSkill() const;
    QString currentIntent() const;
    QString webSocketAddress() const;
    bool useHivemind() const;

Q_SIGNALS:
    void socketStatusChanged();
    void closed();

    void isSpeakingChanged();
    void isListeningChanged();
    void stopped();
    void notUnderstood();
    void currentSkillChanged();
    void currentIntentChanged();
    void serverReadyChanged();
    void speechRequestedChanged(bool expectingResponse);

    void intentRecevied(const QString &type, const QVariantMap &data);

    void fallbackTextRecieved(const QString &skill, const QVariantMap &data);

    void utteranceManagedBySkill(const QString &skill);
    void skillTimeoutReceived(const QString &skillidleid);
    void messageReceived(const QString &message);
    
    void webSocketChanged();
    void useHivemindChanged();

public Q_SLOTS:
    void start();
    void disconnectSocket();
    void reconnect();
    void sendRequest(const QString &type, const QVariantMap &data);
    void sendBinary(const QString &type, const QJsonObject &data);
    void sendText(const QString &message);
    void setWebSocketAddress(QString webSocketAddress);
    void setUseHivemind(bool useHivemind);

private:
    explicit Controller(QObject *parent = nullptr);
    void onMainSocketMessageReceived(const QString &message);

    QWebSocket m_mainWebSocket;

    QTimer m_reconnectTimer;

    QString m_currentSkill;
    QString m_currentIntent;

    bool m_isSpeaking = false;
    bool m_isListening = false;
    bool m_mycroftLaunched = false;
    bool m_serverReady = false;
    
    QSettings m_settings;
};
