//
//  MoodClient.hpp
//  SenderoMoodClient
//
//  Created by Christian Clark on 11/17/16.
//
//

#ifndef MoodClient_hpp
#define MoodClient_hpp

#include <stdio.h>
#include "ofMain.h"
#include "SpecificBehaviour.h"
#include "ofxXmlSettings.h"
#include <map>
#include "ofxGui.h"
#include "CommOutManager.h"
#include "CommInManager.h"
#include "Mood.hpp"
#include "OSCManager.h"
#include "OSCInputManager.h"


class MoodClient : public SpecificBehaviour
{
public:
    
    map<int, ofVec2f> pixelMap;
    map<string, Mood*> moods;
    Mood* currentMood;
    map<string, CommOutManager*> commouts;
    vector<CommInManager*> commins;
    
    MoodClient();
    ~MoodClient();
    
    void customSetup(map<int, Pixel*>* pixels, vector<Pixel*>* pixelsFast);
    
    void update();
    void draw();
    void drawPre();
    void drawPost();
    void keyPressed(int key);
    void exit();
    
    void handleIncomingCommMessages(CommMessage* m);
    
    void stopCurrentMood();
    
    void playCurrentMood();
    
    void assignMood(string moodId);
    
    void moveToNextMood();
    
    void waitForMoodsToLoad();
    
    void updatePixelsWithCurrentMood();
    
    void sendAllCommouts(ofxOscMessage * m);
    
    void loadConfig();
    
    ofxXmlSettings* settings;
    
    float fpsToSendMood;
    float lastSentMoodTimeStamp;
    
    float currentMoodStartTime;
    
};


#endif /* MoodClient_hpp */
