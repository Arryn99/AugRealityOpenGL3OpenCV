#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "openglclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "ScreenQuad.h"
#include "ScreenTextureShader.h"
#include "ObjectDetector.h"
#include "..\glm\glm.hpp"
#include "..\glm\gtc\type_ptr.hpp"
#include "..\glm\gtc\matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <opencv2\highgui\highgui.hpp>

using namespace cv;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(OpenGLClass*, HWND);
	void Shutdown();
	bool Frame(int frameCount);

private:
	bool Render(float rotation);

private:
	OpenGLClass* m_OpenGL;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	ScreenQuad* m_ScreenQuad;
	ScreenTextureShader* m_ScreenTextureShader;

	VideoCapture m_videoCapture;

	Mat cameraFrame;	//updated every 5 frames to hold the camera feed image
	ModelClass* m_ObjModel;

};

#endif