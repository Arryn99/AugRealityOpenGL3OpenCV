#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <stdio.h>
#include "openglclass.h"
#include <opencv2\core\core.hpp>

using namespace cv;

class TextureClass
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
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(OpenGLClass*, char*, unsigned int, bool);
	bool Initialize(OpenGLClass*, unsigned int, bool);
	bool loadMatIntoTexture(OpenGLClass*, Mat& mat);
	void Shutdown();

private:
	bool LoadTarga(OpenGLClass*, char*, unsigned int, bool);

private:
	bool loaded;
	unsigned int m_textureID;
};

#endif