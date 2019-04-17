#include "constellation.h"
#include <iostream>

Constellation::Constellation(vector<int> _starIDs){
    starIDs = _starIDs;
    time = ofGetElapsedTimef();
}

void Constellation::draw(vector<Celestial>& stars){
    float delta = ofGetElapsedTimef() - time;

    ofSetColor(ofMap(delta, 0, FADE_TIME, 255, 0));
    float r = ofMap(delta, 0, FADE_TIME, 4, 1);
    // for(unsigned int i = 0; i < starIDs.size(); i++){
    int lastID = -1;
    for(int i : starIDs){
        // if(i+1 < stars.size()){
        //     ofDrawLine(stars[starIDs[i]].pos, stars[starIDs[i+1]].pos);
        // }
        if(lastID >= 0){
            ofDrawLine(stars[i].pos, stars[lastID].pos);
        }
        ofDrawCircle(stars[i].pos, r);
        lastID = i;
    }

    // cout << " DONE" << endl;

}
