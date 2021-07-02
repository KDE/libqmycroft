import re
import json
import time
import os
import subprocess
from mycroft.skills.core import (MycroftSkill,
                                 intent_handler, intent_file_handler)
from mycroft.messagebus.message import Message
from mycroft.session import SessionManager
from mycroft.stt import MycroftSTT
from speech_recognition import WavFile, AudioData


class Libqmycroft(MycroftSkill):

    def __init__(self):
        super(Libqmycroft, self).__init__(name="Libqmycroft")
        self.registered_namespaces = []
        global set_dyn_name
        set_dyn_name = self

    def initialize(self):
        self.log.info("Libqmycroft Mock Skill Initalized")
        self.add_event(
            "mycroft.create.dynamic.skill",
            self.handle_dynamic_entry)
        self.add_event(
            "mycroft.delete.dynamic.skill",
            self.handle_dynamic_entry_deregister)
        self.add_event(
            "mycroft.dynamic.skill.speak",
            self.handle_dynamic_skill_speak)
        self.add_event(
            "libqmycroft.request.transcribe",
            self.transcribe_audio)

    def handle_dynamic_entry(self, message):
        self.log.info("Got Dynamic Skill Registeration Request")
        skill_namespace = message.data.get("namespace")
        skill_parameters = message.data.get("parameters")

        if skill_namespace not in self.registered_namespaces:
            self.registered_namespaces.append({"namespace": skill_namespace})
        else:
            # Only inform the application of a possible namespace clash
            # Consider all intents part of the same namespace incase of clash
            self.bus.emit(
                "mycroft.dynamic.skill.error", {
                    "errorString": "Warning Namespace Exist"})

        self.handle_intent_voc_registeration(skill_namespace, skill_parameters)

    def handle_intent_voc_registeration(self, skillnamespace, voc_objects):
        filter_namespace = skillnamespace.lower()
        filter_namespace = re.sub(
            r"[-()\"#/@;:<>{}`+=~|.!?,]", "", filter_namespace)

        for x in voc_objects:
            method_to_make = filter_namespace + "_" + x['intent']
            vars()[method_to_make] = Libqmycroft.gen_handler_method(
                message=None, namespace=skillnamespace, dynamic_entries=x)
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
        self.bus.emit(Message("mycroft.dynamic.skill.intent",
                              {"intent_target": intent_target,
                               "intent_call": intent_call,
                               "intent_caller": intent_caller}))

    def gen_handler_method(message, namespace=None, dynamic_entries=None):
        def _function(message):
            global set_dyn_name
            Libqmycroft.event_intent_handler_call(
                set_dyn_name, namespace, dynamic_entries, message.data)
        return _function

    def handle_dynamic_skill_speak(self, message):
        dialog_to_speak = message.data.get("dialog", "")
        if dialog_to_speak:
            self.speak(dialog_to_speak)

    def handle_dynamic_entry_deregister(self, message):
        self.log.info("Got Dynamic Skill Delete Request")
        skill_namespace = message.data.get("namespace")
        skill_parameters = message.data.get("parameters")
        if self.__check_namespace_exist(skill_namespace):
            index = self.__get_namespace_index(skill_namespace)
            self.registered_namespaces.pop(index)

        filter_namespace = skill_namespace.lower()
        filter_namespace = re.sub(
            r"[-()\"#/@;:<>{}`+=~|.!?,]", "", filter_namespace)

        for x in skill_parameters:
            name = self.skill_id + ":" + x['voc'].rsplit('/', 1)[-1]
            self.intent_service.detach_intent(name)

    #Add transcribe support
    def transcribe_audio(self, message):
        self.log.info("Got Transcribe Request")
        stt = MycroftSTT()
        request_id = message.data["responseid"]
        audio_file = message.data["file"]
        audio_wav_file = "/tmp/transcribe_in.wav"
        subprocess.call(["sox","-r","16000", "-t", "sw", "-e", "signed", "-c", "1", "-b", "16", audio_file, audio_wav_file])
        audio = self.__get_audio_data(audio_wav_file)
        text = stt.execute(audio, self.lang)
        self.bus.emit(Message("libqmycroft.request.transcribe.result", {"targetResponseId": request_id, "targetResult": text}))

    def __get_audio_data(self, audiofile):
        wavfile = WavFile(audiofile)
        with wavfile as source:
            return AudioData(source.stream.read(), wavfile.SAMPLE_RATE, wavfile.SAMPLE_WIDTH)

    def __check_namespace_exist(self, namespace):
        list_namespaces = list(
            self.__locate_key(
                self.registered_namespaces,
                'namespace'))
        if namespace in list_namespaces:
            return True
        else:
            return False

    def __get_namespace_index(self, namespace):
        for i, skill in enumerate(self.registered_namespaces):
            if skill['namespace'] == namespace:
                return i
        return None

    def __locate_key(self, node, kv):
        if isinstance(node, list):
            for i in node:
                for x in self.__locate_key(i, kv):
                    yield x
        elif isinstance(node, dict):
            if kv in node:
                yield node[kv]
            for j in node.values():
                for x in self.__locate_key(j, kv):
                    yield x


def create_skill():
    return Libqmycroft()
