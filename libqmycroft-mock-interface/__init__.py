import re
import json
import time
from mycroft.skills.core import MycroftSkill, intent_handler, intent_file_handler
from mycroft.messagebus.message import Message

class Libqmycroft(MycroftSkill):
    
    def __init__(self):
        super(Libqmycroft, self).__init__(name="Libqmycroft")
        self.registered_namespaces = []
        global set_dyn_name
        set_dyn_name = self
    
    def initialize(self):
        self.log.info("Libqmycroft Mock Skill Initalized")
        self.add_event("mycroft.create.dynamic.skill", self.handle_dynamic_entry)
        self.add_event("mycroft.dynamic.skill.speak", self.handle_dynamic_skill_speak)
        
    def handle_dynamic_entry(self, message):
        self.log.info("Got Dynamic Skill Registeration Request")
        skill_namespace = message.data.get("namespace")
        skill_parameters = message.data.get("parameters")
        
        if skill_namespace not in self.registered_namespaces:
            self.registered_namespaces.append(skill_namespace)

        self.handle_intent_voc_registeration(skill_namespace, skill_parameters)
        
    def handle_intent_voc_registeration(self, skillnamespace, voc_objects):
        filter_namespace = skillnamespace.lower()
        filter_namespace = re.sub(r"[-()\"#/@;:<>{}`+=~|.!?,]", "", filter_namespace)
        
        for x in voc_objects:
            method_to_make = filter_namespace + "_" + x['intent']
            vars()[method_to_make] = Libqmycroft.gen_handler_method(message=None, namespace=skillnamespace, dynamic_entries=x) 
            setattr(Libqmycroft, method_to_make, vars()[method_to_make])
            method_to_set = getattr(Libqmycroft, method_to_make)
            
            name = self.skill_id + ":" + x['voc'].rsplit('/', 1)[-1]
            self.intent_service.register_padatious_intent(name, x['voc'])
            time.sleep(2)
            self.add_event(name, method_to_set, 'mycroft.skill.handler')
        
        time.sleep(2)

    def event_intent_handler_call(self, namespace, dynamic_entries, message):
        intent_target = namespace
        intent_call = dynamic_entries.get("intent", "")
        intent_caller = message
        self.bus.emit(Message("mycroft.dynamic.skill.intent", {"intent_target": intent_target, "intent_call": intent_call, "intent_caller": intent_caller}))

    def gen_handler_method(message, namespace=None, dynamic_entries=None):
        def _function(message):
            global set_dyn_name
            Libqmycroft.event_intent_handler_call(set_dyn_name, namespace, dynamic_entries, message.data)
        return _function

    def handle_dynamic_skill_speak(self, message):
        dialog_to_speak = message.data.get("dialog", "")
        if dialog_to_speak:
            self.speak(dialog_to_speak)

def create_skill():
    return Libqmycroft()
 
