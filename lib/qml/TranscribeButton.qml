import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls
import Libqmycroft 1.0

Controls.Button {
    id: rootControl
    property bool transcribeActive: false
    property var target: parent
    anchors.right: parent.right
    icon.name: "audio-input-microphone"
    icon.width: parent.width / 2
    icon.height: parent.height / 2
    icon.color: activePalette.mid
    property var requestID

    function generateUniqueID(){
        audTranscriber.generateRequestIdentifier()
        requestID = audTranscriber.getRequestIdentifier()
    }

    Component.onCompleted: {
        generateUniqueID()
    }

    onTranscribeActiveChanged: {
        if(!transcribeActive){
            audTranscriber.stop()
        }
    }

    background: Rectangle {
        color: "transparent"
    }

    SystemPalette {
        id: activePalette
        colorGroup: SystemPalette.Active
    }

    AudioTranscribe {
        id: audTranscriber

        onResponseReceived: {
            try {
                rootControl.target.text = response
            }
            catch(error) {
                console.log(error)
            }
        }
    }

    Timer {
        id: timer
    }

    function delay(delayTime, cb) {
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }

    Keys.onReturnPressed: {
        clicked()
    }

    onClicked: {
        if(audTranscriber.status == AudioTranscribe.Inactive){
            if(!rootControl.transcribeActive) {
                audTranscriber.start()
                rootControl.transcribeActive = true
                icon.color = activePalette.highlight
                delay(8000, function() {
                    rootControl.transcribeActive = false
                    icon.color = activePalette.mid
                })
            }
        }
    }
}
