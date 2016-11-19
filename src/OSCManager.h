//
//  OSCManager.h
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/30/14.
//
//

#ifndef __OSC_MIDI_Test__OSCManager__
#define __OSC_MIDI_Test__OSCManager__

#include "ofMain.h"
#include "ofxOsc.h"
#include "CommOutManager.h"
#include "CommMessage.h"
#include "OSCCommMessage.h"

class OSCManager: public CommOutManager {
    
public:
    OSCManager(string name_);
    ~OSCManager();
    
    void processInput();
    bool setupFromXML();
    string getType();
    
private:
    
    ofxOscSender* oscSender;
    string host;
    int port;
    
    long sentIndex;
    
};

#endif /* defined(__OSC_MIDI_Test__OSCManager__) */
