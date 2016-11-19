//
//  OSCInputManager.h
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/31/14.
//
//

#ifndef __OSC_MIDI_Test__OSCInputManager__
#define __OSC_MIDI_Test__OSCInputManager__

#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "CommInManager.h"
#include "CommMessage.h"
#include "OSCCommMessage.h"

class OSCInputManager: public CommInManager {
    
public:
    OSCInputManager(string name_);
    ~OSCInputManager();
    
    void processInput();
    bool setupFromXML();
    
private:
    
    ofxOscReceiver* oscReceiver;
    int port;
    
};

#endif /* defined(__OSC_MIDI_Test__OSCInputManager__) */
