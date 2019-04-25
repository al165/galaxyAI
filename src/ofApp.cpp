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
    gui.add(starNum.set("number", 1000, 0, NSTARS));
    gui.add(trails.set("trails", 1, 1, TRAIL_LEN));
    gui.add(fade.set("fade", 255, 1, 255));
    gui.add(speed.set("speed", 100, 0, 300));
    gui.add(eccentricity.set("eccentricity", 0, 0, 0.5));
    gui.add(size.set("size", 0, 0, 1));
    gui.add(constellationTime.set("constellation", 0, 0, 4));
    // gui.add(color.set("color", 0, 0, 1));
    gui.add(orbits.set("orbits", false));
    // gui.add(blur.set("blur", false));

    cout << "DONE" << endl;

    bHide = false;

    ofBackground(0);
    ofSetCircleResolution(8);

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
    // midiIn.openPort("LoopBe Internal MIDI 0");
    midiIn.openPort("Komplete Audio 6 MIDI 1");

    midiIn.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    midiIn.setVerbose(true);

    cout << "DONE" << endl;

}

//--------------------------------------------------------------
void ofApp::update(){
    for(Celestial& star : stars){
        star.update(speed);
    }

    // if messages greater than 5, draw constelation!
    for(unsigned int j=0; j < CHANNELS; j++){
        if(midiMessages[j].size() > 5){
            vector<int> constellation;

            for(unsigned int i=0; i<midiMessages[j].size(); i++){
            		ofxMidiMessage &message = midiMessages[j][i];
                constellation.push_back(message.pitch + 50*j);

                stars[message.pitch + 50*j].startPulse();
                stars[message.pitch + 50*j + 250].startPulse();
                stars[message.pitch + 50*j + 500].startPulse();
            }

            Constellation newConst = Constellation(constellation);
            constellations.push_back(newConst);

            midiMessages[j].clear();

        }

    }

    // clear old constellations
    // cout << "constellations.size() = " << constellations.size();
    deque<int> deadConsts;
    for(unsigned int i=0; i<constellations.size(); i++){
        if(constellations[i].time + constellationTime < ofGetElapsedTimef()){
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
            if(constellationTime > 0.0){
                c.draw(stars, constellationTime);
            }
        }

        // for(Celestial& star : stars){
        //     star.draw(trails, orbits, blur, size, color);
        // }
        for(int i=0; i<starNum; i++){
            stars[i].draw(trails, orbits, size);
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

    // ofSetColor(255);
    // ofDrawLine(-20, 0, 20, 0);
    // ofDrawLine(0, -20, 0, 20);

}

//--------------------------------------------------------------
void ofApp::exit() {
    midiIn.closePort();
    midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {


    if(msg.status == MIDI_NOTE_ON){
        // cout << "NoteOn: " << msg.pitch << endl;
        midiMessages[msg.channel].push_back(msg);
        while(midiMessages[msg.channel].size() > maxMessages){
            midiMessages[msg.channel].erase(midiMessages[msg.channel].begin());
        }

        stars[msg.pitch + 50*msg.channel].startPulse();

    } else if(msg.status == MIDI_CONTROL_CHANGE){
        cout << "Chan " << msg.channel << ": CC " << msg.control;
        cout << " " << msg.value << endl;
        if(msg.channel == 10){
            int value = msg.value;
            switch(msg.control) {
                case 16:
                    starNum.set(ofMap(value, 0, 127, starNum.getMin(), starNum.getMax()));
                    break;
                case 20:
                    trails.set(ofMap(value, 0, 127, trails.getMin(), trails.getMax()));
                    break;
                case 24:
                    fade.set(ofMap(value, 0, 127, fade.getMin(), fade.getMax()));
                    break;
                case 17:
                    speed.set(ofMap(value, 0, 127, speed.getMin(), speed.getMax()));
                    break;
                case 21:
                    eccentricity.set(ofMap(value, 0, 127, eccentricity.getMin(), eccentricity.getMax()));
                    size.set(ofMap(value, 0, 127, size.getMin(), size.getMax()));
                    break;
                case 25:
                    constellationTime.set(ofMap(value, 0, 127, constellationTime.getMin(), constellationTime.getMax()));
                    break;

            }
        }
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

      case 'f':
          ofToggleFullscreen();
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
    fbo.allocate(w, h, GL_RGBA);

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
