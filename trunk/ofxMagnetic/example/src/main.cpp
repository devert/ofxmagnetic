#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main()
{

    // Instantiate app
    testApp * app = new testApp();

    // Set up OpenGL window
    ofAppGlutWindow window;
    window.setGlutDisplayString("rgb alpha double samples depth");
    ofSetupOpenGL(&window, 800, 600, OF_FULLSCREEN);

    // Run application
    ofRunApp(app);

}
