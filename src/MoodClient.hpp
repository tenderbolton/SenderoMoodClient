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
#include <time.h>


class MoodClient : public SpecificBehaviour
{
public:
    
    map<int, ofVec2f> pixelMap;
    map<string, Mood*> moods;
    map<int, ofColor> dayColors;
    Mood* currentMood;
    Mood* nextMood;
    bool onTransition;
    float transitionTime;
    float transitionStartTime;
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
    
    void pauseCurrentMood();
    
    void playCurrentMood();
    
    void playNextMood();
    
    void goToMood(string moodId);
    
    void moveToNextMood();
    
    void waitForMoodsToLoad();
    
    void updatePixelsWithCurrentMood();
    
    void sendAllCommouts(ofxOscMessage * m);
    
    void loadConfig();
    
    void processFBO();
    
    ofColor getDayColor();
    ofColor getCurrentBaseColor(float currTranTime);
    ofColor getCurrentBaseColorTransition(float currTranTime);
    
    ofxXmlSettings* settings;
    
    float fpsToSendMood;
    float lastSentMoodTimeStamp;
    
    float currentMoodStartTime;
    
    ofFbo fbo;
    
    ofImage* imageJoined;
    
    float maxAlphaNorm;
    
    
    //calendar utils
    int getYear();
    int getMonth();
    int getDay();
    int getWeekDay();
    int getHoursDay();
    int getMinutesDay();
    int getSecondsDay();
    
};


#endif /* MoodClient_hpp */
