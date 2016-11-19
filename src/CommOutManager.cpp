//
//  CommOutManager.cpp
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/29/14.
//
//

#include "CommOutManager.h"


CommOutManager::CommOutManager(string name_, bool isThreaded_){
    
    commName = name_;
    isConfigured = false;
    isThreaded = isThreaded_;
    buffMessageQuantity = 0;
    
    //initializing input buffer
    
    for(int i=0; i<MAX_BUFF_SIZE;i++){
        inputBuffer[i] = NULL;
    }
    
}

CommOutManager::~CommOutManager(){
    
}

void CommOutManager::setup(){
    samplingMs = 33; // default sampling is 30 fps -- this only works with threaded managers
    setupFromXML();
    isConfigured = true;
}

void CommOutManager::start(){
    if(isConfigured && isThreaded){
        isRunning = true;
        startThread(false);    // non blocking
    }
}

void CommOutManager::stop(){
    if(isThreaded){
        stopThread();
    }
}

CommMessage* CommOutManager::getNextInputMessage(){
    
    CommMessage* p = NULL;
    p = popOldestMessage();
    return p;
    
}

void CommOutManager::storeMessage(CommMessage* p){
    
    // this method is always invoked from another class. so we have to deal with lock and unlock within this function.
    
    if(lock()){
        if(this->buffMessageQuantity + 1 > MAX_BUFF_SIZE){
            //we pop the first element before inserting
            CommMessage* p = popOldestMessage();
            // popping reduces buffMessageQuantity by 1
            delete p;
        }
        
        //we insert
        this->inputBuffer[(this->buffMessageQuantity-1) + 1] = p;
        this->buffMessageQuantity += 1;
        unlock();
    }
    
}

void CommOutManager::threadedFunction(){
    setupFromThread();
    while(isThreadRunning()) {
        processInput();
        sleep(samplingMs); // we limit the sampling to avoid wasting resources.
    }
    
    deleteAllMessages();
}

void CommOutManager::deleteAllMessages(){
    for(int i=0;i<buffMessageQuantity;i++){
        CommMessage* p = inputBuffer[i];
        delete p;
    }
    this->buffMessageQuantity = 0;
}

void CommOutManager::keyPressed (int key){
    // this method should be thread safe using lock and unlock
}

CommMessage* CommOutManager::popOldestMessage(){
    // must always be called between lock and unlock
    
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

bool CommOutManager::getIsThreaded(){
	return this->isThreaded;
}

void CommOutManager::setupFromThread(){}

void CommOutManager::singleThreadedUpdate(){}