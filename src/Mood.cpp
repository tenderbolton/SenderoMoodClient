//
//  Mood.cpp
//  SenderoMoodClient
//
//  Created by Christian Clark on 11/17/16.
//
//

#include "Mood.hpp"


Mood::Mood(string _id, string _videoPath, float _time){
    this->id=_id;
    this->videoPath=_videoPath;
    this->timeToPlay=_time;
    this->player = new ofVideoPlayer();
    this->player->setLoopState(OF_LOOP_NORMAL);
}

Mood::~Mood(){
    
}

string Mood::getId(){
    return this->id;
}

float Mood::getTimeToPlay(){
    return this->timeToPlay;
}

void Mood::loadVideo(){
    this->player->load(this->videoPath);
}

ofVideoPlayer* Mood::getVideoPlayer(){
    return this->player;
}

void Mood::update(){
    this->player->update();
}