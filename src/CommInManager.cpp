//
//  CommInManager.cpp
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/31/14.
//
//

#include "CommInManager.h"

CommInManager::CommInManager(string name_, bool isThreaded_){
    
    commName = name_;
    isConfigured = false;
    isThreaded = isThreaded_;
    buffMessageQuantity = 0;
    
    //initializing input buffer
    
    for(int i=0; i<MAX_BUFF_SIZE;i++){
        inputBuffer[i] = NULL;
    }
    
}

CommInManager::~CommInManager(){
    
}

void CommInManager::setup(){
    samplingMs = 33; // default sampling is 30 fps -- this only works with threaded managers
    setupFromXML();
    isConfigured = true;
}

void CommInManager::start(){
    if(isConfigured && isThreaded){
        isRunning = true;
        startThread(false);    // non blocking
    }
}

void CommInManager::stop(){
    if(isThreaded){
        stopThread();
    }
}

CommMessage* CommInManager::getNextInputMessage(){
    
    CommMessage* p = NULL;
	if(lock()){
		p = popOldestMessage();
		unlock();
	}
    return p;
    
}

void CommInManager::storeMessage(CommMessage* p){
    // must always be called between lock and unlock
    
    if(this->buffMessageQuantity + 1 > MAX_BUFF_SIZE){
        //we pop the first element before inserting
        CommMessage* p = popOldestMessage();
        // popping reduces buffMessageQuantity by 1
        delete p;
    }
    
    //we insert
    this->inputBuffer[(this->buffMessageQuantity-1) + 1] = p;
    this->buffMessageQuantity += 1;
    
}

void CommInManager::threadedFunction(){
    
    while(isThreadRunning()) {
        processInput();
        sleep(samplingMs); // we limit the sampling to avoid wasting resources.
    }
    
    deleteAllMessages();
}

void CommInManager::deleteAllMessages(){
    for(int i=0;i<buffMessageQuantity;i++){
        CommMessage* p = inputBuffer[i];
        delete p;
    }
    this->buffMessageQuantity = 0;
}

void CommInManager::keyPressed (int key){
    // this method should be thread safe using lock and unlock
}

CommMessage* CommInManager::popOldestMessage(){
    // this method is always invoked from another class. so we have to deal with lock and unlock within this function.
    
    CommMessage* p = NULL;
    if(buffMessageQuantity>0){
        // we pop the first element
        p=this->inputBuffer[0];
            
        for(int i=0;i<buffMessageQuantity;i++){
            if(i!=buffMessageQuantity-1){
                // we copy the next element
                inputBuffer[i] = inputBuffer[i+1];
            }
            else{
                // we set the last element in null
                inputBuffer[i] = NULL;
            }
        }
            
        buffMessageQuantity -= 1;
            
    }
    
    return p;
    
}