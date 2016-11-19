//
//  OSCCommMessage.h
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/30/14.
//
//

#ifndef __OSC_MIDI_Test__OSCCommMessage__
#define __OSC_MIDI_Test__OSCCommMessage__

#include "CommMessage.h"
#include "ofxOsc.h"

class OSCCommMessage: public CommMessage {
    
public:
    
    OSCCommMessage(ofxOscMessage* oscMessage_);
    ~OSCCommMessage();
    
    ofxOscMessage* getOSCMessage();
    

private:
    
    ofxOscMessage* oscMessage;
    
    
};

#endif /* defined(__OSC_MIDI_Test__OSCCommMessage__) */
