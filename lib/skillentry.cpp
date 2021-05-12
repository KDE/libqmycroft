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

#include "skillentry.h"
#include <QJsonObject>

SkillEntry::SkillEntry(QObject *parent) : QObject(parent)
{
}

QString SkillEntry::intent() const
{
    return _intent;
}

void SkillEntry::setIntent(const QString &intent)
{
    if(_intent == intent) return;
    _intent = intent;
    Q_EMIT intentChanged(_intent);
}

QString SkillEntry::action() const
{
    return _action;
}

void SkillEntry::setAction(const QString &action)
{
    if(_action == action) return;
    _action = action;
    Q_EMIT actionChanged(_action);
}

QString SkillEntry::voc() const
{
    return _voc;
}

void SkillEntry::setVoc(const QString &voc)
{
    if(_voc == voc) return;
    _voc = voc;
    Q_EMIT vocChanged(_voc);
}

QString SkillEntry::dialog() const
{
    return _dialog;
}

void SkillEntry::setDialog(const QString &dialog)
{
    if(_dialog == dialog) return;
    _dialog = dialog;
    Q_EMIT dialogChanged(_dialog);
}
