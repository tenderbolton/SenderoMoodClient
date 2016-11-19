//
//  MoodClient.cpp
//  SenderoMoodClient
//
//  Created by Christian Clark on 11/17/16.
//
//

#include "MoodClient.hpp"

MoodClient::MoodClient()
{
    
}


MoodClient::~MoodClient()
{
    
}

void MoodClient::customSetup(map<int, Pixel*>* pixels, vector<Pixel*>* pixelsFast) {
    
    this->settings = new ofxXmlSettings();
    this->currentMood = NULL;
    this->nextMood = NULL;
    onTransition = false;
    transitionTime = 0.0f;
    transitionStartTime=0.0f;
    
    this->imageJoined = NULL;
    
    ofEnableAlphaBlending();
    
    if (settings->loadFile("moodConf.xml")) {
        loadConfig();
    }
    
    fpsToSendMood = 30.0f;
    currentMoodStartTime = 0.0f;
    lastSentMoodTimeStamp = 0.0f;
    
    this->waitForMoodsToLoad();
    this->playCurrentMood();
    this->currentMoodStartTime = ofGetElapsedTimef();
    

}

void MoodClient::loadConfig() {
    settings->pushTag("settings");
    
    int numTransition = settings->getNumTags("transition");
    if (numTransition > 0) {
        this->transitionTime = ofToFloat(settings->getAttribute("transition", "time", "2.0", 0));
    }
    
    int numFBO = settings->getNumTags("fbo_config");
    if (numFBO > 0) {
        float width = ofToFloat(settings->getAttribute("fbo_config", "width", "256", 0));
        float height = ofToFloat(settings->getAttribute("fbo_config", "height", "256", 0));
        fbo.allocate(width, height, GL_RGBA);
        
        imageJoined = new ofImage();
        imageJoined->allocate(fbo.getWidth(),fbo.getHeight(), OF_IMAGE_COLOR_ALPHA);
    }
    
    //loading frame map
    settings->pushTag("framemap");
    int pixelCount = settings->getNumTags("Pixel");
    
    if (pixelCount>0) {
        for (int i = 0; i < pixelCount; i++) {
            int pixId = ofToInt(settings->getAttribute("Pixel", "id", "0", i));
            int x = ofToInt(settings->getAttribute("Pixel", "x", "0", i));
            int y = ofToInt(settings->getAttribute("Pixel", "y", "0", i));
            
            this->pixelMap.insert(pair<int, ofVec2f>(pixId, ofVec2f(x, y)));
        }
    }
    settings->popTag();
    
    //loading commins
    
    settings->pushTag("commins");
    int commInCount = settings->getNumTags("commin");
    for (int i = 0; i < commInCount; i++){
        string commInType = settings->getAttribute("commin", "type", "", i);
        string commInName = settings->getAttribute("commin", "name", "CommInName"+ofToString(i), i);
        int commInPort = settings->getAttribute("commin", "port", 0, i);
        if (commInType == "OSC"){
            OSCInputManager* oim = new OSCInputManager(commInName);
            oim->setup();
            oim->start();
            commins.push_back(oim);
        }
    }
    settings->popTag();
    
    //loading commouts
    
    settings->pushTag("commouts");
    int commOutCount = settings->getNumTags("commout");
    for (int i = 0; i < commOutCount; i++){
        string commOutType = settings->getAttribute("commout", "type", "", i);
        string commOutName = settings->getAttribute("commout", "name", "CommOutName"+ofToString(i), i);
        if (commOutType == "OSC"){
            OSCManager* o = new OSCManager(commOutName);
            o->setup();
            o->start();
            commouts[commOutName] = (CommOutManager*)o;
        }
    }
    settings->popTag();
    
    // loading moods
    
    settings->pushTag("moods");
    int moodsCount = settings->getNumTags("mood");
    
    for (int i = 0; i < moodsCount; i++){
        string name = settings->getAttribute("mood", "name", "", i);
        string videoFile = settings->getAttribute("mood", "video", "", i);
        float timeToPlay = ofToFloat(settings->getAttribute("mood", "time", "0.0", i));
        
        Mood* newMood = new Mood(name,videoFile, timeToPlay);
        
        newMood->loadVideo();
        
        this->moods.insert(pair<string,Mood*>(name,newMood));
        
        if(i==0){
            this->currentMood = newMood;
        }
    
    }
    
    settings->popTag();
    settings->popTag();
}

void MoodClient::update(){
    
    this->currentMood->update();
    
    if(onTransition){
        this->nextMood->update();
    }
    
    this->processFBO();
    
    this->updatePixelsWithCurrentMood();
    
    //updating non threaded commouts
    for (map<string, CommOutManager*>::iterator it=commouts.begin(); it!=commouts.end(); ++it){
        if (!(it->second->getIsThreaded())){
            it->second->singleThreadedUpdate();
        }
    }
    //processing commins
    for (int i = 0; i < commins.size(); i++){
        CommMessage* message = commins[i]->getNextInputMessage();
        if (message != NULL){
            this->handleIncomingCommMessages(message);
        }
        delete message;
    }
    
    
    //calculating current mood time left
    /*
     float timeLeft = this->currentMood->getTimeToPlay() - (ofGetElapsedTimef() - this->currentMoodStartTime);
    
    if(timeLeft<0.0f){
        timeLeft = 0.0f;
    }
     */
    
    // sending current mood
    
    if(ofGetElapsedTimef()-lastSentMoodTimeStamp > 1.0f/fpsToSendMood){
        this->lastSentMoodTimeStamp = ofGetElapsedTimef();
        
        ofxOscMessage* moodHeartBeat = new ofxOscMessage();
        
        moodHeartBeat->setAddress("/mood");
        moodHeartBeat->addStringArg(this->currentMood->getId());
        
        
        //moodHeartBeat->addFloatArg(timeLeft);
        
        this->sendAllCommouts(moodHeartBeat);
        
        if(onTransition){
            
            float currTranTime = ofGetElapsedTimef()-this->transitionStartTime;
            
            if(currTranTime>this->transitionTime){
                currTranTime = transitionTime;
            }
            
            ofxOscMessage* moodTrans = new ofxOscMessage();
            
            moodTrans->setAddress("/moodTransition");
            moodTrans->addStringArg(this->nextMood->getId());
            moodTrans->addFloatArg(currTranTime);
            
            this->sendAllCommouts(moodTrans);
        }
    }
    
    //changing mood if necessary
    /*
    if(timeLeft==0.0f){
        //we advance curent mood
        this->stopCurrentMood();
        this->moveToNextMood();
        this->playCurrentMood();
    }
     */

}

void MoodClient::handleIncomingCommMessages(CommMessage* m){
    
    switch ( m->messageType )
    {
        case OSC:
        {
            OSCCommMessage* o = (OSCCommMessage*) m;
            try {
                if(o->getOSCMessage()->getAddress().compare("/mood/play")==0){
                    this->playCurrentMood();
                }
                
                if(o->getOSCMessage()->getAddress().compare("/mood/stop")==0){
                    this->stopCurrentMood();
                }
                
                if(o->getOSCMessage()->getAddress().compare("/mood/pause")==0){
                    this->pauseCurrentMood();
                }
                
                if(o->getOSCMessage()->getAddress().compare("/mood/next")==0){
                    this->moveToNextMood();
                }
                
                if(o->getOSCMessage()->getAddress().compare("/mood/select")==0){
                    
                    string moodId ="";
                    
                    try{
                        moodId = o->getOSCMessage()->getArgAsString(0);
                    }
                    catch(std::exception const& e){
                        ofLogNotice("Invalid OSC message");
                    }
                    if(moodId.compare("")!=0){
                        this->goToMood(moodId);
                    }
                    
                }
            } catch (  std::exception const& e){
                
                ofLogNotice(e.what());
                
            }
            
            break;
        }
        default:
        {
            ofLogNotice("Unrecognized command.");
        }
    }
    
}

void MoodClient::waitForMoodsToLoad(){
    bool allLoaded = false;
    
    while(!allLoaded){
        
        map<string, Mood*>::iterator it;
        
        for(it=this->moods.begin(); it!= this->moods.end(); it++){
            if(it->second->getVideoPlayer()->isLoaded() == false){
                allLoaded = false;
                sleep(33);
                break;
            }
            allLoaded = true;
        }
        
    }
    
    map<string, Mood*>::iterator itAux;
    
    for(itAux=this->moods.begin(); itAux!= this->moods.end(); itAux++){
        if(itAux->second->getVideoPlayer()->isLoaded() == false){
            itAux->second->getVideoPlayer()->play();
            itAux->second->getVideoPlayer()->setSpeed(0.0f);
        }
    }
    
}

void MoodClient::stopCurrentMood(){
    if(!onTransition){
        this->currentMood->getVideoPlayer()->stop();
        this->currentMood->getVideoPlayer()->setPosition(0.0f);
    }
}

void MoodClient::pauseCurrentMood(){
    if(!onTransition){
        this->currentMood->getVideoPlayer()->setSpeed(0.0f);
        this->currentMood->getVideoPlayer()->setPosition(0.0f);
    }
}

void MoodClient::playCurrentMood(){
    if(!onTransition){
        if(this->currentMood->getVideoPlayer()->isPlaying()){
            this->currentMood->getVideoPlayer()->setSpeed(1.0f);
        }
        else{
            this->currentMood->getVideoPlayer()->play();
            this->currentMood->getVideoPlayer()->setSpeed(1.0f);
        }
        
        this->currentMoodStartTime = ofGetElapsedTimef();
    }
}

void MoodClient::goToMood(string moodId){
    if(!this->onTransition){
        std::map<string,Mood*>::iterator it;
        
        it = this->moods.find(moodId);
        if (it != moods.end()){
            this->nextMood = it->second;
            this->nextMood->getVideoPlayer()->play();
            this->onTransition = true;
            this->transitionStartTime = ofGetElapsedTimef();
        }
    }
}

void MoodClient::moveToNextMood(){
    if(!this->onTransition){
        std::map<string,Mood*>::iterator it;
        
        it = this->moods.find(this->currentMood->getId());
        
        if (it != moods.end()){
            it++;
            if(it==this->moods.end()){
                it=this->moods.begin();
            }
            this->nextMood = it->second;
            this->nextMood->getVideoPlayer()->play();
            this->onTransition = true;
            this->transitionStartTime = ofGetElapsedTimef();
        }
    }
}

void MoodClient::sendAllCommouts(ofxOscMessage * m){
    
    for (map<string, CommOutManager*>::iterator it=commouts.begin(); it!=commouts.end(); ++it){
        //we send a copy of the message to all OSC commout
        ofxOscMessage* messageCopy = new ofxOscMessage(*m);
        OSCCommMessage* newCommOutMessage = new OSCCommMessage(messageCopy);
        
        it->second->storeMessage(newCommOutMessage);
    }
    
    delete m;
}

void MoodClient::draw(){
    
}

void MoodClient::drawPre(){
    
    //calculating current mood time left
    float timeLeft = this->currentMood->getTimeToPlay() - (ofGetElapsedTimef() - this->currentMoodStartTime);
    
    if(timeLeft<0.0f){
        timeLeft = 0.0f;
    }
    
    ofPushMatrix();
    
    ofDisableDepthTest();
    
    ofTranslate(0.0f, 0.0f, 0.0f);
    ofRotate(0.0f);
    
    fbo.draw(20,20);
    //imageJoined->draw(20,300);

    if(this->currentMood->getVideoPlayer()->isPlaying()){
        this->currentMood->getVideoPlayer()->draw(376.0f, 20.0f);
    }
    
    if(onTransition){
        if(this->nextMood->getVideoPlayer()->isPlaying()){
            this->nextMood->getVideoPlayer()->draw(652.0f, 20.0f);
        }
    }

    ofSetColor(255, 50, 50);
    ofDrawBitmapString("// Sendero Mood Client v0.01 //", 10, 560);
    ofSetColor(255);
    ofDrawBitmapString("BLEND", 20, 20);
    ofDrawBitmapString("CURRENT", 376, 20);
    ofDrawBitmapString("NEXT", 652, 20);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 10, 600);
    ofDrawBitmapString("Current Mood: " + this->currentMood->getId(), 10, 620);
    ofDrawBitmapString("Transition: " + ofToString(this->onTransition), 10, 640);
    //ofDrawBitmapString("Press 't' to enable/disable test mode. ", 10, 660);
    //ofDrawBitmapString("Press 'up' or 'down' to change pixel to test. ", 10, 680);
    
    ofEnableDepthTest();
    ofPopMatrix();
    
}

void MoodClient::drawPost(){
    
}

void MoodClient::keyPressed(int key){
    switch (key) {
            
        case 'n':
        {
            this->moveToNextMood();
            break;
        }
        default: {
            break;
        }
    }
}

void MoodClient::exit(){
    
}

void MoodClient::updatePixelsWithCurrentMood(){
    
    if (this->currentMood->getVideoPlayer()->isPlaying()) {
        ofPixels & pxs = this->imageJoined->getPixels();

        vector<Pixel*>::iterator it = this->pixelsFast->begin();
        
        while (it != this->pixelsFast->end()) {
            Pixel* px = *it;
            
            map<int, ofVec2f>::iterator itMap;
            itMap = this->pixelMap.find(px->getId());
            if (itMap != pixelMap.end()) {
                //we have a map for the pixel
                ofVec2f mappedCoordinates = itMap->second;
                
                int pixelSize = 3;
                bool pixelFormatRecognized = false;
                int rOffset = 0;
                int gOffset = 0;
                int bOffset = 0;
                
                switch (pxs.getPixelFormat())
                {
                    case OF_PIXELS_GRAY:
                    {
                        pixelSize = 1;
                        
                        rOffset = 0;
                        gOffset = 0;
                        bOffset = 0;
                        
                        pixelFormatRecognized = true;
                        break;
                    }
                    case OF_PIXELS_GRAY_ALPHA:
                    {
                        pixelSize = 2;
                        
                        rOffset = 0;
                        gOffset = 0;
                        bOffset = 0;
                        
                        pixelFormatRecognized = true;
                        break;
                    }
                    case OF_PIXELS_RGB:
                    {
                        pixelSize = 3;
                        
                        rOffset = 0;
                        gOffset = 1;
                        bOffset = 2;
                        
                        pixelFormatRecognized = true;
                        break;
                    }
                    case OF_PIXELS_BGR:
                    {
                        pixelSize = 3;
                        
                        rOffset = 2;
                        gOffset = 1;
                        bOffset = 0;
                        
                        pixelFormatRecognized = true;
                        break;
                    }
                    case OF_PIXELS_RGBA:
                    {
                        pixelSize = 4;
                        
                        rOffset = 0;
                        gOffset = 1;
                        bOffset = 2;
                        
                        pixelFormatRecognized = true;
                        break;
                    }
                    case OF_PIXELS_BGRA:
                    {
                        pixelSize = 4;
                        
                        rOffset = 2;
                        gOffset = 1;
                        bOffset = 0;
                        
                        pixelFormatRecognized = true;
                        break;
                    }
                    default:
                    {
                        ofLogNotice("Pixel Format from Spout not recognized.");
                    }
                }
                
                if (pixelFormatRecognized) {
                    int rIndex = ((pxs.getWidth() * mappedCoordinates.y) * pixelSize + mappedCoordinates.x * pixelSize + rOffset);
                    int gIndex = ((pxs.getWidth() * mappedCoordinates.y) * pixelSize + mappedCoordinates.x * pixelSize + gOffset);
                    int bIndex = ((pxs.getWidth() * mappedCoordinates.y) * pixelSize + mappedCoordinates.x * pixelSize + bOffset);
                    
                    if (rIndex < pxs.size() && gIndex < pxs.size() && bIndex < pxs.size()) {
                        
                        int r = pxs[rIndex];
                        int g = pxs[gIndex];
                        int b = pxs[bIndex];
                        
                        px->blendRGBA(r, g, b, 255, 1.0f);
                    }
                    else {
                        ofLogError("Pixel index outside of boundaries. Check pixel format.");
                    }
                    
                }
                
            }
            
            it++;
        }
        
    }
}

void MoodClient::processFBO(){
    
    float currTranTime = 0.0f;
    
    if(onTransition){
        currTranTime = ofGetElapsedTimef()-this->transitionStartTime;
        
        if(currTranTime>this->transitionTime){
            currTranTime = transitionTime;
        }
    }
    
    fbo.begin();
    
    ofDisableDepthTest();
    
    ofClear(0, 0, 0, 0);
    
    ofPushMatrix();
    ofColor currentColor = getCurrentBaseColor(currTranTime);
    ofSetColor(currentColor);
    //ofSetColor(255,255,255,255);
    if(this->currentMood->getVideoPlayer()->isPlaying()){
        this->currentMood->getVideoPlayer()->draw(0.0f,0.0f);
    }
    
    if(onTransition){
        
        ofColor tranColor = getCurrentBaseColorTransition(currTranTime);
        
        ofSetColor(tranColor);
        //ofSetColor(255,255,255,255);
        
        if(this->nextMood->getVideoPlayer()->isPlaying()){
            this->nextMood->getVideoPlayer()->draw(0.0f,0.0f);
        }
        
        if(currTranTime == transitionTime){
            this->pauseCurrentMood();
            this->currentMood = this->nextMood;
            this->nextMood = NULL;
            this->onTransition = false;
        }
        
    }
    
    ofPopMatrix();
   
    ofEnableDepthTest();
    
    fbo.end();
    
    ofPixels &p = imageJoined->getPixels();
    fbo.readToPixels(p);
    
    imageJoined->update();
}

ofColor MoodClient::getCurrentBaseColor(float currTranTime){
    ofColor c;
    
    float baseAlpha = 255.0f - ofMap(currTranTime, 0.0f, this->transitionTime, 0.0f, 255.0f);
    //ofLogNotice("base alpha: " + ofToString(baseAlpha));
    c.set(255.0f, 255.0f, 255.0f, baseAlpha);
    //c.set(255.0f, 255.0f, 255.0f, 255.0f);
    //c.setHsb(1.0f, 0.0f, 1.0f);
    return c;
}

ofColor MoodClient::getCurrentBaseColorTransition(float currTranTime){
    ofColor c;
    float alphaTran = ofMap(currTranTime, 0.0f, this->transitionTime, 0.0f, 255.0f);
    //ofLogNotice("next tran: " + ofToString(alphaTran));
    c.set(255.0f, 255.0f, 255.0f, alphaTran);
    //c.setHsb(1.0f, 0.0f, 1.0f);
    return c;
}

