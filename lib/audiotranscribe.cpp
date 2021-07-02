#include "audiotranscribe.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QDebug>

AudioTranscribe::AudioTranscribe(QObject *parent)
    : QObject(parent),
      m_controller(Controller::instance())
{

    if (m_controller->status() != Controller::Open){
        m_controller->start();
    }

    connect(m_controller, &Controller::socketStatusChanged, this,
            [this]() {
        if (m_controller->status() == Controller::Open){
            connect(m_controller, &Controller::messageReceived, this,
                    &AudioTranscribe::onMainSocketMessageReceived);
        }
    });

    QAudioFormat format;
    format.setCodec(QStringLiteral("audio/PCM"));
    format.setSampleRate(16000);
    format.setSampleSize(16);
    format.setChannelCount(1);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        format = info.nearestFormat(format);
        format.setSampleRate(16000);
        qDebug() << "Raw audio format not supported by backend. Trying the nearest format.";
    }

    m_audioInput = new QAudioInput(format, this);
}

void AudioTranscribe::start()
{
    emit startRequested();
    startRecording();
    m_stage = 1;
}

void AudioTranscribe::stop(){
    emit endRequested();
    endRecording();
}

void AudioTranscribe::sendData()
{
    if (m_controller->status() == Controller::Open){
        QJsonObject root;
        qDebug() << "Sending Data";
        root[QStringLiteral("responseid")] = getRequestIdentifier();
        root[QStringLiteral("file")] = QStringLiteral("/tmp/%1_in.raw").arg(m_requestIdentifier);
        m_controller->sendRequest(QStringLiteral("libqmycroft.request.transcribe"), root.toVariantMap());
    }
}

void AudioTranscribe::startRecording()
{
    destinationFile.setFileName(QStringLiteral("/tmp/%1_in.raw").arg(m_requestIdentifier));
    destinationFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
    m_audioInput->start(&destinationFile);
    emit recordingStarted();
}

void AudioTranscribe::endRecording()
{
    m_audioInput->stop();
    destinationFile.close();
    emit recordingEnded();
    sendData();
    m_stage = 0;
}

void AudioTranscribe::generateRequestIdentifier()
{
    m_requestIdentifier = QRandomGenerator::global()->bounded(10000, 99999);
    emit requestIdentifierGenerated();
}

QString AudioTranscribe::getRequestIdentifier()
{
    return QString::number(m_requestIdentifier);
}

void AudioTranscribe::onMainSocketMessageReceived(const QString &message)
{
    auto doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isEmpty()) {
        qWarning() << "Empty or invalid JSON message arrived on the main socket:" << message;
        return;
    }

    auto type = doc[QStringLiteral("type")].toString();
    if(type == QStringLiteral("libqmycroft.request.transcribe.result")){
        QString targetResponseId = doc[QStringLiteral("data")][QStringLiteral("targetResponseId")].toString();
        QString targetResult = doc[QStringLiteral("data")][QStringLiteral("targetResult")].toString();
        if(targetResponseId == getRequestIdentifier()) {
            emit responseReceived(targetResult);
        }
    }
}

AudioTranscribe::Status AudioTranscribe::status() const
{
    switch(m_stage)
    {
    case 0:
        return Inactive;
    case 1:
        return Active;
    default:
        return Inactive;
    }
}
