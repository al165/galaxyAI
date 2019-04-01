#ifndef _CELESTIAL
#define _CELESTIAL
#include "ofMain.h"

#define TRAIL_LEN 15

class Celestial {
    public:
        void setup(float _ecc, float _a, float _gRotation);
        void update(float dTheta);
        void draw(int trails, bool orbits, bool _blur, float scale, float _color);
        void updateEcc(float _ecc);
        void updateBBox();

        ofVec2f pos;

        float r;                // current distance from the main focus
        float ecc;              // eccentricity, where 0 = circle
        float numerator;        // a*(1.0-ecc^2)
        float a;                // the semi-major axis
        float b;
        float center;           // how offset the center is from the main focus
        float orbitAngle;       // what angle throughout the orbit it is at
        float globalRotation;   // roatation of the whole orbit from the center

        deque<ofVec2f> trail;
        int size;
        ofColor color;
        ofImage blur;

        Celestial();

    private:
};
#endif
