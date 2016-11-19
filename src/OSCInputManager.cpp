//
//  OSCInputManager.cpp
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/31/14.
//
//

#include "OSCInputManager.h"


OSCInputManager::OSCInputManager(string name_):CommInManager(name_,true){

}

OSCInputManager::~OSCInputManager(){
    delete this->oscReceiver;
}

void OSCInputManager::processInput(){
    if(lock()){
        
        while(oscReceiver->hasWaitingMessages()){
        
            ofxOscMessage* m = new ofxOscMessage();
            
            oscReceiver->getNextMessage(m);
            
            OSCCommMessage* o = new OSCCommMessage(m);
            
            this->storeMessage(o);
        
        }
        
        unlock();
        
    }

}

bool OSCInputManager::setupFromXML(){
    bool result = true;
    
    if( XML.loadFile("commInputManager_" + commName + ".xml") ){
        
        this->oscReceiver = new ofxOscReceiver();
    
        int pt = ofToInt(XML.getAttribute("OSC_INPUT_SETTINGS","port","5000"));
        
        this->port = pt;
        
        oscReceiver->setup(port);
    }
    else{
        result = false;
    }
    
    return result;

}