#include "ofxMagneticBlobs.h"

//--------------------------------------------------------------
void ofxMagneticBlobs::update(int &frame)
{

    // Iterate through blobs
    int blobCount = BlobList.size();
    for (int i = 0; i < blobCount; i++)
    {

        // Blob's frame number is too old
        if (BlobList[i].frame < frame - 1)
        {

            // Destroy
            BlobList[i].frame = 0;
            BlobList[i].height = 0;
            BlobList[i].ID = 0;
            BlobList[i].peaks.clear();
            BlobList[i].points.clear();
            BlobList[i].width = 0;
            BlobList[i].x = 0;
            BlobList[i].y = 0;
            BlobList.erase(BlobList.begin() + i);

        }

    }

    // Iterate through peaks
    int peakCount = PeakList.size();
    for (int i = 0; i < peakCount; i++)
    {

        // Peak's frame number is too old
        if (PeakList[i].frame < frame - 5)
        {

            // Destroy
            PeakList[i].frame = 0;
            PeakList[i].ID = 0;
            PeakList[i].x = 0;
            PeakList[i].y = 0;
            PeakList.erase(PeakList.begin() + i);

        }

    }

}

//--------------------------------------------------------------
void ofxMagneticBlobs::set(ofxMagneticBlob blob)
{

    // Get blob count
    int blobCount = BlobList.size();

    // Check for blobs to compare
    if (blobCount > 0)
    {

        // Iterate through all blobs
        for (int i = 0; i < blobCount; i++)
        {

            // Blobs intersect
            if (intersect(&blob, &(ofxMagneticBlob)BlobList[i]))
            {

                // Get ID and peak list from old blob
                blob.ID = BlobList[i].ID;
                blob.peaks = BlobList[i].peaks;

                // Set previous position
                blob.pX = BlobList[i].x;
                blob.pY = BlobList[i].y;

                // Overwrite blob to update it
                BlobList[i] = blob;

                // Break operation
                return;

            }

        }

        // This is a new blob
        if (blob.ID <= 0) append(blob);

    }
    else
    {

        // Just add the blob
        append(blob);

    }

    // Clean up
    blobCount = 0;

}

//--------------------------------------------------------------
void ofxMagneticBlobs::append(ofxMagneticBlob blob)
{

    // Assign blob an ID
    ID++;
    blob.ID = ID;

    // Add blob
    BlobList.push_back(blob);

}

//--------------------------------------------------------------
bool ofxMagneticBlobs::intersect(ofxMagneticBlob * blobA, ofxMagneticBlob * blobB)
{

    // Return intersection flag
    return !(blobA->x - blobA->width * 0.5 > blobB->x + blobB->width * 0.5 ||
             blobA->x + blobA->width * 0.5 < blobB->x - blobB->width * 0.5 ||
             blobA->y - blobA->height * 0.5 > blobB->y + blobB->height * 0.5 ||
             blobA->y + blobA->height * 0.5 < blobB->y - blobB->height * 0.5);

}

//--------------------------------------------------------------
bool ofxMagneticBlobs::intersect(ofxMagneticPeak * peakA, ofxMagneticPeak * peakB)
{

    // Return intersection flag
    return !(peakA->x - peakRadius > peakB->x + peakRadius ||
             peakA->x + peakRadius < peakB->x - peakRadius ||
             peakA->y - peakRadius > peakB->y + peakRadius ||
             peakA->y + peakRadius < peakB->y - peakRadius);

}
