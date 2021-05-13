/*
 * Copyright 2021 Aditya Mehra <aix.m@outlook.com>
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

#include "skillmanager.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

SkillManager::SkillManager(QObject *parent): 
    QObject(parent),
    m_controller(Controller::instance())
{
    m_controller->start();
    connect(m_controller, &Controller::socketStatusChanged, this,
            [this]() {
        if (m_controller->status() == Controller::Open){
            if (m_socketReady == true){
                return;
            } else {
                m_socketReady =  true;
            };
            emit socketReadyChanged(isSocketReady());
        }
    });
    
    connect(m_controller, &Controller::messageReceived, this, &SkillManager::onMainSocketMessageReceived);
}

SkillManager::~SkillManager()
{
    deleteSkill();
    m_controller->disconnectSocket();
}

bool SkillManager::isSocketReady() const {
    return m_socketReady;
}

QString SkillManager::socketAddress() const
{
    return m_controller->webSocketAddress();
}

void SkillManager::setSocketAddress(QString socketAddress)
{
    if (m_controller->webSocketAddress() == socketAddress){
        return;
    }
    m_controller->setWebSocketAddress(socketAddress);
    emit socketAddressChanged();
}

QString SkillManager::skillNamespace() const
{
    return m_settings.value(QStringLiteral("skillNamespace")).toString();
}

void SkillManager::setSkillNamespace(QString skillNamespace)
{
    if (SkillManager::skillNamespace() == skillNamespace) {
        return;
    }
    m_settings.setValue(QStringLiteral("skillNamespace"), skillNamespace);
    emit skillNamespaceChanged();
}

QQmlListProperty<SkillEntry> SkillManager::items()
{
    return QQmlListProperty<SkillEntry>(this, &this->m_items);
}

int SkillManager::itemsCount() const
{
    return m_items.count();
}

SkillEntry *SkillManager::item(int index) const
{
    return m_items.at(index);
}

QJsonObject SkillManager::toJson(SkillEntry* &item) const
{
    return {{QStringLiteral("intent"), item->intent()}, {QStringLiteral("voc"), item->voc()}, {QStringLiteral("action"), item->action()}, {QStringLiteral("dialog"), item->dialog()}};
}

void SkillManager::createSkill()
{
    QJsonObject root;
    root[QStringLiteral("namespace")] = m_settings.value(QStringLiteral("skillNamespace")).toString();

    QJsonArray array;
    for(int i = 0; i < m_items.count(); i++){
        array.append(toJson(m_items[i]));
    }
    root[QStringLiteral("parameters")] = array;
    
    if(m_socketReady){
        m_controller->sendRequest(QStringLiteral("mycroft.create.dynamic.skill"), root.toVariantMap());
    }
}

void SkillManager::deleteSkill()
{
    QJsonObject root;
    root[QStringLiteral("namespace")] = m_settings.value(QStringLiteral("skillNamespace")).toString();

    QJsonArray array;
    for(int i = 0; i < m_items.count(); i++){
        array.append(toJson(m_items[i]));
    }
    root[QStringLiteral("parameters")] = array;
    
    if(m_socketReady){
        m_controller->sendRequest(QStringLiteral("mycroft.delete.dynamic.skill"), root.toVariantMap());
    }
}

void SkillManager::onMainSocketMessageReceived(const QString &message)
{
    auto doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isEmpty()) {
        qWarning() << "Empty or invalid JSON message arrived on the main socket:" << message;
        return;
    }
    
    auto type = doc[QStringLiteral("type")].toString();
    
    if(type == QStringLiteral("mycroft.dynamic.skill.intent")){
        QString intent_target = doc[QStringLiteral("data")][QStringLiteral("intent_target")].toString();
        QString intent_call = doc[QStringLiteral("data")][QStringLiteral("intent_call")].toString();
        auto intent_caller = doc[QStringLiteral("data")][QStringLiteral("intent_caller")].toVariant();

        for(int i = 0; i < m_items.count(); i++){
            if(intent_target == skillNamespace() && m_items[i]->intent() == intent_call){
                emit intentResponse(doc.toJson(), m_items[i]->action());
                if(m_items[i]->dialog() != QStringLiteral("")){
                    m_controller->sendRequest(QStringLiteral("mycroft.dynamic.skill.speak"), {{QStringLiteral("dialog"), m_items[i]->dialog()}});
                }
            }
        }
    }
}
