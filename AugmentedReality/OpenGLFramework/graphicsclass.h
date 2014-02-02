////////////////////////////////////////////////////////////////////////////////
// Filename: GraphicsClass.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "openglclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColourShaderClass.h"

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
	bool Frame();

private:
	bool Render();

private:
	OpenGLClass* m_OpenGL;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColourShaderClass* m_ColourShader;
};

#endif