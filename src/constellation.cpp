#include "constellation.h"
#include <iostream>

Constellation::Constellation(vector<int> _starIDs){
    starIDs = _starIDs;
    time = ofGetElapsedTimef();
}

void Constellation::draw(vector<Celestial>& stars, float fade_time){
    float delta = ofGetElapsedTimef() - time;

    ofSetColor(255, 255, 255, ofMap(delta, 0, fade_time, 200, 0));
    float r = ofMap(delta, 0, fade_time, 4, 1);

    int lastID = -1;
    for(int i : starIDs){
        if(lastID >= 0){
            ofSetLineWidth(2);
            ofDrawLine(stars[i].pos, stars[lastID].pos);
        }
        // ofDrawCircle(stars[i].pos, r);
        lastID = i;
    }

}
