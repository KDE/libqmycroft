#pragma once

#include <QObject>
#include <QAudioInput>
#include <QFile>
#include "controller.h"
#include <QRandomGenerator>

class Controller;
class AudioTranscribe : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY transcribeStatusChanged)
    Q_ENUMS(Status)

public:
    //static AudioTranscribe* instance();
    enum Status {
        Inactive,
        Active
    };
    Status status() const;
    explicit AudioTranscribe(QObject *parent = nullptr);

public Q_SLOTS:
    void start();
    void stop();
    void generateRequestIdentifier();
    QString getRequestIdentifier();

Q_SIGNALS:
    void transcribeStatusChanged();
    void responseReceived(const QString &response);
    void startRequested();
    void endRequested();
    void recordingStarted();
    void recordingEnded();
    void requestIdentifierGenerated();

private:
    void onMainSocketMessageReceived(const QString &message);
    void startRecording();
    void endRecording();
    void sendData();
    qint32 m_requestIdentifier;
    Controller *m_controller;
    QFile destinationFile;
    QAudioInput *m_audioInput;
    QIODevice *device;
    qint32 m_stage;
};
