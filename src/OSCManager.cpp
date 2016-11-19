//
//  OSCManager.cpp
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/30/14.
//
//

#include "OSCManager.h"



OSCManager::OSCManager(string name_):CommOutManager(name_,true){
    
}

OSCManager::~OSCManager(){
    delete this->oscSender;
}

void OSCManager::processInput(){
    
    if(lock()){
        
        CommMessage* p = getNextInputMessage();
        
        unlock();
        
        if(p!=NULL){
            if(p->getMessageType()==OSC){
                //we treat the message
                OSCCommMessage* o = (OSCCommMessage*) p;
                
                o->getOSCMessage()->addStringArg(ofToString(this->sentIndex));
                
                oscSender->sendMessage(*(o->getOSCMessage()));
                
                this->sentIndex += 1;
                
                delete o;
                
            }
            else{
                //we delete the message since we dont know how to process it
                delete p;
            }
        }
    }
    
}

bool OSCManager::setupFromXML(){
    bool result = true;
    
    if( XML.loadFile("commManager_" + commName + ".xml") ){
        
        //get osc output ip
        
        oscSender = new ofxOscSender();
        
        string ip = XML.getAttribute("OSC_SETTINGS","host","127.0.0.1");
        int pt = ofToInt(XML.getAttribute("OSC_SETTINGS","port","5000"));
        
        this->host = ip;
        this->port = pt;
        
        oscSender->setup(host, port);
        sentIndex = 0;
    }
    else{
        result = false;
    }
    
    return result;
    
}

string OSCManager::getType(){
    return "OSC";
}
