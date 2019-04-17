#include "ofApp.h"
#include "math.h"
#include <iostream>

//--------------------------------------------------------------
void ofApp::setup(){
    ofResetElapsedTimeCounter();
    //ofSetFrameRate(30);
    // setup GUI...
    eccentricity.addListener(this, &ofApp::eccentricityChange);
    gui.setup();
    gui.add(starNum.setup("number", 1000, 1, NSTARS));
    gui.add(trails.setup("trails", 1, 1, TRAIL_LEN));
    gui.add(fade.setup("fade", 255, 1, 255));
    gui.add(speed.setup("speed", 100, 0, 2000));
    gui.add(eccentricity.setup("eccentricity", 0, 0, 0.8));
    gui.add(size.setup("size", 0, 0, 1));
    gui.add(color.setup("color", 0, 0, 1));
    gui.add(orbits.setup("orbits", false));
    gui.add(blur.setup("blur", false));

    bHide = false;

    ofBackground(0);
    ofSetCircleResolution(50);

    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    fbo.begin();
        ofClear(255, 255, 255, 0);
    fbo.end();

    for(int i=0; i<NSTARS; i++){
        float r = ofRandom(100, ofGetHeight()/2);
        float theta = ofRandom(0, 2*PI);

        if(ofRandom(1) < 1.0) {
            theta = ofMap(r, 100, ofGetHeight()/2, 0, -2*PI);
            theta *= ofRandom(0.6, 1.0);

            if(ofRandom(1) < 0.5){
                theta += PI;
            }
        }

        stars[i].setup(0, r, theta);
    }

    // setup Midi connection
    midiIn.listInPorts();
    midiIn.openPort(0);
    // midiIn.openPort("LoopBe Internal MIDI");

    midiIn.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    midiIn.setVerbose(true);

}

//--------------------------------------------------------------
void ofApp::update(){
    //float dt = speed*ofGetLastFrameTime();

    for(int i=0; i<NSTARS; i++){
        stars[i].update(speed);
    }

    // if messages greater than 5, draw constelation!
    if(midiMessages.size() > 5){
        deque<Celestial> constellation;

        for(unsigned int i=0; i<midiMessages.size(); i++){
        		ofxMidiMessage &message = midiMessages[i];
            constellation.push_front(stars[message.pitch]);
        }

        Constellation newConst = Constellation(constellation);
        constellations.push_back(newConst);
        cout << constellation.size() << " " << constellations.size() << endl;
        // deque<int> star_ids;
        // for(unsigned int i = 0; i < midiMessages.size(); i++) {
        // 		ofxMidiMessage &message = midiMessages[i];
        //     star_ids.push_front(message.pitch);
        // }
        //
        midiMessages.clear();
        //
        // ofSetColor(255);
        //
        // for(unsigned int i = 0; i < star_ids.size(); i++){
        //     if(i+1 < star_ids.size()){
        //         ofDrawLine(stars[star_ids[i]].pos, stars[star_ids[i+1]].pos);
        //     }
        //     ofDrawCircle(stars[star_ids[i]].pos, 8);
        // }

    }

    // clear old constellations
    for(unsigned int i=0; i<constellations.size(); i++){
        if(!constellations[i].alive){
            constellations.erase(constellations.begin() + i);
            cout << "deleted " << i << endl;
        }
    }

    fbo.begin();
        // fade the stars..
        ofFill();
        ofSetColor(0, 0, 0, fade);
        ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());


        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        for(int i=0; i<starNum; i++){
            stars[i].draw(trails, orbits, blur, size, color);
        }

        for(Constellation c : constellations){
            c.draw();
        }

    fbo.end();
}

void ofApp::eccentricityChange(float &_ecc){
    for(int i=0; i<NSTARS; i++){
        stars[i].updateEcc(eccentricity);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();

    ofSetColor(255, 255, 255);
    fbo.draw(0, 0);

    if(!bHide){
        gui.draw();
    }

    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

    ofSetColor(255);
    ofDrawLine(-20, 0, 20, 0);
    ofDrawLine(0, -20, 0, 20);





}

//--------------------------------------------------------------
void ofApp::exit() {
    midiIn.closePort();
    midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

    if(msg.status == MIDI_NOTE_ON){
      midiMessages.push_back(msg);
    }


    while(midiMessages.size() > maxMessages){
        midiMessages.erase(midiMessages.begin());
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
      case ' ':
        bHide = !bHide;
        break;

      case '?':
        midiIn.listInPorts();
        break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
