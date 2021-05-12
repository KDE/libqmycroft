import QtQuick 2.12
import QtQuick.Window 2.12
import Libqmycroft 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Mycroft!")
    property string currentPath: Qt.resolvedUrl(".").substring(7)
    
    SkillManager {
        id: skillRegisteration
        skillNamespace: "Test-Skill"
        
        SkillEntry {
            intent: "testoneintent"
            voc: currentPath + "/vocab/test_intent_one.intent"
            action: "example-action-1"
            dialog: "Intent One Successful"
        }
        
        SkillEntry {
            intent: "testtwointent"
            voc: currentPath + "/vocab/test_intent_two.intent"
            action: "example-action-2"
            dialog: "Intent Two Successful"
        }
        
        onSocketReadyChanged: {
            if(socketReady){
                skillRegisteration.createSkill()
            }
        }
        
        onIntentResponse: {
            if(action == "example-action-1"){
                rectangleExample1.color = "blue"
            }
            if(action == "example-action-2"){
                rectangleExample2.color = "green"
            }
        }
    }
    
    Rectangle {
        id: rectangleExample1
        width: parent.width
        height: parent.height /2
        color: "green"
    }
    
    Rectangle {
        id: rectangleExample2
        anchors.top: rectangleExample1.bottom
        width: parent.width
        height: parent.height /2
        color: "black"
    }
}
