#include "Logger.h"

Logger::Logger(){
    //gui = new ofxUICanvas();
	width = 350;
	height = 274;
	showLog = true;
	maxMessageSize = 50;
	log;
}

Logger::~Logger(){}

void Logger::setup(){
	gui2 = new ofxUICanvas(ofGetWidth() - width, 10, width, 35);
	gui = new ofxUIScrollableCanvas(ofGetWidth() - width, 35.0f, width, height);
    //gui->setScrollAreaToScreen();
    gui->setScrollableDirections(false, true);
	gui->setDamping(0.5f);
	gui->setSnapping(false);
	ofxUILabel* l = new ofxUILabel("LogPanelTitleLabel", "Log Panel", OFX_UI_FONT_LARGE);
	gui2->addWidgetPosition(l, OFX_UI_WIDGET_POSITION_DOWN, OFX_UI_ALIGN_CENTER);
	//l->setPadding(18.0f);
	//gui2->addLabel("LogPanelTitleLabel", "Log Panel");
	//ofxUILabel* title = (ofxUILabel*)gui2->getWidget("LogPanelTitleLabel");
	//title->
	closeButton = new ofxUIImageButton(ofGetWidth() - 16.0f, 0.0f, 16.0f, 16.0f, false, "GUI/toggleon.png", "CloseButton");
	//closeButton->getRect()->setX(ofGetWidth() - 32.0f);
	closeButton->setLabelVisible(false);
	gui2->addWidgetPosition(closeButton, OFX_UI_WIDGET_POSITION_RIGHT, OFX_UI_ALIGN_RIGHT);
	//gui2->addWidgetEastOf(closeButton, "LogPanelTitleLabel");
	gui2->addSpacer(width, 1);
	nameOfLastAdded = "firstSpacer";
	gui->addSpacer(nameOfLastAdded, 1);
	//gui->setPosition(ofGetWidth() - width, 0);
	gui->setVisible(true);
    gui->autoSizeToFitWidgets();
	ofAddListener(gui2->newGUIEvent, this, &Logger::guiEvent); 
}

void Logger::update(){
	if (!log.empty()){
		LoggerEventArgs lea = log.front();
		ofxUILabel* l;
		int mSplit = 1;
		if(lea.message.size() > maxMessageSize){
			vector<string> messages = Split(lea.message, maxMessageSize);
			mSplit = messages.size();
			for (int i = 0; i < messages.size(); i++){
				string newName = "newLabel_" + ofToString(i) + ofToString(ofGetElapsedTimeMillis());
				l = new ofxUILabel(newName, messages[i], OFX_UI_FONT_SMALL);
				gui->addWidgetSouthOf(l, nameOfLastAdded);
				if (lea.level == error){
					l->setColorFill(ofColor(150, 0, 0));
				}
				else if (lea.level == warning){
					l->setColorFill(ofColor(250, 250, 0));
				}
				else if (lea.level == success){
					l->setColorFill(ofxUIColor(0, 150, 0));
				}
				//l->setAutoSize(false);
				nameOfLastAdded = newName;
			}
			
		}
		else{
			string newName = "newLabel" + ofToString(ofGetElapsedTimeMillis());
			l = new ofxUILabel(newName, lea.message, OFX_UI_FONT_SMALL);
			gui->addWidgetSouthOf(l, nameOfLastAdded);
			if (lea.level == error){
				l->setColorFill(ofColor(150, 0, 0));
			}
			else if (lea.level == warning){
				l->setColorFill(ofColor(250, 250, 0));
			}
			else if (lea.level == success){
				l->setColorFill(ofxUIColor(0, 150, 0));
			}
			//l->setAutoSize(false);
			nameOfLastAdded = newName;
		}
		gui->addWidgetSouthOf(new ofxUISpacer(width, 2), nameOfLastAdded);
		//gui->addLabel(nameOfLastAdded, lea.message);
		//gui->addSpacer(width, 5);
		log.pop();
		gui->autoSizeToFitWidgets();
		gui->setHeight(height);
		//works if growing upwards (adding at the top)
		/*float currentY = gui->getRect()->getY();
		float widgetHeight = l->getPaddingRect()->getHeight() + 2;
		gui->getRect()->setY(currentY + widgetHeight);*/
		//for adding at the bottom
		vector<ofxUIWidget*> wdgts = gui->getWidgetsOfType(OFX_UI_WIDGET_LABEL);
		if(wdgts.size() > 18){
			float currentY = gui->getRect()->getY();
			float widgetHeight = (l->getPaddingRect()->getHeight() + 2) * mSplit;
			gui->getRect()->setY(currentY - widgetHeight);
		}
		/*if(wdgts.size() > 150){
			vector<ofxUIWidget *>::iterator it = wdgts.begin();
			vector<ofxUIWidget *>::iterator eit = wdgts.begin() + 50;
			for(; it != eit; ++it) {
				ofxUIWidget *w = (*it);
				delete w;
			}
			gui->clearWidgets();
			gui->resetPlacer();
		}*/
		//gui->update();
	}
}

void Logger::addMessageToLog(logLevel level, string message){
	LoggerEventArgs lea;
	lea.level = level;
	lea.message = message;
	log.push(lea);
}

void Logger::guiEvent(ofxUIEventArgs& e){
	if(e.widget->getName() == "FULLSCREEN")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        ofSetFullscreen(toggle->getValue());   
    }
	else if(e.widget->getName() == "CloseButton"){
		ofxUIImageButton* b = (ofxUIImageButton*) e.widget;
		if (b->getValue() == 0){
			showLog = !showLog;
			gui->setVisible(showLog);
		}
	}
}

void Logger::exit(){
	delete gui;
	//log = new queue<LoggerEventArgs>();
}

bool Logger::isLogOpen(){
	return gui->isVisible();
}

vector<string> Logger::Split(const string& str, int splitLength)
{
   int NumSubstrings = str.length() / splitLength;
   vector<string> ret;

   for (auto i = 0; i < NumSubstrings; i++)
   {
        ret.push_back(str.substr(i * splitLength, splitLength));
   }

   // If there are leftover characters, create a shorter item at the end.
   if (str.length() % splitLength != 0)
   {
        ret.push_back(str.substr(splitLength * NumSubstrings));
   }
   return ret;
}

void Logger::handleResize(){
	gui->setPosition(ofGetWidth() - width - 10.0f, 35.0f);
	gui2->setPosition(ofGetWidth() - width - 10.0f, 10.0f);
}