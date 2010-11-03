#ifndef _MAGNETIC_APP
#define _MAGNETIC_APP

#define SETTING_AMPLIFY                 0
#define SETTING_CALIBRATION_TIME        1
#define SETTING_CONTOUR_SMOOTHING       2
#define SETTING_DEVICE_ID               3
#define SETTING_DILATE                  4
#define SETTING_ERODE                   5
#define SETTING_FLIP_HORIZONTAL         6
#define SETTING_FLIP_VERTICAL           7
#define SETTING_FRAME_RATE              8
#define SETTING_HP_BLUR                 9
#define SETTING_HP_NOISE_REDUCTION      10
#define SETTING_INVERT                  11
#define SETTING_MAX_BLOBS               12
#define SETTING_MAX_BLOB_SIZE           13
#define SETTING_MIN_BLOB_SIZE           14
#define SETTING_PEAK_GAP_DISTANCE       15
#define SETTING_PEAK_RADIUS             16
#define SETTING_SIMULATED_BLOB_RADIUS   17
#define SETTING_SKELETONIZE             18
#define SETTING_SMOOTH                  19
#define SETTING_THRESHOLD               20

#define MAGNETIC_MODE_NORMAL            0
#define MAGNETIC_MODE_SETTINGS          1
#define MAGNETIC_MODE_CALIBRATION       2

#include "ofMain.h"
#include "ofxMagneticBlobs.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

class ofxMagneticApp : public ofBaseApp
{

	public:

		// Constructor
		ofxMagneticApp() {}

		// Destructor
		~ofxMagneticApp()
		{

			// Clear arrays
			delete [] __calibrationCameraPoints;
			delete [] __calibrationScreenPoints;
			delete [] __calibrationTriangles;
			delete [] __calibrationWarpMap;

		}

		//--------------------------------------------------------------
		virtual void setup()
		{

            // Version
            version = "0.2.6";

			// Load XML
			if (!__configXML.loadFile("config.xml")) exit();
			if (!__calibrationXML.LoadFile("data/calibration.xml", TIXML_ENCODING_UTF8)) exit();

			// Set width and height
			__height = (int)__configXML.getValue("application:height", 600);
			__width = (int)__configXML.getValue("application:width", 800);
			ofSetWindowShape(__width, __height);

			// Set background color and clearing
			ofSetBackgroundAuto(true);
			ofBackground(0, 0, 0);

			// Make things smoove
			ofSetVerticalSync(true);
			ofEnableSmoothing();
			ofEnableAlphaBlending();
			ofSetCircleResolution(36);

			// Load font
			__font.loadFont("micross.ttf", 30);
			__fontID.loadFont("micross.ttf", 15);
			__fontSub.loadFont("micross.ttf", 10);
			__fontTitle.loadFont("micross.ttf", 50);

			// Hide mouse
			ofHideCursor();

			// Set flags
			__bCommands = false;
			__bLearnBackground = true;
			__bCalibrate = false;
			__bSettings = false;

			// Set settings titles
			__settingsCount = 21;
			__settingsTitles = new string[__settingsCount];
			__settingsTitles[SETTING_AMPLIFY] = "AMPLIFY";
			__settingsTitles[SETTING_CALIBRATION_TIME] = "CALIBRATION TIME";
			__settingsTitles[SETTING_CONTOUR_SMOOTHING] = "CONTOUR SMOOTHING";
			__settingsTitles[SETTING_DEVICE_ID] = "DEVICE ID";
			__settingsTitles[SETTING_DILATE] = "DILATE";
			__settingsTitles[SETTING_ERODE] = "ERODE";
			__settingsTitles[SETTING_FLIP_HORIZONTAL] = "FLIP HORIZONTAL";
			__settingsTitles[SETTING_FLIP_VERTICAL] = "FLIP VERTICAL";
			__settingsTitles[SETTING_FRAME_RATE] = "FRAME RATE";
			__settingsTitles[SETTING_HP_BLUR] = "HIGH PASS BLUR";
			__settingsTitles[SETTING_HP_NOISE_REDUCTION] = "HIGH PASS NOISE REDUCTION";
			__settingsTitles[SETTING_INVERT] = "INVERT";
			__settingsTitles[SETTING_MAX_BLOBS] = "MAX BLOBS";
			__settingsTitles[SETTING_MAX_BLOB_SIZE] = "MAX BLOB SIZE";
			__settingsTitles[SETTING_MIN_BLOB_SIZE] = "MIN BLOB SIZE";
			__settingsTitles[SETTING_PEAK_GAP_DISTANCE] = "PEAK GAP DISTANCE";
			__settingsTitles[SETTING_PEAK_RADIUS] = "PEAK RADIUS";
			__settingsTitles[SETTING_SIMULATED_BLOB_RADIUS] = "SIMULATED BLOB RADIUS";
			__settingsTitles[SETTING_SKELETONIZE] = "SKELETONIZE";
			__settingsTitles[SETTING_SMOOTH] = "SMOOTH";
			__settingsTitles[SETTING_THRESHOLD] = "THRESHOLD";

			// Get settings from XML
			__amplify = (int)__configXML.getValue("config:amplify", 0);
			__blur = (int)__configXML.getValue("config:blur", 0);
			__calibrationTime = (int)__configXML.getValue("config:calibrationTime", 1000);
			__captureHeight = (int)__configXML.getValue("config:captureDevice:height", 240);
			__captureWidth = (int)__configXML.getValue("config:captureDevice:width", 320);
			__contourSmoothing = (int)__configXML.getValue("config:contourSmoothing", 0);
			__deviceID = (int)__configXML.getValue("config:captureDevice:id", 1);
			__dilate = (int)__configXML.getValue("config:dilate", 0);
			__erode = (int)__configXML.getValue("config:erode", 0);
			__flipHorizontal = (int)__configXML.getValue("config:flip:horizontal", 0) == 1;
			__flipVertical = (int)__configXML.getValue("config:flip:vertical", 0) == 1;
			__frameRate = (int)__configXML.getValue("config:frameRate", 30);
			__bFullscreen = (int)__configXML.getValue("config:fullscreen", 0) == 1;
			__invert = (int)__configXML.getValue("config:invert", 0) == 1;
			__maxBlobs = (int)__configXML.getValue("config:maxBlobs", 20);
			__maxBlobSize = (int)__configXML.getValue("config:maxBlobSize", (__captureWidth * __captureHeight) / 3);
			__minBlobSize = (int)__configXML.getValue("config:minBlobSize", 20);
			__noiseReduction = (int)__configXML.getValue("config:noiseReduction", 0);
			__peakGapDistance = (int)__configXML.getValue("config:peakGapDistance", 5);
			__peakRadius = (int)__configXML.getValue("config:peakRadius", 20);
			__simulatedBlobRadius = (int)__configXML.getValue("config:simulatedBlobRadius", 20);
			__skeletonize = (int)__configXML.getValue("config:skeletonize", 0) == 1;
			__smooth = (int)__configXML.getValue("config:smooth", 0);
			__threshold = (int)__configXML.getValue("config:threshold", 127);

			// Set peak radius in blobs
			__blobs.peakRadius = __peakRadius;

			// Set capture:display ratio
			__scaleX = __width / __captureWidth;
			__scaleY = __height / __captureHeight;

			// Get calibration settings from XML
			TiXmlHandle hDoc(&__calibrationXML);
			TiXmlElement* pCalibration = hDoc.FirstChild("calibration").ToElement();
			__calibrationColumns = atoi(pCalibration->Attribute("columns"));
			__calibrationRows = atoi(pCalibration->Attribute("rows"));
			__calibrationPoint = 0;
			__calibrationPointCount = __calibrationColumns * __calibrationRows;
			__calibrationPointSet = false;
			__calibrationTriangleCount = (__calibrationColumns - 1) * (__calibrationRows - 1) * 3 * 2;

			// Set up calibration and create warp map
			__setupCalibration();
			__warp();

			// Initialize camera
			__initCamera();

			// Toggle fullscreen
			if ((__bFullscreen && ofGetWindowMode() != OF_FULLSCREEN) || (!__bFullscreen && ofGetWindowMode() != OF_FULLSCREEN)) ofToggleFullscreen();

			// Set frame count
			__frame = 0;

			// Set setup flag
			__bSetup = true;

			// Report version
			cout << "\n\nMagnetic v" + version + "\n. . . . . . . .\n^^^^^^^^^^^^^^^\n\n";

		};

		//--------------------------------------------------------------
		virtual void update()
		{

            // Check setup flag
			if (__bSetup)
			{

				// Update blob list
				__blobs.update(__frame);

				// Calibrating
				if (__bCalibrate)
				{

					// Only calibrate if a single blob is present
					if (__blobs.BlobList.size() == 1)
					{

						// Waiting time is over!
						if (!__calibrationPointSet && __calibrationStartTime < ofGetElapsedTimeMillis() - __calibrationTime)
						{

							// Get the point in XML
							TiXmlHandle hDoc(&__calibrationXML);
							TiXmlElement* pPoint = hDoc.FirstChild("calibration").Child("point", __calibrationPoint).ToElement();

							// Set point position from blob
							pPoint->SetAttribute("x", ofToString((float)__blobs.BlobList[0].x / (float)__captureWidth));
							pPoint->SetAttribute("y", ofToString((float)__blobs.BlobList[0].y / (float)__captureHeight));

							// Set flags
							__calibrationPointSet = true;

						}

					}
					else
					{

						// Reset counter if no blob or too many blobs present
						__calibrationStartTime = ofGetElapsedTimeMillis();

						// No blobs and point has been set (this prevents
						// the last blob from calibrating the next point)
						if (__blobs.BlobList.size() == 0 && __calibrationPointSet)
						{

							// Learn background
							if (!__calibrationAutoMode) __bLearnBackground = true;

							// Set and save calibration
							__setupCalibration();
							__saveSettings();

							// Advance to next point and reset counter
							__calibrationPoint = (__calibrationPoint + 1) % __calibrationPointCount;
							__calibrationStartTime = ofGetElapsedTimeMillis();

							// Set flags
							__calibrationPointSet = false;

						}

					}

				}

				// Grab frame
				__vidGrabber.grabFrame();

				// Make sure it's a new frame
				if (__vidGrabber.isFrameNew())
				{

					// Get color image and flip it as needed
					__colorImage.setFromPixels(__vidGrabber.getPixels(), __captureWidth, __captureHeight);
					__colorImage.mirror(__flipVertical, __flipHorizontal);

					// Get greyscale image
					__greyImage = __colorImage;
					__tempImage = __greyImage;

					// Smoothing filter (gaussian blur)
					if (__smooth > 0) __greyImage.blur(__smooth);

					// High pass filter (thanks to CCV)
					if (__noiseReduction > 0 && __blur > 0)
                    {

                        // Apply blur
					    cvSmooth(__greyImage.getCvImage(), __tempImage.getCvImage(), CV_BLUR, (__blur * 2) + 1);
					    cvSub(__greyImage.getCvImage(), __tempImage.getCvImage(), __tempImage.getCvImage());

					    // Apply noise reduction
					    cvSmooth(__tempImage.getCvImage(), __tempImage.getCvImage(), CV_BLUR , (__noiseReduction * 2) + 1);

					    // Copy image back
					    __greyImage = __tempImage;

					}

					// Amplify image (multiply)
					if (__amplify > 0)
					{
					    cvMul(__greyImage.getCvImage(), __greyImage.getCvImage(), __tempImage.getCvImage(), (float)__amplify / 128.0f);
					    __greyImage = __tempImage;
					}

					// Learning background
					if (__bLearnBackground)
					{

						// Clear blob list, copy frame to background, and turn off flag
						__blobs.BlobList.clear();
						__greyBg = __greyImage;
						__bLearnBackground = false;

					}

					// Find difference between current frame and background
					__greyDiff.absDiff(__greyBg, __greyImage);
					__greyDiff.threshold(__threshold, __invert);

					// Erode binary image (thin)
					if (__erode > 0) for (int i = 0; i < __erode; i++) __greyDiff.erode_3x3();

					// Skeletonization is on
					if (__skeletonize)
					{

					    // TO DO: Skeletonize binary image

                    }

					// Dilate binary image (thicken)
					if (__dilate > 0) for (int i = 0; i < __dilate; i++) __greyDiff.dilate_3x3();

					// Find blobs in difference
					__contourFinder.findContours(__greyDiff, __minBlobSize * __minBlobSize, __maxBlobSize * __maxBlobSize, __maxBlobs, false);

				}

				// Iterate through blobs
				int blobCount = __contourFinder.nBlobs;
				for (int i = 0; i < blobCount; i++)
				{

					// Get the blob
					ofxCvBlob cfBlob = __contourFinder.blobs[i];

                    // Create a new blob
                    ofxMagneticBlob blob;
                    blob.frame = __frame;
                    blob.ID = -1;

                    // Set position
                    blob.x = cfBlob.centroid.x;
                    blob.y = cfBlob.centroid.y;

                    // Calculate warped size
                    ofPoint topLeft = __warpPoint(cfBlob.boundingRect.x, cfBlob.boundingRect.y);
                    ofPoint bottomRight = __warpPoint(cfBlob.boundingRect.x + cfBlob.boundingRect.width, cfBlob.boundingRect.y + cfBlob.boundingRect.height);

                    // Set height and width
                    blob.height = bottomRight.y - topLeft.y;
                    blob.width = bottomRight.x - topLeft.x;

                    // Copy point list
                    blob.points = __contourSmoothing > 0 ? __smoothContours(cfBlob.pts) : cfBlob.pts;

                    // Set blob in list
                    __blobs.set(blob);

				}

				// Mouse blob is active
				if (__mouseBlob.frame > 0)
				{

					// Update mouse blob
					__mouseBlob.frame++;
					//__blobs.set(__mouseBlob);

				}

				// Increment frame count
				__frame++;

			}

		}

		//--------------------------------------------------------------
		virtual void draw()
		{

			// Check setup flag
			if (__bSetup)
			{

				// Calibrating
				if (__bCalibrate)
				{

                    // Determine small circle size
                    float smallCircleSize = __height / 48.0000f;
                    if (smallCircleSize < 10) smallCircleSize = 10.0f;

                    // Automatic calibration mode
                    if (__calibrationAutoMode)
                    {

                        // Get position inside of an interval that is twice the calibration time
                        int millis = (ofGetElapsedTimeMillis() - __calibrationAutoStartTime) % (__calibrationTime * 3);

                        // Learn black background
                        if (millis < (float)__calibrationTime * 0.5) __bLearnBackground = true;

                        // Show light for a little longer than the calibration time
                        if (millis >= (float)__calibrationTime * 0.75 && millis < (float)__calibrationTime * 2.75)
                        {

                            // Get row/column from index
                            int i = (int)floor((float)__calibrationPoint / (float)__calibrationColumns);
                            int j = __calibrationPoint % __calibrationColumns;

                            // Find x, y
                            float x = (__width / (__calibrationColumns - 1)) * j;
                            float y = (__height / (__calibrationRows - 1)) * i;

                            // Draw circle
                            ofFill();
                            ofSetColor(255, 255, 255, 255);
                            ofCircle(x, y, smallCircleSize);

                        }

                    }
                    else
                    {

                        // Set fill mode
                        ofFill();

                        // Draw image
                        ofSetColor(255, 0, 255, 127);
                        __greyDiff.draw(0, 0, __width, __height);
                        ofSetColor(255, 255, 255, 127);
                        __colorImage.draw(0, 0, __width, __height);

                        // Iterate through triangles
                        ofNoFill();
                        ofSetColor(0, 255, 0, 64);
                        for (int i = 0; i < __calibrationTriangleCount; i += 6)
                        {

                            // Skip drawing last column so it won't connect to next row
                            if ((i / 6) % __calibrationColumns != __calibrationColumns)
                            {

                                // Draw shape
                                ofBeginShape();
                                for (int j = 0; j < 6; j++) ofVertex(__calibrationCameraPoints[__calibrationTriangles[i + j]].x * __scaleX, __calibrationCameraPoints[__calibrationTriangles[i + j]].y * __scaleY);
                                ofEndShape(OF_CLOSE);

                            }

                        }

                        // Iterate through rows
                        ofFill();
                        for (int i = 0; i < __calibrationRows; i++)
                        {

                            // Iterate through columns
                            for (int j = 0; j < __calibrationColumns; j++)
                            {

                                // Get position and index
                                int index = i * __calibrationColumns + j;
                                float x = (__width / (__calibrationColumns - 1)) * j;
                                float y = (__height / (__calibrationRows - 1)) * i;

                                // Draw a line from the grid to the point
                                ofNoFill();
                                ofSetColor(0, 255, 0, 127);
                                if (index == __calibrationPoint) ofLine(x, y, __calibrationCameraPoints[index].x * __scaleX, __calibrationCameraPoints[index].y * __scaleY);

                                // Draw calibration circle
                                float circleSize = index == __calibrationPoint ? (1.0000f - ((float)ofGetElapsedTimeMillis() - (float)__calibrationStartTime) / (float)__calibrationTime) * ((float)__height / 12.0000f) : smallCircleSize;
                                if (circleSize < 0) circleSize = 0.0f;
                                ofFill();
                                ofSetColor(255, 255, 255, 92);
                                ofCircle(x, y, circleSize);
                                ofNoFill();
                                ofSetColor(255, 255, 255, 192);
                                ofCircle(x, y, circleSize);

                                // This is the current point
                                if (index == __calibrationPoint)
                                {

                                    // Draw a cross
                                    ofNoFill();
                                    ofSetColor(255, 255, 255, 255);
                                    smallCircleSize *= 0.5f;
                                    ofLine(x - smallCircleSize, y, x + smallCircleSize, y);
                                    ofLine(x, y - smallCircleSize, x, y + smallCircleSize);

                                }

                            }

                        }

                        // Show instructions
                        ofFill();
                        ofSetColor(255, 255, 255, 255);
                        __font.drawString("CALIBRATION", 40, (float)__height * 0.5f);
                        __fontSub.drawString("Touch the highlighted circle until it disappears to calibrate a point\nUse arrows keys to warp points\nPress \"-\" to remove columns or \"+\" to add columns\nPress \"[\" to remove rows or \"]\" to add rows\nPress \"Enter\" to advance to the next point\nPress \"Backspace\" to back up to the previous point\nPress \"R\" to reset all points\nPress \"V\" to begin visible light automatic calibration\nPress \"C\" to exit", 40, (float)__height * 0.5f + 30);

                    }

				}

				// Changing settings
				if (__bSettings)
				{

					// Set fill mode
					ofFill();

					// Draw image
					ofSetColor(255, 0, 255, 255);
					__greyDiff.draw(0, 0, __width, __height);
					ofSetColor(255, 255, 255, 127);
					__greyImage.draw(0, 0, __width, __height);

					// Initialize strings
					string setting = "SET " + __settingsTitles[__settingsIndex] + ": ";
					string description = "";

					// Converts value to string
					switch (__settingsIndex)
					{

                        case SETTING_AMPLIFY:
                            setting += __amplify > 0 ? ofToString(__amplify) : "OFF";
                            description = "Multiplies the image to enhance bright areas";

							break;

                        case SETTING_CALIBRATION_TIME:
							setting += ofToString(__calibrationTime);
							description = "Time in milliseconds to hold a calibration point";

							break;

						case SETTING_CONTOUR_SMOOTHING:
							setting += __contourSmoothing > 0 ? ofToString(__contourSmoothing) : "OFF";
							description = "Simplifies blob outlines";

							break;

						case SETTING_DEVICE_ID:
							setting += ofToString(__deviceID);
							description = "Selects capture device";

							break;

						case SETTING_DILATE:
							setting += __dilate > 0 ? ofToString(__dilate) : "OFF";
							description = "Thickens blobs";

							break;

						case SETTING_ERODE:
							setting += __erode > 0 ? ofToString(__erode) : "OFF";
							description = "Thins blobs";

							break;

						case SETTING_FLIP_HORIZONTAL:
							setting += __flipHorizontal > 0 ? "ON" : "OFF";
							description = "Flips the input image horizontally";

							break;

						case SETTING_FLIP_VERTICAL:
							setting += __flipVertical > 0 ? "ON" : "OFF";
							description = "Flips the input image vertically";

							break;

						case SETTING_FRAME_RATE:
							setting += ofToString(__frameRate);
							description = "Frame rate of the capture device";

							break;

                        case SETTING_HP_BLUR:
                            setting += __blur > 0 ? ofToString(__blur) : "OFF";
                            description = "Blurs image before noise reduction in high pass filter\nRequires high pass noise reduction value of 1 or greater";

							break;

                        case SETTING_HP_NOISE_REDUCTION:
                            setting += __noiseReduction > 0 ? ofToString(__noiseReduction) : "OFF";
                            description = "Reduces static and noise in image after blur in high pass filter\nRequires high pass blur value of 1 or greater";

							break;

						case SETTING_INVERT:
                            setting += __invert > 0 ? "ON" : "OFF";
                            description = "Tracks dark blobs";

							break;

						case SETTING_MAX_BLOBS:
							setting += ofToString(__maxBlobs);
							description = "Limits number of blobs to track";

							break;

						case SETTING_MAX_BLOB_SIZE:
							setting += ofToString(__maxBlobSize);
							description = "Maximum size of blob to track";

							break;

						case SETTING_MIN_BLOB_SIZE:
							setting += ofToString(__minBlobSize);
							description = "Minimum size of blob to track";

							break;

						case SETTING_PEAK_GAP_DISTANCE:
							setting += ofToString(__peakGapDistance);
							description = "Minimum distance between points in outline to calculate a peak";

							break;

						case SETTING_PEAK_RADIUS:
							setting += ofToString(__peakRadius);
							description = "Artificial area created around a peak";

							break;

						case SETTING_SIMULATED_BLOB_RADIUS:
							setting += ofToString(__simulatedBlobRadius);
							description = "Artificial area created around a mouse when pressed";

							break;

                        case SETTING_SKELETONIZE:
							setting += __skeletonize > 0 ? "ON" : "OFF";
							description = "Converts blobs to skeletons and adds joints to peak list";

							break;

                        case SETTING_SMOOTH:
							setting += __smooth > 0 ? ofToString(__smooth) : "OFF";
							description = "Applies blur to image";

							break;

						case SETTING_THRESHOLD:
							setting += ofToString(__threshold);
							description = "Shades of grey higher than this value make up blobs";

							break;

					}

					// Write string
					ofSetColor(255, 255, 255, 255);
					__font.drawString(setting, 40, 50);
                    __fontID.drawString(description, 40, 75);
					__fontSub.drawString("Use left and right arrow keys to switch setting\nUse up and down arrow keys to change a setting's value\nPress \"S\" to exit", 40, 130);

				}

				// Calibrating or updating settings
				if ((__bCalibrate || __bSettings) && !__calibrationAutoMode)
				{

					// Iterate through blobs
					int blobCount = __blobs.BlobList.size();
					for (int i = 0; i < blobCount; i++)
					{

						// Get the blob position
						ofxMagneticBlob blob = __blobs.BlobList[i];
						float x = blob.x * __scaleX;
						float y = blob.y * __scaleY;

						// Draw ellipse around blob
						ofNoFill();
						ofSetColor(255, 255, 255, 127);
						ofEllipse(x, y, blob.width * __scaleX, blob.height * __scaleY);

						// Write ID
						ofFill();
						ofSetColor(255, 0, 255, 255);
						__fontID.drawString("`" + ofToString(blob.ID), x + 5, y + blob.height * __scaleY);

					}

				}

				// We're within the first five seconds or commands requested
				if (ofGetElapsedTimeMillis() < 5000 || __bCommands)
				{

					// Show instructions
					ofFill();
					ofSetColor(255, 255, 255, 255);
					__fontTitle.drawString("MAGNETIC", 40, __height - 170);
					__fontSub.drawString("v" + version, 362, __height - 150);
					__fontSub.drawString("\"B\" to clear background\n\"S\" for settings\n\"C\" to calibrate\n\"F11\" to toggle fullscreen mode\n\"F1\" to show these instructions\n\"Esc\" to exit\n\nFRAME RATE: " + ofToString((int)round(ofGetFrameRate())), 40, __height - 130);

				}

			}

		};

		//--------------------------------------------------------------
		virtual void keyPressed(int key)
		{

			// Check setup flag
			if (__bSetup)
			{

				// Updating settings
				if (__bSettings)
				{

					// Up/down keys
					if (key == OF_KEY_DOWN || key == OF_KEY_UP)
					{

						// Key on settings index
						switch (__settingsIndex)
						{

                            case SETTING_AMPLIFY:

								// Adjust blur
								if (key == OF_KEY_DOWN) __amplify--;
								if (key == OF_KEY_UP) __amplify++;
								if (__amplify < 0) __amplify = 0;
								if (__amplify > 300) __amplify = 300;

								break;

                            case SETTING_CALIBRATION_TIME:

								// Adjust blur
								if (key == OF_KEY_DOWN) __calibrationTime--;
								if (key == OF_KEY_UP) __calibrationTime++;
								if (__calibrationTime < 100) __calibrationTime = 100;
								if (__calibrationTime > 5000) __calibrationTime = 5000;

								break;

							case SETTING_CONTOUR_SMOOTHING:

								// Adjust contour smoothing
								if (key == OF_KEY_DOWN) __contourSmoothing--;
								if (key == OF_KEY_UP) __contourSmoothing++;
								if (__contourSmoothing < 0) __contourSmoothing = 0;
								if (__contourSmoothing > 100) __contourSmoothing = 100;

								break;

							case SETTING_DEVICE_ID:

								// Switch cameras
								int lastDeviceID = __deviceID;
								if (key == OF_KEY_DOWN) __deviceID--;
								if (key == OF_KEY_UP) __deviceID++;
								if (__deviceID >= __deviceCount - 1) __deviceID = __deviceCount - 1;;
								if (__deviceID < 0) __deviceID = 0;

								// Reset camera
								if (__deviceID != lastDeviceID) __initCamera();

								break;

							case SETTING_DILATE:

								// Clamp dilate
								if (key == OF_KEY_DOWN) __dilate--;
								if (key == OF_KEY_UP) __dilate++;
								if (__dilate < 0) __dilate = 0;
								if (__dilate > 100) __dilate = 100;

								break;

							case SETTING_ERODE:

								// Clamp erode
								if (key == OF_KEY_DOWN) __erode--;
								if (key == OF_KEY_UP) __erode++;
								if (__erode < 0) __erode = 0;
								if (__erode > 100) __erode = 100;

								break;

							case SETTING_FLIP_HORIZONTAL:

								// Toggle horizontal flip
								__flipHorizontal = !__flipHorizontal;

								break;

							case SETTING_FLIP_VERTICAL:

								// Toggle vertical flip
								__flipVertical = !__flipVertical;

								break;

							case SETTING_FRAME_RATE:

								// Adjust frame rate
								if (key == OF_KEY_DOWN) __frameRate--;
								if (key == OF_KEY_UP) __frameRate++;
								if (__frameRate < 5) __frameRate = 5;
								if (__frameRate > 120) __frameRate = 120;

								// Update frame rate
								__vidGrabber.setDesiredFrameRate(__frameRate);

								break;

                            case SETTING_HP_BLUR:

								// Adjust blur
								if (key == OF_KEY_DOWN) __blur--;
								if (key == OF_KEY_UP) __blur++;
								if (__blur < 0) __blur = 0;
								if (__blur > 100) __blur = 100;

								break;

                            case SETTING_HP_NOISE_REDUCTION:

								// Adjust blur
								if (key == OF_KEY_DOWN) __noiseReduction--;
								if (key == OF_KEY_UP) __noiseReduction++;
								if (__noiseReduction < 0) __noiseReduction = 0;
								if (__noiseReduction > 100) __noiseReduction = 100;

								break;

							case SETTING_INVERT:

								// Toggle invert
								__invert = !__invert;

								break;

							case SETTING_MAX_BLOBS:

								// Adjust maximum blob count
								if (key == OF_KEY_DOWN) __maxBlobs--;
								if (key == OF_KEY_UP) __maxBlobs++;
								if (__maxBlobs < 1) __maxBlobs = 1;
								if (__maxBlobs > 100) __maxBlobs = 100;

								break;

							case SETTING_MAX_BLOB_SIZE:

								// Adjust maximum blob size
								if (key == OF_KEY_DOWN) __maxBlobSize--;
								if (key == OF_KEY_UP) __maxBlobSize++;
								if (__maxBlobSize < __minBlobSize + 1) __maxBlobSize = __minBlobSize + 1;
								if (__maxBlobSize > 4000) __maxBlobSize = 4000;

								break;

							case SETTING_MIN_BLOB_SIZE:

								// Adjust minimum blob size
								if (key == OF_KEY_DOWN) __minBlobSize--;
								if (key == OF_KEY_UP) __minBlobSize++;
								if (__minBlobSize < 1) __minBlobSize = 1;
								if (__minBlobSize > __maxBlobSize - 1) __maxBlobSize = __maxBlobSize - 1;

								break;

							case SETTING_PEAK_GAP_DISTANCE:

								// Adjust simulated blob radius
								if (key == OF_KEY_DOWN) __peakGapDistance--;
								if (key == OF_KEY_UP) __peakGapDistance++;
								if (__peakGapDistance < 1) __peakGapDistance = 1;
								if (__peakGapDistance > 100) __peakGapDistance = 100;

								break;

							case SETTING_PEAK_RADIUS:

								// Adjust simulated blob radius
								if (key == OF_KEY_DOWN) __peakRadius--;
								if (key == OF_KEY_UP) __peakRadius++;
								if (__peakRadius < 1) __peakRadius = 1;
								if (__peakRadius > 100) __peakRadius = 100;

								break;

							case SETTING_SIMULATED_BLOB_RADIUS:

								// Adjust simulated blob radius
								if (key == OF_KEY_DOWN) __simulatedBlobRadius--;
								if (key == OF_KEY_UP) __simulatedBlobRadius++;
								if (__simulatedBlobRadius < __minBlobSize) __simulatedBlobRadius = __minBlobSize;
								if (__simulatedBlobRadius > 200) __simulatedBlobRadius = 200;

								break;

                            case SETTING_SKELETONIZE:

                                // Toggle skeletonize
                                __skeletonize = !__skeletonize;

                                break;

                            case SETTING_SMOOTH:

								// Adjust blur
								if (key == OF_KEY_DOWN) __smooth--;
								if (key == OF_KEY_UP) __smooth++;
								if (__smooth < 0) __smooth = 0;
								if (__smooth > 100) __smooth = 100;

								break;

							case SETTING_THRESHOLD:

								// Adjust threshold
								if (key == OF_KEY_DOWN) __threshold--;
								if (key == OF_KEY_UP) __threshold++;
								if (__threshold < 0) __threshold = 0;
								if (__threshold > 255) __threshold = 255;

								break;

						}

						// Save settings
						__saveSettings();

					}

				}

				// Calibrating
				if (__bCalibrate)
				{

					// Backspace
					if (key == OF_KEY_BACKSPACE)
					{

						// Decrease calibration point
						__calibrationPoint = (__calibrationPoint + __calibrationPointCount - 1) % __calibrationPointCount;

						// Reset counter for previous calibration point
						__calibrationStartTime = ofGetElapsedTimeMillis();

					}

					// Enter
					if (key == OF_KEY_RETURN)
					{

						// Increment calibration point
						__calibrationPoint = (__calibrationPoint + 1) % __calibrationPointCount;

						// Reset counter for next calibration point
						__calibrationStartTime = ofGetElapsedTimeMillis();

					}

					// Arrow keys
					if (key == OF_KEY_DOWN ||
					        key == OF_KEY_LEFT ||
					        key == OF_KEY_RIGHT ||
					        key == OF_KEY_UP)
					{

						// Init calibration point adjusters
						float x = 0.0000f;
						float y = 0.0000f;

						// Set value
						if (key == OF_KEY_DOWN) y++;
						if (key == OF_KEY_LEFT) x--;
						if (key == OF_KEY_RIGHT) x++;
						if (key == OF_KEY_UP) y--;

						// Value changed
						if (x != 0.0000f || y != 0.0000f)
						{

							// Get the point in XML
							TiXmlHandle hDoc(&__calibrationXML);
							TiXmlElement* pPoint = hDoc.FirstChild("calibration").Child("point", __calibrationPoint).ToElement();

							// Update point
							pPoint->SetAttribute("x", ofToString((atof(pPoint->Attribute("x")) * (float)__captureWidth + x) / (float)__captureWidth));
							pPoint->SetAttribute("y", ofToString((atof(pPoint->Attribute("y")) * (float)__captureHeight + y) / (float)__captureHeight));

							// Update calibration and warp map
							// Set up calibration and create warp map
							__setupCalibration();

						}

						// Learn background
						__bLearnBackground = true;

						// Save
						__saveSettings();

					}

				}

			}

		};

		//--------------------------------------------------------------
		virtual void keyReleased(int key)
		{

			// Check setup flag
			if (__bSetup)
			{

				// B to learn background
				if (key == 'b' || key == 'B') __bLearnBackground = true;

				// C for calibration
				if (key == 'c' || key == 'C') __calibrate();

				// S for settings
				if (key == 's' || key == 'S') __settings();

				// F1 brings up commands
				if (key == OF_KEY_F1) __bCommands = !__bCommands;

				// F11 to toggle fullscreen
				if (key == OF_KEY_F11)
				{

					// Toggle mode
					ofToggleFullscreen();

					// Switch flag and save settings
					__bFullscreen = !__bFullscreen;
					__saveSettings();

				}

				// ESC exits
				if (key == OF_KEY_ESC) exit();

				// Updating settings
				if (__bSettings)
				{

					// Toggle settings
					if (key == OF_KEY_LEFT) __settingsIndex = (__settingsIndex + __settingsCount - 1) % __settingsCount;
					if (key == OF_KEY_RIGHT) __settingsIndex = (__settingsIndex + 1) % __settingsCount;

				}

				// Calibrating
				if (__bCalibrate)
				{

					// "R" to reset
					if (key == 'r' || key == 'R')
					{

						// Reset and save
						__resetCalibration();
						__setupCalibration();
						__warp();
						__saveSettings();

					}

                    // "V" for visible light automatic calibration routine
                    if (key == 'v' || key == 'V')
                    {

                        // Set auto calibration params
                        __calibrationAutoMode = !__calibrationAutoMode;
                        __calibrationAutoStartTime = __calibrationAutoMode ? ofGetElapsedTimeMillis() : 0;

                    }

					// Note current row and column count
					int currentCalibrationColumns = __calibrationColumns;
					int currentCalibrationRows = __calibrationRows;

					// Use +/-, [/] to manipulate grid size
					if (key == '-' || key == '_') __calibrationColumns--;
					if (key == '=' || key == '+') __calibrationColumns++;
					if (key == '[' || key == '{') __calibrationRows--;
					if (key == ']' || key == '}') __calibrationRows++;

					// Limit layout
					if (__calibrationColumns < 2) __calibrationColumns = 2;
					if (__calibrationColumns > 12) __calibrationColumns = 12;
					if (__calibrationRows < 2) __calibrationRows = 2;
					if (__calibrationRows > 12) __calibrationRows = 12;

					// Count has changed
					if (__calibrationColumns != currentCalibrationColumns ||
					        __calibrationRows != currentCalibrationRows)
					{

						// Update counts
						__calibrationPointCount = __calibrationColumns * __calibrationRows;
						__calibrationTriangleCount = (__calibrationColumns - 1) * (__calibrationRows - 1) * 3 * 2;

						// Reset calibration
						__resetCalibration();
						__setupCalibration();
						__saveSettings();

					}

				}

			}

		};

		//--------------------------------------------------------------
		virtual void mouseDragged(int x, int y, int button)
		{

			// Check setup flag
			if (__bSetup) __updateMouseBlob(x, y);

		}

		//--------------------------------------------------------------
		virtual void mousePressed(int x, int y, int button)
		{

			// Check setup flag
			if (__bSetup)
			{

				// Create a blob at the mouse cursor
				__blobs.ID++;
				__mouseBlob.height = __simulatedBlobRadius / __scaleY;
				__mouseBlob.ID = __blobs.ID;
				__mouseBlob.width = __simulatedBlobRadius / __scaleX;

                // Update mouse blob
				__updateMouseBlob(x, y);

			}

		}

		//--------------------------------------------------------------
		virtual void mouseReleased(int x, int y, int button)
		{

			// Check setup flag
			if (__bSetup)
			{

				// Setting frame to 0 will remove blob
				__mouseBlob.frame = 0;
				__blobs.set(__mouseBlob);

			}

		}

		//--------------------------------------------------------------
		virtual void windowResized(int w, int h)
		{

			// Check setup flag
			if (__bSetup)
			{

				// Record screen size
				__width = w;
				__height = h;

				// Reset scale
				__scaleX = (float)__width / (float)__captureWidth;
				__scaleY = (float)__height / (float)__captureHeight;

				// Save settings
				__saveSettings();

			}

		};

		// Create list of blobs with scaled-to-screen dimensions
		vector<ofxMagneticBlob> getBlobs();
		vector<ofxMagneticBlob> getBlobs(bool bReturnPoints);
		vector<ofxMagneticBlob> getBlobs(bool bReturnPoints, bool bReturnPeaks);

        // Returns peak radius
        int getPeakRadius();

		// Returns color video
		ofxCvColorImage getVideo();

		// Get current mode
		int getMode();

	private:

		// NOTE: All private properties and methods have a "__" prefix
		//       to prevent conflicts with derived class.

		// Loaded assets
		ofxXmlSettings __configXML;
		ofTrueTypeFont __font;
		ofTrueTypeFont __fontID;
		ofTrueTypeFont __fontSub;
		ofTrueTypeFont __fontTitle;

		// Blob manager
		ofxMagneticBlobs __blobs;

		// Flags
		bool __bCalibrate;
		bool __bCommands;
		bool __bFullscreen;
		bool __bLearnBackground;
		bool __bSettings;
		bool __bSetup;

		// Open CV
		ofxCvColorImage __colorImage;
		ofxCvContourFinder __contourFinder;
		ofxCvGrayscaleImage __greyBg;
		ofxCvGrayscaleImage __greyDiff;
		ofxCvGrayscaleImage __greyImage;
		ofxCvGrayscaleImage __tempImage;
		ofVideoGrabber __vidGrabber;

		// Display
		int __height;
		int __width;
		float __scaleX;
		float __scaleY;

		// Settings
		int __amplify;
		int __blur;
		int __captureHeight;
		int __captureWidth;
		int __contourSmoothing;
		int __deviceCount;
		int __deviceID;
		int __dilate;
		int __erode;
		bool __flipHorizontal;
		bool __flipVertical;
		int __frame;
		int __frameRate;
		bool __invert;
		int __maxBlobs;
		int __maxBlobSize;
		int __minBlobSize;
		int __noiseReduction;
		int __peakGapDistance;
		int __peakRadius;
		int __settingsCount;
		int __settingsIndex;
		bool __skeletonize;
		int __smooth;
		string* __settingsTitles;
		int __threshold;

		// Contour smoothing
		vector<ofPoint> __smoothContours(vector<ofPoint> points);

		// Mouse simulates blob
		ofxMagneticBlob __mouseBlob;
		int __simulatedBlobRadius;

		// Calibration
		bool __calibrationAutoMode;
		int __calibrationAutoStartTime;
		ofPoint* __calibrationCameraPoints;
		int __calibrationColumns;
		bool __calibrationCoordsInTriangle(float x, float y, ofPoint pointA, ofPoint pointB, ofPoint pointC);
		bool __calibrationCoordsOnSameSide(float x, float y, ofPoint pointA, ofPoint pointB, ofPoint pointC);
		int __calibrationRows;
		int __calibrationPoint;
		int __calibrationPointCount;
		bool __calibrationPointSet;
		ofPoint* __calibrationScreenPoints;
		int __calibrationStartTime;
		int __calibrationTime;
		int* __calibrationTriangles;
		int __calibrationTriangleCount;
		ofPoint* __calibrationWarpMap;
		TiXmlDocument __calibrationXML;
		ofPoint __warpPoint(float x, float y);

		// Methods
		void __calibrate();
		void __initCamera();
		void __resetCalibration();
		void __saveSettings();
		void __settings();
		void __setupCalibration();
		void __updateMouseBlob(int x, int y);
		void __warp();

		// Version
		string version;

};

#endif
