////////////////////////////////////////////////////////////////////////////////
// Filename: ScreenQuad.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ScreenQuad_H_
#define _ScreenQuad_H_

#include "Textureclass.h"

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

	bool Initialize(OpenGLClass*, Mat&, unsigned int, bool);
	void Shutdown(OpenGLClass*);
	void Render(OpenGLClass*);

	void updateTexture(OpenGLClass* OpenGL, Mat& textureFilename);

	int InitialiseCamera(OpenGLClass* OpenGL);

private:
	bool InitializeBuffers(OpenGLClass*);
	void ShutdownBuffers(OpenGLClass*);
	void RenderBuffers(OpenGLClass*);

	bool LoadTexture(OpenGLClass*, char*, unsigned int, bool);
	void ReleaseTexture();

private:
	int m_vertexCount, m_indexCount;
	unsigned int m_vertexArrayId, m_vertexBufferId, m_indexBufferId;
	TextureClass* m_ScreenTexture;
};

#endif