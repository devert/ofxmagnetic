#ifndef _MAGNETIC_BLOBS
#define _MAGNETIC_BLOBS

#include "ofxMagneticPeak.h"
#include "ofxMagneticBlob.h"
#include "ofTypes.h"
#include <vector>

// Blob manager
class ofxMagneticBlobs
{

public:

    // Next ID
    int ID;
    int peakID;

    // Radius of peak points
    int peakRadius;

    // Peaks and blobs
    vector<ofxMagneticBlob> BlobList;
    vector<ofxMagneticPeak> PeakList;

    // Functions
    void set(ofxMagneticBlob blob);
    void update(int &frame);
    bool intersect(ofxMagneticBlob* blobA, ofxMagneticBlob* blobB);
    bool intersect(ofxMagneticPeak* peakA, ofxMagneticPeak* peakB);

private:

    // Functions
    void append(ofxMagneticBlob blob);

};

#endif
