//
//  OSCCommMessage.cpp
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/30/14.
//
//

#include "OSCCommMessage.h"

OSCCommMessage::OSCCommMessage(ofxOscMessage* oscMessage_): CommMessage(OSC){
    
    this->oscMessage = oscMessage_;
    
}

OSCCommMessage::~OSCCommMessage(){
    
    delete oscMessage;
    
}

ofxOscMessage* OSCCommMessage::getOSCMessage(){
    
    return this->oscMessage;
    
}
