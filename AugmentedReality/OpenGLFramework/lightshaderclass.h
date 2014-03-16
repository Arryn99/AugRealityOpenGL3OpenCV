#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

#include <fstream>
#include "openglclass.h"

using namespace std;

class LightShaderClass
{
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(OpenGLClass*, HWND);
	void Shutdown(OpenGLClass*);
	void SetShader(OpenGLClass*);
	bool SetShaderParameters(OpenGLClass*, float*, float*, float*, int, float*, float*);

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