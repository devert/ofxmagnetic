#include "ofxMagneticApp.h"

//--------------------------------------------------------------
void ofxMagneticApp::__calibrate()
{

	// Check setup flag
	if (__bSetup)
	{

		// Set calibration corner and timer
		__bCalibrate = !__bCalibrate;
		__calibrationPoint = 0;
		__calibrationStartTime = ofGetElapsedTimeMillis();

		// Turn off settings
		if (__bCalibrate && __bSettings) __settings();

		// Finished calibrating
        if (!__bCalibrate)
        {

            // Update warp map and turn off auto flag
            __warp();
            __calibrationAutoMode = false;

        }

	}

}

//--------------------------------------------------------------
bool ofxMagneticApp::__calibrationCoordsInTriangle(float x, float y, ofPoint pointA, ofPoint pointB, ofPoint pointC)
{

    // Returns true if coordinates are triangulated
    return __calibrationCoordsOnSameSide(x, y, pointA, pointB, pointC) &&
           __calibrationCoordsOnSameSide(x, y, pointB, pointA, pointC) &&
           __calibrationCoordsOnSameSide(x, y, pointC, pointA, pointB);

}

//--------------------------------------------------------------
bool ofxMagneticApp::__calibrationCoordsOnSameSide(float x, float y, ofPoint pointA, ofPoint pointB, ofPoint pointC)
{

    // Subtract B from C
    ofPoint pointCB;
    pointCB.set(pointC.x - pointB.x, pointC.y - pointB.y);

    // Create the points we will use to find cross products
    ofPoint crossProductPointA;
    ofPoint crossProductPointB;
    crossProductPointA.set(x - pointB.x, y - pointB.y);
    crossProductPointB.set(pointA.x - pointB.x, pointA.y - pointB.y);

    // Fond cross products
    float crossProductA = pointCB.x * crossProductPointA.y - pointCB.y * crossProductPointA.x;
    float crossProductB = pointCB.x * crossProductPointB.y - pointCB.y * crossProductPointB.x;

    // Return true if cross products multiplied are positive
    return crossProductA * crossProductB >= 0.0f;


}

//--------------------------------------------------------------
vector<ofxMagneticBlob> ofxMagneticApp::getBlobs()
{

	// Get blob without point or peak data
	return getBlobs(false, false);

}

//--------------------------------------------------------------
vector<ofxMagneticBlob> ofxMagneticApp::getBlobs(bool bReturnPoints)
{

	// Get blob without points, but no peak data
	return getBlobs(bReturnPoints, false);

}

//--------------------------------------------------------------
vector<ofxMagneticBlob> ofxMagneticApp::getBlobs(bool bReturnPoints, bool bReturnPeaks)
{

	// Initialize return vector
	vector<ofxMagneticBlob> screenBlobs;

	// Iterate through blobs
	int blobCount = __blobs.BlobList.size();
	for (int i = 0; i < blobCount; i++)
	{

		// Create a new blob
		ofxMagneticBlob blob;

		// Copy and scale properties from blob in list
		blob.frame = __blobs.BlobList[i].frame;
		blob.ID = __blobs.BlobList[i].ID;

        // Warp and scale centroid
        ofPoint centroid = __warpPoint(__blobs.BlobList[i].x, __blobs.BlobList[i].y);
        ofPoint previous = __warpPoint(__blobs.BlobList[i].pX, __blobs.BlobList[i].pY);

        // Check bounds
        if (centroid.x > 0 && centroid.y > 0)
        {

            // Copy warped/scaled dimensions
            blob.height = __blobs.BlobList[i].height * __scaleY;
            blob.width = __blobs.BlobList[i].width * __scaleX;
            blob.x = centroid.x * __scaleX;
            blob.y = centroid.y * __scaleY;

            // Check previous position
            if (previous.x > 0 && previous.y > 0)
            {

                //Scale previous
                blob.pX = previous.x * __scaleX;
                blob.pY = previous.y * __scaleY;

            }

            // Return points
            if (bReturnPoints)
            {

                // List of contour distances
                vector<float> contourDistances;

                // Iterator
                int j;

                // Iterate through points
                int pointCount = __blobs.BlobList[i].points.size();
                for (j = 0; j < pointCount; j++)
                {

                    // Warp point
                    ofPoint warpedPoint = __warpPoint(__blobs.BlobList[i].points[j].x, __blobs.BlobList[i].points[j].y);

                    // Check bounds
                    if (warpedPoint.x > 0 && warpedPoint.y > 0)
                    {

                        // Scale point
                        warpedPoint.x *= __scaleX;
                        warpedPoint.y *= __scaleY;

                        // Add point to list
                        blob.points.push_back(warpedPoint);

                        // Peaks requested
                        if (bReturnPeaks)
                        {

                            // Record distance from centroid
                            float vx = (warpedPoint.x - blob.x) / __scaleX;
                            float vy = (warpedPoint.y - blob.y) / __scaleY;
                            contourDistances.push_back((float)sqrt(vx * vx + vy * vy));
                            vx = 0;
                            vy = 0;

                        }

                    }

                }

                // Peaks requested
                if (bReturnPeaks)
                {

                    // Create peak and valley flags
                    bool bPeak;
                    bool bValley;

                    // Iterate through distances
                    pointCount = blob.points.size();
                    for (j = 0; j < pointCount; j++)
                    {

                        // Set peak flag
                        bPeak = true;
                        bValley = false;

                        // Iterate through through range
                        for (int k = -__peakGapDistance; k <= __peakGapDistance; k++)
                        {

                            // Greater distance found
                            if (j != k && contourDistances[j] < contourDistances[(j + k + pointCount) % pointCount])
                            {

                                // Set flag to false and exit loop
                                bPeak = false;
                                k = __peakGapDistance;

                            }

                        }

                        // Skeletonization is on
                        if (__skeletonize)
                        {

                            // Set "valley" flag
                            bValley = true;

                            // Iterate through through range
                            for (int k = -__peakGapDistance; k <= __peakGapDistance; k++)
                            {

                                // Lesser distance found
                                if (j != k && contourDistances[j] > contourDistances[(j + k + pointCount) % pointCount])
                                {

                                    // Set flag to false and exit loop
                                    bValley = false;
                                    k = __peakGapDistance;

                                }

                            }

                        }

                        // This is a peak or valley that is not touching the screen's edge
                        if ((bPeak || bValley) &&
                            blob.points[j].x > 0 &&
                            blob.points[j].y > 0 &&
                            blob.points[j].x > (__contourSmoothing * 2) * __scaleX &&
                            blob.points[j].x < __width - (__contourSmoothing * 2) * __scaleX &&
                            blob.points[j].y > (__contourSmoothing * 2) * __scaleY &&
                            blob.points[j].y < __height - (__contourSmoothing * 2) * __scaleY)
                        {

                            // Create the peak
                            ofxMagneticPeak peak;
                            peak.ID = -1;
                            peak.frame = __frame;
                            peak.x = blob.points[j].x;
                            peak.y = blob.points[j].y;
                            peak.pX = peak.x;
                            peak.pY = peak.y;

                            // Iterate through existing peaks
                            int peakCount = __blobs.PeakList.size();
                            for (int k = 0; k < peakCount; k++)
                            {

                                // Peak overlaps existing peak
                                if (__blobs.intersect(&peak, &(ofxMagneticPeak)__blobs.PeakList[k]))
                                {

                                    // Update peak ID in blob list
                                    peak.ID = __blobs.PeakList[k].ID;

                                    // Update peak in list
                                    peak.pX = __blobs.PeakList[k].x;
                                    peak.pY = __blobs.PeakList[k].y;
                                    __blobs.PeakList[k] = peak;

                                }

                            }

                            // This is a new peak
                            if (peak.ID < 0)
                            {

                                // Increment ID and assign it
                                __blobs.peakID++;
                                peak.ID = __blobs.peakID;

                                // Copy peak to blob in original and master lists
                                __blobs.BlobList[i].peaks.push_back(peak);
                                __blobs.PeakList.push_back(peak);


                            }

                            // Add the peak
                            blob.peaks.push_back(peak);

                            // Clean up
                            peakCount = 0;

                        }

                        // Clean up
                        bPeak = 0;
                        bValley = 0;

                    }

                }

                // Clean up
                contourDistances.clear();
                j = 0;
                pointCount = 0;

            }

            // Add blob
            screenBlobs.push_back(blob);

        }

	}

	// Return collection
	return screenBlobs;

}

//--------------------------------------------------------------
int ofxMagneticApp::getMode()
{

    // Set mode
    int mode = MAGNETIC_MODE_NORMAL;

    // Switch mode as needed
    if (__bSettings) mode = MAGNETIC_MODE_SETTINGS;
    if (__bCalibrate) mode = MAGNETIC_MODE_CALIBRATION;

    // Return mode
    return mode;

}

//--------------------------------------------------------------
int ofxMagneticApp::getPeakRadius()
{

    // Return peak radius
    return __peakRadius;

}

//--------------------------------------------------------------
ofxCvColorImage ofxMagneticApp::getVideo()
{

	// Return color video
	return __colorImage;

}

//--------------------------------------------------------------
void ofxMagneticApp::__initCamera()
{

	// Grab video
	__vidGrabber.setDeviceID(__deviceID);
	__vidGrabber.setDesiredFrameRate(__frameRate);
	__vidGrabber.setVerbose(true);
	__vidGrabber.initGrabber(__captureWidth, __captureHeight);

	// Set up images
	__colorImage.allocate(__captureWidth, __captureHeight);
	__greyImage.allocate(__captureWidth, __captureHeight);
	__greyBg.allocate(__captureWidth, __captureHeight);
	__greyDiff.allocate(__captureWidth, __captureHeight);
	__tempImage.allocate(__captureWidth, __captureHeight);

	// Set device count
	__deviceCount = __vidGrabber.getDeviceCount();

}

//--------------------------------------------------------------
void ofxMagneticApp::__resetCalibration()
{

    // Get XML
    TiXmlHandle hDoc(&__calibrationXML);
    TiXmlElement* pCalibration = hDoc.FirstChild("calibration").ToElement();

    // Update grid parameters and clear points
    pCalibration->SetAttribute("columns", ofToString(__calibrationColumns));
    pCalibration->SetAttribute("rows", ofToString(__calibrationRows));
    pCalibration->Clear();

    // Iterate through rows
    for (int i = 0; i < __calibrationRows; i++)
    {

        // Iterate through columns
        for (int j = 0; j < __calibrationColumns; j++)
        {

            // Create point
            TiXmlElement* pPoint = new TiXmlElement("point");

            // Set value (0.0000 - 1.0000)
            pPoint->SetAttribute("x", ofToString(((float)(((float)__captureWidth / (float)(__calibrationColumns - 1)) * j)) / (float)__captureWidth));
            pPoint->SetAttribute("y", ofToString(((float)(((float)__captureHeight / (float)(__calibrationRows - 1)) * i)) / (float)__captureHeight));

            // Add point to <calibration> node
            pCalibration->LinkEndChild(pPoint);

        }

    }

    // Reset calibration settings
    __calibrationPoint = 0;
    __calibrationStartTime = ofGetElapsedTimeMillis();

    // Learn background (will reset warp map)
    __bLearnBackground = true;

}

//--------------------------------------------------------------
void ofxMagneticApp::__saveSettings()
{

	// Check setup flag
	if (__bSetup)
	{

		// Update settings
		__configXML.setValue("config:amplify", __amplify, 0);
		__configXML.setValue("config:blur", __blur, 0);
		__configXML.setValue("config:application:width", __width, 0);
		__configXML.setValue("config:application:height", __height, 0);
		__configXML.setValue("config:calibrationTime", __calibrationTime, 0);
		__configXML.setValue("config:captureDevice:id", __deviceID, 0);
		__configXML.setValue("config:captureDevice:width", __captureWidth, 0);
		__configXML.setValue("config:captureDevice:height", __captureHeight, 0);
		__configXML.setValue("config:contourSmoothing", __contourSmoothing, 0);
		__configXML.setValue("config:dilate", __dilate, 0);
		__configXML.setValue("config:erode", __erode, 0);
		__configXML.setValue("config:flip:horizontal", __flipHorizontal ? 1 : 0, 0);
		__configXML.setValue("config:flip:vertical", __flipVertical ? 1 : 0, 0);
		__configXML.setValue("config:frameRate", __frameRate, 0);
		__configXML.setValue("config:fullscreen", __bFullscreen ? 1 : 0, 0);
		__configXML.setValue("config:invert", __invert ? 1 : 0, 0);
		__configXML.setValue("config:maxBlobs", __maxBlobs, 0);
		__configXML.setValue("config:maxBlobSize", __maxBlobSize, 0);
		__configXML.setValue("config:minBlobSize", __minBlobSize, 0);
		__configXML.setValue("config:noiseReduction", __noiseReduction, 0);
		__configXML.setValue("config:peakGapDistance", __peakGapDistance, 0);
		__configXML.setValue("config:peakRadius", __peakRadius, 0);
		__configXML.setValue("config:simulatedBlobRadius", __simulatedBlobRadius, 0);
		__configXML.setValue("config:skeletonize", __skeletonize ? 1 : 0, 0);
		__configXML.setValue("config:smooth", __smooth, 0);
		__configXML.setValue("config:threshold", __threshold, 0);

		// Save settings and calibration
		__configXML.saveFile("config.xml");
		__calibrationXML.SaveFile("data/calibration.xml");

	}

}

//--------------------------------------------------------------
void ofxMagneticApp::__settings()
{

	// Check setup flag
	if (__bSetup)
	{

		// Get latest device count when entering
		// settings in case new camera was added
		if (!__bSettings) __deviceCount = __vidGrabber.getDeviceCount();

		// Toggle settings flag
		__bSettings = !__bSettings;
		__settingsIndex = 0;

		// Turn off calibration
		if (__bSettings && __bCalibrate) __calibrate();

	}

}

//--------------------------------------------------------------
void ofxMagneticApp::__setupCalibration()
{

    // Get calibration points from XML
    TiXmlHandle hDoc(&__calibrationXML);
    TiXmlHandle hCalibration = hDoc.FirstChild("calibration");

    // Initialize arrays
    __calibrationCameraPoints = new ofPoint[__calibrationPointCount];
    __calibrationScreenPoints = new ofPoint[__calibrationPointCount];
    __calibrationTriangles = new int[__calibrationTriangleCount];

    // Find distance between lines in grid
    float deltaX = (float)__captureWidth / (float)(__calibrationColumns - 1);
	float deltaY = (float)__captureHeight / (float)(__calibrationRows - 1);

    // Iterate through rows
    int t = 0;
    for (int i = 0; i < __calibrationRows; i++)
    {

        // Iterate through columns
        for (int j = 0; j < __calibrationColumns; j++)
        {

            // Omit last point in triangle mesh
            if (i < __calibrationRows - 1 && j < __calibrationColumns - 1)
            {

                // Create a downward pointing right triangle on left side of rectangle
                __calibrationTriangles[t + 0] = (j + 0) + (i + 0) * __calibrationColumns;
                __calibrationTriangles[t + 1] = (j + 1) + (i + 0) * __calibrationColumns;
                __calibrationTriangles[t + 2] = (j + 0) + (i + 1) * __calibrationColumns;
                t += 3;

                // And an upward pointing right triangle on right side of the rectangle
                __calibrationTriangles[t + 0] = (j + 1) + (i + 0) * __calibrationColumns;
                __calibrationTriangles[t + 1] = (j + 1) + (i + 1) * __calibrationColumns;
                __calibrationTriangles[t + 2] = (j + 0) + (i + 1) * __calibrationColumns;
                t += 3;

            }

            // Calculate point index
            int index = i * __calibrationColumns + j;

            // Get calibration point from XML
            TiXmlElement* pPoint = hCalibration.Child("point", index).ToElement();

            // Set grid point on screen and camera
            __calibrationScreenPoints[index].set(deltaX * (float)j, deltaY * (float)i);
            __calibrationCameraPoints[index].set(atof(pPoint->Attribute("x")) * (float)__captureWidth, atof(pPoint->Attribute("y")) * (float)__captureHeight);

        }

    }

}

//--------------------------------------------------------------
vector<ofPoint> ofxMagneticApp::__smoothContours(vector<ofPoint> points)
{

	// Create return vector
	vector<ofPoint> smoothedContours;

	// Previous index...
	int lastIndex;

	// Iterate through points
	int pointCount = points.size();
	for (int i = 0; i < pointCount; i++)
	{

		// Past first point
		if (i > 0)
		{

			// Calculate distance
			float vx = points[i].x - points[lastIndex].x;
			float vy = points[i].y - points[lastIndex].y;
			float distance = (float)sqrt(vx * vx + vy * vy);

			// Minimum distance from last point exceeded
			if (distance > (float)__contourSmoothing)
			{

				// Add point to list
				lastIndex = i;
				smoothedContours.push_back(points[i]);

			}

		}
		else
		{

			// Add point to list
			lastIndex = i;
			smoothedContours.push_back(points[i]);

		}

	}

	// Return the list
	return smoothedContours;

}

//--------------------------------------------------------------
void ofxMagneticApp::__updateMouseBlob(int x, int y)
{

    // Check setup flag
    if (__bSetup)
    {

        // Blob outline dimensions
        float height = __mouseBlob.width * 0.5f;
        float width = __mouseBlob.height * 0.5f;

        // Update mouse blob frame and position
        __mouseBlob.frame = __frame;
        __mouseBlob.x = x / __scaleX;
        __mouseBlob.y = y / __scaleY;

        // Clear mouse blob points
        __mouseBlob.points.clear();

        // Iterate through circle
        double angleStep = TWO_PI / CIRC_RESOLUTION;
        for (double theta = angleStep; theta <= TWO_PI; theta += angleStep)
        {

            // Find point in circle
            ofPoint point;
            point.x = __mouseBlob.x - width * (float)cos(theta);
            point.y = __mouseBlob.y - height * (float)sin(theta);

            // Add point
            __mouseBlob.points.push_back(point);

        }

        // Set mouse blob
        //__blobs.set(__mouseBlob);

    }

}

//--------------------------------------------------------------
void ofxMagneticApp::__warp()
{

    // Initialize warp map
    __calibrationWarpMap = new ofPoint[__captureWidth * __captureHeight];

    // Iterate through camera height
    for (int i = 0; i < __captureHeight; i++)
    {

        // Iterate through camera width
        for (int j = 0; j < __captureWidth; j++)
        {

            // Initialize new x, y from iterators
            float x = (float)j;
            float y = (float)i;

            // Iterate through triangles
            int triangleIndex = -1;
            for (int k = 0; k < __calibrationTriangleCount; k += 3)
            {

                // Check if point is inside this triangle
                if (__calibrationCoordsInTriangle(x, y, __calibrationCameraPoints[__calibrationTriangles[k]], __calibrationCameraPoints[__calibrationTriangles[k + 1]], __calibrationCameraPoints[__calibrationTriangles[k + 2]]))
                {

                    // Mark triangle and break operation
                    triangleIndex = k;
                    break;

                }

            }

            // Point found within triangle
            if (triangleIndex >= 0)
            {

                // Get points of triangle from camera and screen
                ofPoint cameraPointA = __calibrationCameraPoints[__calibrationTriangles[triangleIndex]];
                ofPoint cameraPointB = __calibrationCameraPoints[__calibrationTriangles[triangleIndex + 1]];
                ofPoint cameraPointC = __calibrationCameraPoints[__calibrationTriangles[triangleIndex + 2]];
                ofPoint screenPointA = __calibrationScreenPoints[__calibrationTriangles[triangleIndex]];
                ofPoint screenPointB = __calibrationScreenPoints[__calibrationTriangles[triangleIndex + 1]];
                ofPoint screenPointC = __calibrationScreenPoints[__calibrationTriangles[triangleIndex + 2]];

                // Calculate total area of triangle
                float area = (cameraPointA.x - cameraPointB.x) * (cameraPointA.y - cameraPointC.y) - (cameraPointA.y - cameraPointB.y) * (cameraPointA.x - cameraPointC.x);

                // Calculate the area between the camera point and points B and C
                float areaA = (x - cameraPointB.x) * (y - cameraPointC.y) - (y - cameraPointB.y) * (x - cameraPointC.x);

                // Calculate the area between the point A and the camera point and point C
                float areaB = (cameraPointA.x - x) * (cameraPointA.y - cameraPointC.y) - (cameraPointA.y - y) * (cameraPointA.x - cameraPointC.x);

                // Calculate pressure from each sector
                float baryA = areaA / area;
                float baryB = areaB / area;
                float baryC = 1.0f - baryA - baryB;

                // Calculate new position
                x = (screenPointA.x * baryA) + (screenPointB.x * baryB) + (screenPointC.x * baryC);
                y = (screenPointA.y * baryA) + (screenPointB.y * baryB) + (screenPointC.y * baryC);

            }
            else
            {

                // Set x, y to negative if not found
                x = -1;
                y = -1;

            }

            // Set warped point
            __calibrationWarpMap[i * __captureWidth + j].set(x, y);

        }

    }

}

//--------------------------------------------------------------
ofPoint ofxMagneticApp::__warpPoint(float x, float y)
{

    // Find point in warp map
    int index = (int)y * __captureWidth + (int)x;

    // Get remapped coordinates
    ofPoint warpedPoint;
    warpedPoint.set(__calibrationWarpMap[index].x, __calibrationWarpMap[index].y);

    // Return warped point
    return warpedPoint;

}
