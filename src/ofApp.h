#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "star.h"
#include "celestial.h"

#define NSTARS 2000

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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

                float time;

                Celestial stars[NSTARS];

                ofFbo fbo;

                ofxPanel gui;
                ofxIntSlider starNum;
                ofxIntSlider trails;
                ofxIntSlider fade;
                ofxFloatSlider speed;
                ofxFloatSlider eccentricity;
                ofxFloatSlider size;
                ofxFloatSlider color;
                ofxToggle orbits;
                ofxToggle blur;

                bool bHide;

};
