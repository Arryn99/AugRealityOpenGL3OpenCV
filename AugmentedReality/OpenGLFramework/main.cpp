////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////

#include "SurfCameraFeed.h"
#include <windows.h>
#include "systemclass.h"

using namespace cv;
using namespace std;


int openCV();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//OpenCVCamera();	

	SystemClass* System;
	bool result;
	
	
	// Create the system object.
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}

int openCV(){
	/////////OPENCV
	Mat image;
	image = imread("Photo3.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

	if(! image.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	//namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
	imshow("E", image );                   // Show our image inside it.

	waitKey(0);                                          // Wait for a keystroke in the window
	return 0;
	/////////
}