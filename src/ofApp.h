#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxGui.h"
#include "ofxDatGui.h"
#include "star.h"
#include "celestial.h"
#include "constellation.h"

#define NSTARS 2000
#define CHANNELS 16
#define PULSE_TIME 0.5

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void onNoteInDropdownEvent(ofxDatGuiDropdownEvent e);
		void onCCInDropdownEvent(ofxDatGuiDropdownEvent e);

    void eccentricityChange(float &_ecc);
		void newMidiMessage(ofxMidiMessage& eventArgs);
		void noteMidiMessage(ofxMidiMessage& eventArgs);
		void ccMidiMessage(ofxMidiMessage& eventArgs);

    // float time;
    ofFbo fbo;

    ofxPanel gui;
    ofParameter<int> starNumTarget;
    ofParameter<int> trailsTarget;
    ofParameter<int> fadeTarget;
    ofParameter<int> speedTarget;
    ofParameter<float> eccentricityTarget;
    ofParameter<float> sizeTarget;
		ofParameter<float> constellationTime;
    ofParameter<bool> orbits;
		ofParameter<float> beatBrightness;

		ofxDatGui* guiPanel;
		ofxDatGuiDropdown* midiNoteDevice;
		ofxDatGuiDropdown* midiCCDevice;

		int starNum;
		int trails;
		int fade;
		int speed;
		float eccentricity;
		float size;
		bool pulse = false;
		float pulse_start;

    bool bHide;

    vector<Celestial> stars;
		deque<Constellation> constellations;

		ofxMidiIn midiCCIn;
		ofxMidiIn midiNoteIn;
		std::vector<ofxMidiMessage> midiMessages[CHANNELS];
		std::size_t maxMessages = 10;

		ofxMidiClock clock;
		bool clockRunning = false;
		unsigned int beats = 0;
		unsigned int quarters = 0;

};
