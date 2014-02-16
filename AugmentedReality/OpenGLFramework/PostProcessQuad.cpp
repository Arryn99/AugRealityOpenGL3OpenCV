#include "PostProcessQuad.h"

PostProcessQuad::PostProcessQuad(){
};

PostProcessQuad::~PostProcessQuad() {
};

bool PostProcessQuad::render(OpenGLClass* OpenGL) {
	VertexType* vertices;
	unsigned int* indices;
	int i;


	// Create the vertex array.
	vertices = new VertexType[4];

	vertices[0].x = -1.0f;
	vertices[0].y = 1.0f;
	vertices[0].z = 0.0f;

	vertices[1].x = 1.0f;
	vertices[1].y = 1.0f;
	vertices[1].z = 0.0f;

	vertices[2].x = 1.0f;
	vertices[2].y = -1.0f;
	vertices[2].z = 0.0f;

	vertices[3].x = -1.0f;
	vertices[3].y = -1.0f;
	vertices[3].z = 0.0f;

	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned int[6];
	if(!indices)
	{
		return false;
	}

	indices[0] = 2;
	indices[1] = 0;
	indices[2] = 3;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 0;


	// Allocate an OpenGL vertex array object.
	OpenGL->glGenVertexArrays(1, &m_vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Generate an ID for the vertex buffer.
	OpenGL->glGenBuffers(1, &m_vertexBufferId);

	// Bind the vertex buffer and load the vertex (position, texture, and normal) data into the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(VertexType), vertices, GL_STATIC_DRAW);

	// Enable the three vertex array attributes.
	OpenGL->glEnableVertexAttribArray(0);  // Vertex position.
	OpenGL->glEnableVertexAttribArray(1);  // Texture coordinates.
	OpenGL->glEnableVertexAttribArray(2);  // Normals.

	// Specify the location and format of the position portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), 0);

	// Specify the location and format of the texture coordinate portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)NULL + (3 * sizeof(float)));

	// Specify the location and format of the normal vector portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)NULL + (5 * sizeof(float)));

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
}
