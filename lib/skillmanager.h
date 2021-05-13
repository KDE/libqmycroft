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

#ifndef QSKILL_H
#define QSKILL_H

#include <QObject>
#include <QVector>
#include <QQmlListProperty>
#include <QStringList>
#include <QSettings>
#include "skillentry.h"
#include "controller.h"

class Controller;
class SkillManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString skillNamespace READ skillNamespace WRITE setSkillNamespace NOTIFY skillNamespaceChanged)
    Q_PROPERTY(QString socketAddress READ socketAddress WRITE setSocketAddress NOTIFY socketAddressChanged)
    Q_PROPERTY(QQmlListProperty<SkillEntry> items READ items)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    explicit SkillManager(QObject *parent = nullptr);
    ~SkillManager() override;
    QString skillNamespace() const;
    QString socketAddress() const;
    QQmlListProperty<SkillEntry> items();

public Q_SLOTS:
    int itemsCount() const;
    SkillEntry *item(int) const;
    void setSkillNamespace(QString skillNamespace);
    QJsonObject toJson(SkillEntry* &item) const;
    void createSkill();
    void deleteSkill();
    bool isSocketReady() const;
    void onMainSocketMessageReceived(const QString &message);
    void setSocketAddress(QString socketAddress);

Q_SIGNALS:
    void skillNamespaceChanged();
    void socketReadyChanged(bool socketReady);
    void intentResponse(const QVariant &response, const QString &action);
    void socketAddressChanged();

private:
    bool m_socketReady = false;
    QSettings m_settings;
    QList<SkillEntry*> m_items;
    Controller *m_controller;
};

#endif // LIBQMYCROFT_H
