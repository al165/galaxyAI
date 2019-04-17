#include "constellation.h"

Constellation::Constellation(deque<Celestial> &_stars){
    stars = _stars;
    time = ofGetElapsedTimef();
    alive = true;
}

void Constellation::draw(){
    if(!alive){
        return;
    }
    float delta = ofGetElapsedTimef() - time;

    if(delta > FADE_TIME){
        alive = false;
        return;
    }

    ofSetColor(255);

    ofSetColor(ofMap(delta, 0, FADE_TIME, 255, 0));
    for(unsigned int i = 0; i < stars.size(); i++){
        if(i+1 < stars.size()){
            ofDrawLine(stars[i].pos, stars[i+1].pos);
        }
        ofDrawCircle(stars[i].pos, 4);
    }

}
