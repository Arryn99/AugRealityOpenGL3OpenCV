////////////////////////////////////////////////////////////////////////////////
// Filename: ModelClass.h
// The ModelClass is responsible for encapsulating the geometry for 3D models.
////////////////////////////////////////////////////////////////////////////////

#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include "openglclass.h"

class ModelClass
{
private:
	//Here is the definition of our vertex type that will be used with the vertex buffer in this ModelClass.
	//This typedef must match the input variables layout in the ColourShaderClass as well as the GLSL vertex shader.

	struct VertexType
	{
		float x, y, z;
		float r, g, b;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();
	
	//The functions here handle initializing and shutdown of the model's vertex and index buffers.
	//The Render function puts the model geometry on the video card and draws it using the GLSL colour shader.

	bool Initialize(OpenGLClass*);
	void Shutdown(OpenGLClass*);
	void Render(OpenGLClass*);

private:
	bool InitializeBuffers(OpenGLClass*);
	void ShutdownBuffers(OpenGLClass*);
	void RenderBuffers(OpenGLClass*);

	//The private variables in the ModelClass are the vertex array object, vertex buffer, and index buffer IDs.
	//Also there are two integers to keep track of the size of the vertex and index buffers.

private:
	int m_vertexCount, m_indexCount;
	unsigned int m_vertexArrayId, m_vertexBufferId, m_indexBufferId;
};

#endif