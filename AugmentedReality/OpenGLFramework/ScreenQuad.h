////////////////////////////////////////////////////////////////////////////////
// Filename: ScreenQuad.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ScreenQuad_H_
#define _ScreenQuad_H_

#include "ScreenTexture.h"

class ScreenQuad
{
private:
	struct VertexType
	{
		float x, y, z;
		float tu, tv;
	};

public:
	ScreenQuad();
	ScreenQuad(const ScreenQuad&);
	~ScreenQuad();

	bool Initialize(OpenGLClass*, char*, unsigned int, bool);
	void Shutdown(OpenGLClass*);
	void Render(OpenGLClass*);

	void CopyFrame(OpenGLClass* OpenGL, Mat &cameraFrame);
	int InitialiseCamera(OpenGLClass* OpenGL);

protected:
	ScreenTexture* m_ScreenTexture;

private:
	bool InitializeBuffers(OpenGLClass*);
	void ShutdownBuffers(OpenGLClass*);
	void RenderBuffers(OpenGLClass*);

	bool LoadTexture(OpenGLClass*, char*, unsigned int, bool);
	void ReleaseTexture();

private:
	int m_vertexCount, m_indexCount;
	unsigned int m_vertexArrayId, m_vertexBufferId, m_indexBufferId;
	
};

#endif