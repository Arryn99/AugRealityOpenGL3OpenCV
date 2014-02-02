////////////////////////////////////////////////////////////////////////////////
// Filename: ColourShaderClass.h
// This class will be used to compile and execute our colour GLSL shaders
// so that we can render the 3D models that are on the GPU.
////////////////////////////////////////////////////////////////////////////////

#ifndef _COLOURSHADERCLASS_H_
#define _COLOURSHADERCLASS_H_


#include <fstream>
using namespace std;

#include "openglclass.h"


class ColourShaderClass
{
public:
	ColourShaderClass();
	ColourShaderClass(const ColourShaderClass&);
	~ColourShaderClass();

	//The functions here handle the initialisation and shutdown of the shader.
	//The SetShaderParameters function sets the shader uniform variables and
	//the SetShader function sets the shader code as the current rendering system.

	bool Initialize(OpenGLClass*, HWND);
	void Shutdown(OpenGLClass*);
	void SetShader(OpenGLClass*);
	bool SetShaderParameters(OpenGLClass*, float*, float*, float*);

private:
	bool InitializeShader(char*, char*, OpenGLClass*, HWND);
	char* LoadShaderSourceFile(char*);
	void OutputShaderErrorMessage(OpenGLClass*, HWND, unsigned int, char*);
	void OutputLinkerErrorMessage(OpenGLClass*, HWND, unsigned int);
	void ShutdownShader(OpenGLClass*);

private:
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;
	unsigned int m_shaderProgram;
};

#endif