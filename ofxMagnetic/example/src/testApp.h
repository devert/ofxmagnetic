#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxMagneticApp.h"

class testApp : public ofxMagneticApp
{

public:

    // Routines
    void setup();
    void update();
    void draw();

    // Events
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void audioReceived(float * input, int bufferSize, int nChannels);
    void audioRequested(float * input, int bufferSize, int nChannels);

private:

};

#endif
