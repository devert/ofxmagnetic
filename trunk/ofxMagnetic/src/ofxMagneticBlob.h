#ifndef _MAGNETIC_BLOB
#define _MAGNETIC_BLOB

#include "ofTypes.h"
#include <vector>

// A blob
class ofxMagneticBlob
{

public:

    // Destructor
    ~ofxMagneticBlob()
    {

        // Clean up
        for (int i = 0; i < points.size(); i++) points[i].~ofPoint();
        peaks.clear();
        points.clear();

    }

    // Define blob structure
    int frame;
    float height;
    int ID;
    vector<ofxMagneticPeak> peaks;
    vector<ofPoint> points;
    float pX;
    float pY;
    float width;
    float x;
    float y;

};

#endif
