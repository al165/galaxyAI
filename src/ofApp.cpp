#include "ofApp.h"
#include "math.h"
#include <iostream>

//--------------------------------------------------------------
void ofApp::setup(){

    cout << "Starting..." << endl;

    ofResetElapsedTimeCounter();
    //ofSetFrameRate(30);
    cout << "Setting up GUI... ";
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

    cout << "DONE" << endl;

    bHide = false;

    ofBackground(0);
    ofSetCircleResolution(50);

    cout << "Setting up FBO...";

    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    fbo.begin();
        ofClear(255, 255, 255, 0);
    fbo.end();

    cout << "DONE" << endl;

    cout << "Setting up stars...";

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

        Celestial star = Celestial(0, r, theta);
        // cout << i << " ";
        stars.push_back(star);

        // stars[i].setup(0, r, theta);
    }

    cout << "DONE" << endl;

    cout << "stars.size(): " << stars.size() << endl;

    cout << stars[stars.size()-1].pos << endl;

    // setup Midi connection
    cout << "Setting up MIDI connection..." << endl;
    midiIn.listInPorts();
    // midiIn.openPort(0);
    midiIn.openPort("LoopBe Internal MIDI 0");

    midiIn.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    midiIn.setVerbose(true);

    cout << "DONE" << endl;

}

//--------------------------------------------------------------
void ofApp::update(){
    //float dt = speed*ofGetLastFrameTime();
    // cout << "UPDATE" << endl;

    // for(int i=0; i<NSTARS; i++){
    for(Celestial& star : stars){
        // stars[i].update(speed);
        star.update(speed);
    }

    // if messages greater than 5, draw constelation!
    for(unsigned int j=0; j < CHANNELS; j++){
        if(midiMessages[j].size() > 5){
            // cout << "creating constellation...";
            vector<int> constellation;

            for(unsigned int i=0; i<midiMessages[j].size(); i++){
            		ofxMidiMessage &message = midiMessages[j][i];
                constellation.push_back(message.pitch + 50*j);
            }

            Constellation newConst = Constellation(constellation);
            constellations.push_back(newConst);

            midiMessages[j].clear();

            // cout << "DONE" << endl;

        }

    }

    // clear old constellations
    // cout << "constellations.size() = " << constellations.size();
    deque<int> deadConsts;
    for(unsigned int i=0; i<constellations.size(); i++){
        if(constellations[i].time + FADE_TIME < ofGetElapsedTimef()){
            deadConsts.push_front(i);
            // constellations.erase(constellations.begin() + i);
            // cout << "deleted " << i << endl;
        }
    }

    for(int i : deadConsts){
        constellations.erase(constellations.begin() + i);
    }

    fbo.begin();
        // fade the stars..
        ofFill();
        ofSetColor(0, 0, 0, fade);
        ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());


        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

        for(Constellation& c : constellations){
            c.draw(stars);
        }

        // for(Celestial& star : stars){
        //     star.draw(trails, orbits, blur, size, color);
        // }
        for(int i=0; i<starNum; i++){
            stars[i].draw(trails, orbits, blur, size, color);
        }


    fbo.end();
}

//--------------------------------------------------------------
void ofApp::eccentricityChange(float &_ecc){
    for(Celestial& star : stars){
        star.updateEcc(eccentricity);
    }
    // for(int i=0; i<NSTARS; i++){
    //     stars[i].updateEcc(eccentricity);
    // }
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
        midiMessages[msg.channel].push_back(msg);
        while(midiMessages[msg.channel].size() > maxMessages){
            midiMessages[msg.channel].erase(midiMessages[msg.channel].begin());
        }
    } else if(msg.status == MIDI_CONTROL_CHANGE){
        cout << "Chan " << msg.channel << ": CC " << msg.control << endl;
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
