#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{

    // Set up Magnetic
    ofxMagneticApp::setup();

    // TO DO: Put your setup code here

}

//--------------------------------------------------------------
void testApp::update()
{

    // Update Magnetic
    ofxMagneticApp::update();

    // TO DO: Put your update code here

}

//--------------------------------------------------------------
void testApp::draw()
{

    // Draw video
    ofSetColor(255, 255, 255, 255);
    ofxMagneticApp::getVideo().draw(0, 0, ofGetWidth(), ofGetHeight());

    // Iterate through blobs
    vector<ofxMagneticBlob> blobs = getBlobs(true, true);
    int blobCount = blobs.size();
    for (int i = 0; i < blobCount; i++)
    {

        // Get the blob
        ofxMagneticBlob blob = blobs[i];

        // Draw ellipse representing blob size and position
        ofFill();
        ofSetColor(255, 255, 255, 64);
        ofEllipse(blob.x, blob.y, blob.width, blob.height);

        // Begin outline
        ofNoFill();
        ofSetColor(0, 255, 0, 255);
        ofBeginShape();

        // Draw blob outline
        int pointCount = blob.points.size();
        for (int j = 0; j < pointCount; j++) ofVertex(blob.points[j].x, blob.points[j].y);

        // Close shape
        ofEndShape(OF_CLOSE);

        // Draw peaks
        ofFill();
        ofSetColor(0, 0, 255, 127);
        int peakCount = blob.peaks.size();
        for (int j = 0; j < peakCount; j++) ofCircle(blob.peaks[j].x, blob.peaks[j].y, ofGetHeight() / 40);

    }

    // Draw Magnetic
    ofxMagneticApp::draw();

}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels)
{

    // TO DO: Put your audioReceived code here


}

//--------------------------------------------------------------
void testApp::audioRequested(float * input, int bufferSize, int nChannels)
{

    // TO DO: Put your audioRequested code here

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

    // Pass to Magnetic
    ofxMagneticApp::keyPressed(key);

    // TO DO: Put your keyPressed code here

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

    // Pass to Magnetic
    ofxMagneticApp::keyReleased(key);

    // TO DO: Put your keyReleased code here

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

    // TO DO: Put your mouseMoved code here

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

    // Pass to Magnetic
    ofxMagneticApp::mouseDragged(x, y, button);

    // TO DO: Put your mouseDragged code here

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

    // Pass to Magnetic
    ofxMagneticApp::mousePressed(x, y, button);

    // TO DO: Put your mousePressed code here

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

    // Pass to Magnetic
    ofxMagneticApp::mouseReleased(x, y, button);

    // TO DO: Put your mouseReleased code here

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

    // Pass to Magnetic
    ofxMagneticApp::windowResized(w, h);

    // TO DO: Put your windowResized code here

}
