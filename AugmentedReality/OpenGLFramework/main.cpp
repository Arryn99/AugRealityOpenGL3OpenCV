////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////


#include "SurfCameraFeed.h"
#include <windows.h>
#include "systemclass.h"

using namespace cv;
using namespace std;

#define S_WIDTH	800		//client area resolution
#define S_HEIGHT 600

typedef struct Mouse
{
	int x,y;
}Mouse;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
void DrawScene();

bool gameRunning = true;
bool keys[256];

Mouse MousePos;

int  openCV();

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)											
	{														
	case WM_CREATE:	
		break;

	case WM_SIZE:
		//resize the open gl window when the window is resized
		//ResizeGLWindow(LOWORD(lParam),HIWORD(lParam));
		//GetClientRect(hwnd, &gRect);
		break;	

	case WM_KEYDOWN:
		keys[wParam]=true;
		break;

	case WM_KEYUP:
		keys[wParam]=false;
		break;

	case WM_MOUSEMOVE:
		MousePos.x = LOWORD (lParam);
		MousePos.y = HIWORD (lParam);
		break;

	case WM_PAINT:
		break;		

	case WM_DESTROY:	
		PostQuitMessage(0);				
		break;		
	}													

	return DefWindowProc (hwnd, message, wParam, lParam);																
}



int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow) {

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
	image = imread("logo.png", CV_LOAD_IMAGE_COLOR);   // Read the file

	if(! image.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
	imshow( "Display window", image );                   // Show our image inside it.

	waitKey(0);                                          // Wait for a keystroke in the window
	return 0;
	/////////
}