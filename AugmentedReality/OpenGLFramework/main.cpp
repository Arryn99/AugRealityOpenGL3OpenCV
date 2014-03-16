////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////

#include "SurfCameraFeed.h"
#include <windows.h>
#include "systemclass.h"

using namespace cv;
using namespace std;

SurfCameraFeed surfCameraInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	surfCameraInstance.Init("Photo2.jpg");

	while (true) {
		surfCameraInstance.Frame();
	}

	SystemClass* System;
	bool result;
	//int counter = 0;
	
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