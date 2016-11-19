#pragma once

#include "ofMain.h"
#include "ofxUI.h"

enum logLevel {
	info,
	success,
	warning,
	error
};

class LoggerEventArgs{
public:
	logLevel level;
	string message;
};

class Logger{
public:
	Logger();
	~Logger();
	void setup();
	void update();
	void addMessageToLog(logLevel level, string message);
	void guiEvent(ofxUIEventArgs& e);
	void exit();
	bool isLogOpen();
	vector<string> Split(const std::string& str, int splitLength);
	void handleResize();

	string name;
	ofxUICanvas* gui2;
	ofxUIScrollableCanvas* gui;
	ofxUIImageButton* closeButton;
	ofEvent<LoggerEventArgs> newLoggerEvent;
	float width;
	float height;
	bool showLog;
	int maxMessageSize;

protected:
	queue<LoggerEventArgs> log;
	string nameOfLastAdded;
};