#include "star.h"

Star::Star(){

}

void Star::setup(ofVec2f position, ofVec2f velocity){
    pos = position;
    vel = velocity;
    acc = ofVec2f(0, 0);

    trail.push_front(pos);
    //trail.addVertex(pos.x, pos.y, 0);
    timeElapsed = 0;

    size = 1;
    color.set(255, 255, 255);
}

void Star::update(float dt){
    vel += (acc*dt);
    pos += (vel*dt);
    acc *= 0;

    timeElapsed += dt;

    while(timeElapsed > 30.0){
        // add a point to the trail...
        trail.push_front(pos);
        //trail.addVertex(pos.x, pos.y, 0);
        
        timeElapsed -= 1.0;
    }

    while(trail.size() > TRAIL_LEN){
        trail.pop_back();
        //trail.getVertices().erase(trail.getVertices().begin());
    }

}

void Star::applyForce(ofVec2f force){
    acc += force;
}

void Star::draw(int trails){
    ofSetColor(color);
    ofDrawCircle(pos.x, pos.y, size);
    ofDrawLine(pos, trail[0]);

    //trail.draw();

    for(int i=0; i < trails-1; i++){
        ofSetColor(color, ofMap(i, 0, trails-1, 255, 0));
        ofDrawLine(trail[i], trail[i+1]);
    }
}

