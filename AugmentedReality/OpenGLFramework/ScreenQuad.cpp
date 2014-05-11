////////////////////////////////////////////////////////////////////////////////
// Filename: ScreenQuad.cpp
//
// This class sets a quad to be the size of the window
// It will be set to the rear of the program and used to draw the OpenCV camera feed behind the model
////////////////////////////////////////////////////////////////////////////////

#include "ScreenQuad.h"


ScreenQuad::ScreenQuad()
{
	m_ScreenTexture = 0;
}


ScreenQuad::ScreenQuad(const ScreenQuad& other)
{
}


ScreenQuad::~ScreenQuad()
{
}


bool ScreenQuad::Initialize(OpenGLClass* OpenGL, Mat& textureFilename, unsigned int textureUnit, bool wrap)
{
	bool result;


	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(OpenGL);
	if(!result)
	{
		return false;
	}

	// Create the texture object.
	m_ScreenTexture = new TextureClass;
	if (!m_ScreenTexture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_ScreenTexture->Initialize(OpenGL, textureUnit, wrap);
	
	if (!result)
	{
		return false;
	}

	return true;
}

void ScreenQuad::updateTexture(OpenGLClass* OpenGL, Mat& textureFilename ){
	m_ScreenTexture->loadMatIntoTexture(OpenGL, textureFilename, 0);
}


void ScreenQuad::Shutdown(OpenGLClass* OpenGL)
{
	// Release the texture used for this model.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers(OpenGL);

	return;
}


void ScreenQuad::Render(OpenGLClass* OpenGL)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(OpenGL);

	return;
}


bool ScreenQuad::InitializeBuffers(OpenGLClass* OpenGL)
{
	VertexType* vertices;
	unsigned int* indices;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned int[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array with data.

	// Bottom left.
	vertices[0].x = -1.0f;  // Position.
	vertices[0].y = -1.0f;
	vertices[0].z =  0.0f;

	vertices[0].tu = 1.0f;  // Texture coordinates.
	vertices[0].tv = 1.0f;

	// Top left.
	vertices[1].x = -1.0f;  // Position.
	vertices[1].y = 1.0f;
	vertices[1].z = 0.0f;

	vertices[1].tu = 1.0f;  // Texture coordinates.
	vertices[1].tv = 0.0f;

	// Bottom right.
	vertices[2].x =  1.0f;  // Position.
	vertices[2].y = -1.0f;
	vertices[2].z =  0.0f;

	vertices[2].tu = 0.0f;  // Texture coordinates.
	vertices[2].tv = 1.0f;

	// Top right.
	vertices[3].x = 1.0f;  // Position.
	vertices[3].y = 1.0f;
	vertices[3].z = 0.0f;

	vertices[3].tu = 0.0f;  // Texture coordinates.
	vertices[3].tv = 0.0f;

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Bottom right.

	indices[3] = 2;  // Bottom right.
	indices[4] = 1;  // Top left.
	indices[5] = 3;  // Top right.


	// Allocate an OpenGL vertex array object.
	OpenGL->glGenVertexArrays(1, &m_vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Generate an ID for the vertex buffer.
	OpenGL->glGenBuffers(1, &m_vertexBufferId);

	// Bind the vertex buffer and load the vertex (position and texture) data into the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(VertexType), vertices, GL_STATIC_DRAW);

	// Enable the two vertex array attributes.
	OpenGL->glEnableVertexAttribArray(0);  // Vertex position.
	OpenGL->glEnableVertexAttribArray(1);  // Texture coordinates.

	// Specify the location and format of the position portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), 0);

	// Specify the location and format of the texture coordinate portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)NULL + (3 * sizeof(float)));

	// Generate an ID for the index buffer.
	OpenGL->glGenBuffers(1, &m_indexBufferId);

	// Bind the index buffer and load the index data into it.
	OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
	OpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount* sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	// Now that the buffers have been loaded we can release the array data.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ScreenQuad::ShutdownBuffers(OpenGLClass* OpenGL)
{
	// Disable the two vertex array attributes.
	OpenGL->glDisableVertexAttribArray(0);
	OpenGL->glDisableVertexAttribArray(1);
	
	// Release the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
	OpenGL->glDeleteBuffers(1, &m_vertexBufferId);

	// Release the index buffer.
	OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	OpenGL->glDeleteBuffers(1, &m_indexBufferId);

	// Release the vertex array object.
	OpenGL->glBindVertexArray(0);
	OpenGL->glDeleteVertexArrays(1, &m_vertexArrayId);

	return;
}


void ScreenQuad::RenderBuffers(OpenGLClass* OpenGL)
{
	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	return;
}


bool ScreenQuad::LoadTexture(OpenGLClass* OpenGL, char* textureFilename, unsigned int textureUnit, bool wrap)
{
	bool result;


	// Create the texture object.
	m_ScreenTexture = new TextureClass;
	if(!m_ScreenTexture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_ScreenTexture->Initialize(OpenGL, textureFilename, textureUnit, wrap);
	if(!result)
	{
		return false;
	}

	return true;
}


void ScreenQuad::ReleaseTexture()
{
	// Release the texture object.
	if(m_ScreenTexture)
	{
		m_ScreenTexture->Shutdown();
		delete m_ScreenTexture;
		m_ScreenTexture = 0;
	}

	return;
}