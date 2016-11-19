//
//  CommOutManager.h
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/29/14.
//
//

#ifndef __OSC_MIDI_Test__CommOutManager__
#define __OSC_MIDI_Test__CommOutManager__

#include "ofMain.h"
#include "CommMessage.h"
#include "ofxXmlSettings.h"

#define MAX_BUFF_SIZE 12

class CommOutManager: public ofThread {
    
public:
    
    CommOutManager(string name_, bool isThreaded_);
    ~CommOutManager();
    void setup();
    void start();
    void stop();
    virtual void processInput()=0;
    virtual bool setupFromXML()=0;
    virtual string getType()=0;
    CommMessage* getNextInputMessage();
    void storeMessage(CommMessage* p);
    void deleteAllMessages();
    virtual void keyPressed (int key);
	virtual void singleThreadedUpdate();
	bool getIsThreaded();
    
protected:
    
    void threadedFunction();
    CommMessage* inputBuffer[MAX_BUFF_SIZE];
    bool isRunning;
    bool isConfigured;
    bool isThreaded;
    string commName;
    int samplingMs;
    ofxXmlSettings XML;
	virtual void setupFromThread();
    
private:
    
    int buffMessageQuantity;
    CommMessage* popOldestMessage();
    
};


#endif /* defined(__OSC_MIDI_Test__CommOutManager__) */
