#ifndef _CONSELLATION
#define _CONSELLATION
#include "ofMain.h"
#include "celestial.h"

#define FADE_TIME 2

class Constellation {
    public:
        // Constellation(deque<Celestial> &_stars);
        Constellation(vector<int> _starIDs);
        void draw(vector<Celestial>& stars);
        float time;

    private:
        vector<int> starIDs;


};
#endif
