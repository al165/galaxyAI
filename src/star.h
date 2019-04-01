#ifndef _STAR
#define _STAR
#include "ofMain.h"

#define TRAIL_LEN 8

class Star {
    public:
        void setup(ofVec2f position, ofVec2f velocity);
        void update(float dt);
        void applyForce(ofVec2f force);
        void draw(int trails);

        ofVec2f pos;
        ofVec2f vel;
        ofVec2f acc;

        deque<ofVec2f> trail;
        //ofPolyline trail;

        float timeElapsed;

        int size;
        ofColor color;

        Star();

    private:
};
#endif
