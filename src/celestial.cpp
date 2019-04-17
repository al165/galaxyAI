#include "celestial.h"
#include "math.h"

Celestial::Celestial(float _ecc, float _a, float _gRotation){
    setup(_ecc, _a, _gRotation);
}

void Celestial::setup(float _ecc, float _a, float _gRotation){
    ecc = _ecc;
    a = _a;
    orbitAngle = 0;
    globalRotation = _gRotation;
    numerator = a*(1.0-ecc*ecc);

    r = numerator / (1.0 + ecc*cos(orbitAngle));
    pos = ofVec2f(r, 0);

    size = ofRandom(1, 4);
    //color.set(255, 255, 255, 255);
    color.set(255, ofRandom(200, 255), ofRandom(200, 255));
    blur.loadImage("star_blur_s.png");
    blur.setColor(color);

    updateBBox();
}

void Celestial::update(float dTheta){
    orbitAngle += dTheta/(r*r);

    // change the orbit angle as a function of the distance from the center
    r = numerator / (1.0 + ecc*cos(orbitAngle));

}

void Celestial::updateEcc(float _ecc){
    ecc = _ecc;
    numerator = a*(1.0-ecc*ecc);

    updateBBox();
}

void Celestial::updateBBox(){
    b = a*sqrt(1-ecc*ecc);
    center = -a*ecc;
}

void Celestial::draw(int trails, bool orbits, bool _blur, float scale, float _color){

    ofPushMatrix();
        ofRotateZRad(globalRotation);

        // draw orbit ellipse...
        if(orbits){
            ofSetColor(128);
            ofNoFill();
            ofDrawEllipse(center, 0, 2*a, 2*b);
        }


        float x = r * cos(orbitAngle);
        float y = r * sin(orbitAngle);

        GLfloat m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        ofMatrix4x4 mat(m);
        ofVec3f point(x, y, 0);
        ofVec3f newPos = point*mat;
        pos.x = newPos.x;
        pos.y = newPos.y;

        // draw blur...
        if(_blur){
            blur.draw(x-blur.getWidth()/2, y-blur.getHeight()/2);
        }

        // draw star...
        ofSetColor(color);
        ofFill();
        ofDrawCircle(x, y, 1 + scale*size);

        // draw trails...
        float x0 = x;
        float y0 = y;
        float diff = 0.01*min(trails, 5);

        for(int i=0; i<trails; i++){
            float tTheta = orbitAngle - i*diff;
            float t_r = numerator / (1.0 + ecc*cos(tTheta));

            float x1 = t_r * cos(tTheta);
            float y1 = t_r * sin(tTheta);

            ofSetColor(color, ofMap(i, 0, trails-1, 255, 0));
            ofDrawLine(x0, y0, x1, y1);

            x0 = x1;
            y0 = y1;
        }
    ofPopMatrix();



}
