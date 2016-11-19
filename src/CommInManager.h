//
//  CommInManager.h
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/31/14.
//
//

#ifndef __OSC_MIDI_Test__CommInManager__
#define __OSC_MIDI_Test__CommInManager__

#include "ofMain.h"
#include "CommMessage.h"
#include "ofxXmlSettings.h"

#define MAX_BUFF_SIZE 12

class CommInManager: public ofThread {
    
public:
    
    CommInManager(string name_, bool isThreaded_);
    ~CommInManager();
    void setup();
    void start();
    void stop();
    virtual void processInput()=0;
    virtual bool setupFromXML()=0;
    CommMessage* getNextInputMessage();
    void storeMessage(CommMessage* p);
    void deleteAllMessages();
    virtual void keyPressed (int key);
    
protected:
    
    void threadedFunction();
    CommMessage* inputBuffer[MAX_BUFF_SIZE];
    bool isRunning;
    bool isConfigured;
    bool isThreaded;
    string commName;
    int samplingMs;
    ofxXmlSettings XML;
    
private:
    
    int buffMessageQuantity;
    CommMessage* popOldestMessage();
    
};


#endif /* defined(__OSC_MIDI_Test__CommInManager__) */
