#include "ofApp.h"
#include "math.h"
#include <iostream>

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(true);
    wglSwapIntervalEXT(1);

    cout << "Starting..." << endl;

    ofResetElapsedTimeCounter();
    //ofSetFrameRate(30);
    cout << "Setting up GUI... ";
    // setup GUI...
    // eccentricityTarget.addListener(this, &ofApp::eccentricityChange);
    gui.setup();
    gui.add(starNumTarget.set("number", 1000, 0, NSTARS));
    gui.add(trailsTarget.set("trails", 1, 1, TRAIL_LEN));
    gui.add(fadeTarget.set("fade", 255, 1, 255));
    gui.add(speedTarget.set("speed", 100, 0, 300));
    gui.add(eccentricityTarget.set("eccentricity", 0, 0, 0.5));
    gui.add(sizeTarget.set("size", 0, 0, 1));
    gui.add(constellationTime.set("constellation", 1, 0, 4));
    gui.add(beatBrightness.set("beat brightness", 0, 0, 100));
    // gui.add(color.set("color", 0, 0, 1));
    gui.add(orbits.set("orbits", false));
    // gui.add(blur.set("blur", false));

    guiPanel = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    guiPanel->addHeader("MIDI");
    guiPanel->addLabel("MIDI Note in device...");
    midiNoteDevice = guiPanel->addDropdown("Select...", midiNoteIn.getInPortList());
    guiPanel->addLabel("MIDI CC in device...");
    midiCCDevice = guiPanel->addDropdown("Select...", midiCCIn.getInPortList());

    guiPanel->setAutoDraw(false);

    midiNoteDevice->onDropdownEvent(this, &ofApp::onNoteInDropdownEvent);
    midiCCDevice->onDropdownEvent(this, &ofApp::onCCInDropdownEvent);

    starNum = starNumTarget;
    trails = trailsTarget;
    fade = fadeTarget;
    speed = speedTarget;
    size = sizeTarget;

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
    midiNoteIn.listInPorts();
    midiNoteIn.openPort(0);
    midiCCIn.openPort(0);
    // midiIn.openPort("LoopBe Internal MIDI 0");
    // midiIn.openPort("Komplete Audio 6 MIDI 1");
    // midiIn.openPort("MPKmini2 3");

    midiNoteIn.ignoreTypes(false, false, true);
    midiNoteIn.addListener(this);
    midiNoteIn.setVerbose(true);

    midiCCIn.ignoreTypes(false, false, true);
    midiCCIn.addListener(this);
    midiCCIn.setVerbose(true);

    cout << "DONE" << endl;

}

//--------------------------------------------------------------
void ofApp::update(){
    guiPanel->update();

    // Update parameters...
    if(starNum != starNumTarget){
        int dt = starNumTarget - starNum;
        if(dt > 0){
            starNum += 2;
        } else {
            starNum -= 2;
        }
        starNum = ofClamp(starNum, 0, NSTARS);
    }

    if(trails != trailsTarget){
        int dt = trailsTarget - trails;
        if(dt > 0){
            trails += 1;
        } else {
            trails -= 1;
        }
        trails = ofClamp(trails, 0, TRAIL_LEN);

    }


    if(fade != fadeTarget){
        int dt = fadeTarget - fade;
        if(dt > 0){
            fade += 2;
        } else {
            fade -= 2;
        }
        fade = ofClamp(fade, 1, 255);

    }


    if(speed != speedTarget){
        int dt = speedTarget - speed;
        if(dt > 0){
            speed += 1;
        } else {
            speed -= 1;
        }
        speed = ofClamp(speed, speedTarget.getMin(), speedTarget.getMax());

    }

    if(abs(eccentricity - eccentricityTarget) > 0.01){
        float dt = eccentricityTarget - eccentricity;
        eccentricity += dt/60;
        eccentricity = ofClamp(eccentricity, eccentricityTarget.getMin(), eccentricityTarget.getMax());
        eccentricityChange(eccentricity);
    }

    if(abs(sizeTarget - size) > 0.01){
        float dt = sizeTarget - size;
        size += dt/60;
        size = ofClamp(size, sizeTarget.getMin(), sizeTarget.getMax());
    }

    // Start drawing...
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
        ofFill();
        if(pulse){
          float delta = ofGetElapsedTimef() - pulse_start;
          float grey = ofMap(delta, 0.0, PULSE_TIME, beatBrightness, 0.0);
          ofSetColor(grey, grey, grey, fade);
          if(delta > PULSE_TIME){
            pulse = false;
          }
        } else {
          ofSetColor(0, 0, 0, fade);
        }
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
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();

    ofSetColor(255, 255, 255);
    fbo.draw(0, 0);

    if(!bHide){
        gui.draw();
        guiPanel->draw();
    }

    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

    // ofSetColor(255);
    // ofDrawLine(-20, 0, 20, 0);
    // ofDrawLine(0, -20, 0, 20);

}

//--------------------------------------------------------------
void ofApp::exit() {
    midiNoteIn.closePort();
    midiNoteIn.removeListener(this);
    midiCCIn.closePort();
    midiCCIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

    if(msg.portName == midiNoteDevice->getLabel()){
      this->noteMidiMessage(msg);
    } else if(msg.portName == midiCCDevice->getLabel()){
      this->ccMidiMessage(msg);
    }
}


void ofApp::noteMidiMessage(ofxMidiMessage& msg){

    if(clock.update(msg.bytes)){
        beats = clock.getBeats();
        int newQuarter = (beats / 4)%4;
        if(newQuarter != quarters){
          quarters = newQuarter;
          // cout << quarters << endl;
          pulse = true;
          pulse_start = ofGetElapsedTimef();
        }
    }

    switch (msg.status) {
      case MIDI_TIME_CLOCK:
        // cout << beats << endl;

      case MIDI_START: case MIDI_CONTINUE:
        if(!clockRunning){
          clockRunning = true;
          cout << "Clock set running" << endl;
        }

        break;
      case MIDI_STOP:
        if(clockRunning){
          clockRunning = false;
          cout << "Clock stopped" << endl;
        }

        break;

      case MIDI_NOTE_ON:

        midiMessages[msg.channel].push_back(msg);
        while(midiMessages[msg.channel].size() > maxMessages){
            midiMessages[msg.channel].erase(midiMessages[msg.channel].begin());
        }

        stars[msg.pitch + 50*msg.channel].startPulse();
        break;

      default:
        break;
    }

}

void ofApp::ccMidiMessage(ofxMidiMessage& msg){
    if(msg.status == MIDI_CONTROL_CHANGE){
        cout << "Chan " << msg.channel << ": CC " << msg.control;
        cout << " " << msg.value << endl;
        if(msg.channel == 1){
            int value = msg.value;
            switch(msg.control) {
                case 25:
                    starNumTarget.set(ofMap(value, 0, 127, starNumTarget.getMin(), starNumTarget.getMax()));
                    break;
                case 26:
                    trailsTarget.set(ofMap(value, 0, 127, trailsTarget.getMin(), trailsTarget.getMax()));
                    break;
                case 27:
                    fadeTarget.set(ofMap(value, 0, 127, fadeTarget.getMin(), fadeTarget.getMax()));
                    break;
                case 28:
                    speedTarget.set(ofMap(value, 0, 127, speedTarget.getMin(), speedTarget.getMax()));
                    break;
                case 29:
                    eccentricityTarget.set(ofMap(value, 0, 127, eccentricityTarget.getMin(), eccentricityTarget.getMax()));
                    sizeTarget.set(ofMap(value, 0, 127, sizeTarget.getMin(), sizeTarget.getMax()));
                    break;
                case 30:
                    constellationTime.set(ofMap(value, 0, 127, constellationTime.getMin(), constellationTime.getMax()));
                    break;
                case 31:
                    beatBrightness.set(ofMap(value, 0, 127, beatBrightness.getMin(), beatBrightness.getMax()));
                    break;

            }
        }
    }

}

//--------------------------------------------------------------
void ofApp::onNoteInDropdownEvent(ofxDatGuiDropdownEvent e){
    cout << "onNoteInDropdownEvent: " << e.target->getLabel() << " Selected" << endl;

    midiNoteIn.closePort();
    midiNoteIn.openPort(e.target->getLabel());
}

void ofApp::onCCInDropdownEvent(ofxDatGuiDropdownEvent e){
    cout << "onCCInDropdownEvent: " << e.target->getLabel() << " Selected" << endl;

    midiCCIn.closePort();
    midiCCIn.openPort(e.target->getLabel());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
      case ' ':
          bHide = !bHide;
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
