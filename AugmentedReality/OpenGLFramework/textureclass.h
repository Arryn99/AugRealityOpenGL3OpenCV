////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <stdio.h>
#include "openglclass.h"

class TextureClass
{
private:
	//The image format used is called Targa and has a unique header that requires this structure
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

	//The first two functions will load a texture from a given file name and unload that texture when it is no longer needed.

	bool Initialize(OpenGLClass*, char*, unsigned int, bool);
	void Shutdown();

private:
	//The LoadTarga function loads a targa image into an OpenGL texture
	bool LoadTarga(OpenGLClass*, char*, unsigned int, bool);

private:
	//The loaded boolean indicates if a texture has been loaded into this class object or not.
	//The m_textureID is the ID number of the texture as OpenGL sees it.
	bool loaded;
	unsigned int m_textureID;
};

#endif