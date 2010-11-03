#ifndef _MAGNETIC_PEAK
#define _MAGNETIC_PEAK

// A peak
typedef struct ofxMagneticPeak
{

    // Define peak structure
    int ID;
    int frame;
    bool inner;
    float x;
    float y;
    float pX;
    float pY;

};

#endif
