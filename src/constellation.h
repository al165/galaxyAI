#ifndef _CONSELLATION
#define _CONSELLATION
#include "ofMain.h"
#include "celestial.h"

#define FADE_TIME 2

class Constellation {
    public:
        Constellation(deque<Celestial> &_stars);
        void draw();

        deque<Celestial> stars;
        float time;
        bool alive;

};
#endif
