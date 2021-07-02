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

#include "qmycroftplugin.h"
#include "controller.h"
#include "skillmanager.h"
#include "audiotranscribe.h"
#include "skillentry.h"
#include <QQmlContext>
#include <QQmlEngine>

static QObject *controllerSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);

    //singleton managed internally, qml should never delete it
    engine->setObjectOwnership(Controller::instance(), QQmlEngine::CppOwnership);
    return Controller::instance();
}

void QmycroftPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<Controller>(uri, 1, 0, "Controller", controllerSingletonProvider);
    qmlRegisterType<SkillManager>(uri, 1, 0, "SkillManager");
    qmlRegisterType<SkillEntry>(uri, 1,0, "SkillEntry");
    qmlRegisterType<AudioTranscribe>(uri, 1, 0, "AudioTranscribe");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/TranscribeButton.qml")), uri, 1, 0, "TranscribeButton");
}
