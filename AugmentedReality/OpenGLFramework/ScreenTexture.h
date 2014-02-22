////////////////////////////////////////////////////////////////////////////////
// Filename: ScreenTexture.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _ScreenTexture_H_
#define _ScreenTexture_H_

#include <stdio.h>
#include "openglclass.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class ScreenTexture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	ScreenTexture();
	ScreenTexture(const ScreenTexture&);
	~ScreenTexture();

	bool Initialize(OpenGLClass*, char*, unsigned int, bool);
	void Shutdown();

	void loadFrameIntoTexture(OpenGLClass* OpenGL, Mat &cameraFrame, unsigned int textureUnit);
	void createCameraTexture(OpenGLClass* OpenGL, unsigned int textureUnit);

private:
	bool LoadTarga(OpenGLClass*, char*, unsigned int, bool);

private:
	bool loaded;
	unsigned int m_textureID;
};

#endif