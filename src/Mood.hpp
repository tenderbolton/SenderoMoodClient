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

class Mood
{
private:
    string id;
    string videoPath;
    float timeToPlay;
    ofVideoPlayer* player;

public:
    
    Mood(string _id, string _videoPath, float _time);
    ~Mood();
    
    string getId();
    float getTimeToPlay();
    void loadVideo();
    ofVideoPlayer* getVideoPlayer();
    void update();
    
};


#endif /* Mood_hpp */
