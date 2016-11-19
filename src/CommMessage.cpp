//
//  CommMessage.cpp
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/29/14.
//
//

#include "CommMessage.h"



CommMessage::CommMessage(CommMessageType messageType_){
    
    messageType = messageType_;
    
}

CommMessage::~CommMessage(){
    

}

CommMessageType CommMessage::getMessageType(){
    return messageType;
}