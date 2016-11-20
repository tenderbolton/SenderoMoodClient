//
//  Mood.hpp
//  SenderoMoodClient
//
//  Created by Christian Clark on 11/17/16.
//
//

#ifndef Mood_hpp
#define Mood_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxHapPlayer.h"

class Mood
{
private:
    string id;
    string videoPath;
    float timeToPlay;
    ofxHapPlayer* player;

public:
    
    Mood(string _id, string _videoPath, float _time);
    ~Mood();
    
    string getId();
    float getTimeToPlay();
    void loadVideo();
    ofxHapPlayer* getVideoPlayer();
    void update();
    
};


#endif /* Mood_hpp */
