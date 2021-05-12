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

#ifndef SKILLENTRY_H
#define SKILLENTRY_H

#include <QObject>

class SkillEntry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString intent READ intent WRITE setIntent NOTIFY intentChanged)
    Q_PROPERTY(QString action READ action WRITE setAction NOTIFY actionChanged)
    Q_PROPERTY(QString voc READ voc WRITE setVoc NOTIFY vocChanged)
    Q_PROPERTY(QString dialog READ dialog WRITE setDialog NOTIFY dialogChanged)

public:
    explicit SkillEntry(QObject *parent = nullptr);

    QString intent() const;
    void setIntent(const QString &intent);

    QString action() const;
    void setAction(const QString &action);

    QString voc() const;
    void setVoc(const QString &voc);
    
    QString dialog() const;
    void setDialog(const QString &voc);

signals:
    void intentChanged(QString intent);
    void actionChanged(QString action);
    void vocChanged(QString voc);
    void dialogChanged(QString dialog);
    
private:
    QString _intent;
    QString _action;
    QString _voc;
    QString _dialog;
};
#endif

