#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxGui.h"
#include "star.h"
#include "celestial.h"
#include "constellation.h"

#define NSTARS 2000
#define CHANNELS 16

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

                void eccentricityChange(float &_ecc);
								void newMidiMessage(ofxMidiMessage& eventArgs);

                // float time;
                ofFbo fbo;

                ofxPanel gui;
                ofParameter<int> starNum;
                ofParameter<int> trails;
                ofParameter<int> fade;
                ofParameter<float> speed;
                ofParameter<float> eccentricity;
                ofParameter<float> size;
								ofParameter<float> constellationTime;
                ofParameter<bool> orbits;

                bool bHide;

                vector<Celestial> stars;
								deque<Constellation> constellations;

								ofxMidiIn midiIn;
								std::vector<ofxMidiMessage> midiMessages[CHANNELS];
								std::size_t maxMessages = 10;

};
