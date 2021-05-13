# Libqmycroft

**Mycroft integration library using a mock API to integrate Qt/QML applications as dynamic skills in Mycroft Core.**

Libqmycroft allows QML based applications to register themselves as dynamic skills using the Libqmycroft Mock Skills Interface which eliminates the need to create specific skills for traditional applications to gain voice control over the application user interface. 

*The library is currently limited to usage of only [Mycroft's Padatious](https://mycroft-ai.gitbook.io/docs/mycroft-technologies/padatious) intent parser framework, The library is in early development and interface/API breakage can be expected!*




--------------------------------

- [Libqmycroft](#libqmycroft)
  * [Libqmycroft Requirements](#libqmycroft-requirements)
  * [Libqmycroft Installation](#libqmycroft-installation)
  * [Libqmycroft API & Usage](#libqmycroft-api---usage)
      - [SkillManager Object](#skillmanager-object)
      - [SkillEntry Object](#skillentry-object)
      - [Controller Class](#controller-class)
  * [Usage Example of Libqmycroft in QML application](#usage-example-of-libqmycroft-in-qml-application)

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



## Libqmycroft Requirements

- [Mycroft-Core](https://github.com/MycroftAI/mycroft-core/): Local installation of Mycroft Core. The code itself  includes anything that is generic to all instances of Mycroft. 
- Libqmycroft: Library framework and QML Module contained in this repository and built and installed using cmake.
- Libqmycroft-Mock-Skills-Interface: Mycroft Skill Interface part of this repository, requires installation into Mycroft's skill folder 



## Libqmycroft Installation

- Installing Mycroft-Core (git version):

  - ```bash
    git clone https://github.com/MycroftAI/mycroft-core
    cd mycroft-core
    ./dev_setup.sh
    ```

- Installing Libqmycroft & Libqmycroft-Mock-Skills-Interface (installation script for Mycroft-Core git version only):

  - ```bash
    git clone https://invent.kde.org/AdityaM/libqmycroft
    cd libqmycroft
    ./dev_setup.sh
    ```

- Installing Libqmycroft (manually):

  - ```bash
    git clone https://invent.kde.org/AdityaM/libqmycroft
    cd libqmycroft
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_LIBDIR=lib -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
    make
    sudo make install
    ```

- Installing & Libqmycroft-Mock-Skills-Interface (manually):

  - ```bash
    git clone https://invent.kde.org/AdityaM/libqmycroft
    cd libqmycroft
    cp -R libqmycroft-mock-interface /opt/mycroft/skills/ 
    ```



## Libqmycroft API & Usage

Libqmycroft provides developers with a declarative QML API for Skill Management and Skill Registration.

#### SkillManager Class & Object

The SkillManager Object allows applications to manage and register SkillEntry Objects, The SkillManager class also exposes functions and signals for creating the dynamic skill and listeners for intents and actions that have been invoked by voice interaction. The SkillManager class also exposes additional connection settings like setting the web socket address manually for connecting to a Mycroft-Core instance which can be used in cases of supporting remote instances or different than normal web socket address. 

Important properties of the SkillManager Object:

- **skillNamespace** *(required)*: Name of the dynamic skill to register skill entries against.
- **socketAddress** *(optional)*: Web Socket address of the Mycroft-Core instance. *Defaults to local mycroft-core default address*

Important functions of the SkillManager Object:

- **createSkill() Method** *(required)*: Registers dynamic skill namespace and skill entries with Libqmycroft-Mock-Skills-Interface
- **deleteSkill() Method** *(optional)*: Deletes the registered skill intents and namespace registration from Libqmycroft-Mock-Skills-Interface, it is automatically called in the SkillManager class deconstructor method when parent object is destroyed. *For example: when the application window in which the SkillManager object was registered is closed*  

#### SkillEntry Class & Object

The SkillEntry Object allows applications to create skill entries to be registered during the time of skill creation. SkillEntry Object manages entries for:

- **intent** *(required)*: An intent is the task the user intends to accomplish when they say  something. The role of the intent parser is to extract from the user's  speech key data elements that specify their intent. [Read More](https://mycroft-ai.gitbook.io/docs/skill-development/user-interaction/intents) *Currently should not contain any special characters and should always be defined in lowercase string*

- **voc** *(required)*: ***Absolute Path to an intent file*** with keywords that can activate the intent. Padatious intents uses a series of example sentences to train a machine learning model to identify an intent. *Currently limited to single Padatious intents.* *Filenames should start with the application **namespace_**define_your_intent_here.intent this is to avoid active intent collision* 

- **action** *(required)*: A string based action response based on the intent triggered, notifies the Skill Manager `"onIntentResponse"` via `"action"` variable letting the application then decide what action to take based on the action received.

- **dialog** *(optional)*: A string of text to be spoken by Mycroft when a positive intent response is received for the registered SkillEntry intent.

  

#### Controller Class

The controller class is a minified abstraction of the original Mycroft Controller class from the [Mycroft GUI](https://github.com/MycroftAI/mycroft-gui) project, It is a singleton instance that manages web-socket connections and message exchange to the Mycroft message-bus. It additionally also exports signals for various known mycroft type messages that applications can choose to react towards, for example when mycroft is listening or speaking . 



## Usage Example of Libqmycroft in QML application

***Example QML Implementation***

```QML
...
import Libqmycroft 1.0

Window {
	...
    title: qsTr("Hello Mycroft!")
    
    SkillManager {
        id: skillRegisteration
        skillNamespace: "testskill"
        
        SkillEntry {
            intent: "testoneintent"
            voc: currentPath + "/vocab/testskill_test_intent_one.intent"
            action: "example-action-1"
            dialog: "Intent One Successful"
        }
        
        SkillEntry {
            intent: "testtwointent"
            voc: currentPath + "/vocab/testskill_test_intent_two.intent"
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
                console.log("Got Example 1 Action")
            }
            if(action == "example-action-2"){
                console.log("Got Example 2 Action")
            }
        }
    }
}
```

***Example Voc `"$application-installed-dir/vocab/testskill_test_intent_one.intent"` file implementation***

```
test application example one dynamic intent
test application example one intent
test first intent of dynamic application
```