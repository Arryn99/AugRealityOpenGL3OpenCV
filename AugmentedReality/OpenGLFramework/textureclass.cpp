////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "textureclass.h"

//The class constructor will initialize the loaded boolean to false so that we know there has not been a texture loaded yet.
TextureClass::TextureClass()
{
	loaded = false;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

//Initialize takes in the OpenGL pointer, the file name of the texture, the texture unit to load the texture into
//and a boolean value indicating if the texture should wrap or clamp the colors at the edges. It then loads the targa
//file into the OpenGL texture unit specified by calling the LoadTarga function. The texture can now be used to render with.
bool TextureClass::Initialize(OpenGLClass* OpenGL, char* filename, unsigned int textureUnit, bool wrap)
{
	bool result;


	// Load the targa file.
	result = LoadTarga(OpenGL, filename, textureUnit, wrap);
	if(!result)
	{
		return false;
	}

	return true;
}

//The Shutdown function releases the texture resource if it has been loaded.
void TextureClass::Shutdown()
{
	// If the texture was loaded then make sure to release it on shutdown.
	if(loaded)
	{
		glDeleteTextures(1, &m_textureID);
		loaded = false;
	}

	return;
}

//LoadTarga loads a .tga image onto an OpenGL texture. It also sets up texture filtering, texture wrapping, and mipmaps for the texture.
bool TextureClass::LoadTarga(OpenGLClass* OpenGL, char* filename, unsigned int textureUnit, bool wrap)
{
	int error, width, height, bpp, imageSize;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	//The beginning section loads the .tga file into a buffer called targaImage.

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	width = (int)targaFileHeader.width;
	height = (int)targaFileHeader.height;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if(bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if(!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = fread(targaImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}
	
	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	//Now that the buffer contains the .tga data we create an OpenGL texture object and copy the buffer into that texture object.
	//Note that .tga have the RGB reversed so in glTextImage2D we need to set the input format as GL_BGRA so it will reverse the
	//red and blue component for us when loading it in.

	// Set the unique texture unit in which to store the data.
	OpenGL->glActiveTexture(GL_TEXTURE0 + textureUnit);

	// Generate an ID for the texture.
	glGenTextures(1, &m_textureID);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// Load the image data into the texture unit.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, targaImage);

	// Set the texture color to either wrap around or clamp to the edge.
	if(wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// Set the texture filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Generate mipmaps for the texture.
	OpenGL->glGenerateMipmap(GL_TEXTURE_2D);

	// Release the targa image data.
	delete [] targaImage;
	targaImage = 0;

	// Set that the texture is loaded.
	loaded = true;

	return true;
}