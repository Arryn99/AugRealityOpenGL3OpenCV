////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "modelclass.h"


ModelClass::ModelClass()
{
	m_Texture = 0; //initialise the texture object to null
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

//The Initialize function will call the initialization functions for the vertex and index buffers.

//Initialize takes as input the file name of the .tga texture that the model will be using. It also takes as
//input the texture unit in which to load the .tga file into. And it also takes a boolean value which indicates if
//the color should wrap around or clamp at the edges.

bool ModelClass::Initialize(OpenGLClass* OpenGL, char* textureFilename, unsigned int textureUnit, bool wrap)
{
	bool result;


	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(OpenGL);
	if(!result)
	{
		return false;
	}

	//the initialise function calls a new private function that will load the .tga texture.
	// Load the texture for this model.
	result = LoadTexture(OpenGL, textureFilename, textureUnit, wrap);
	if(!result)
	{
		return false;
	}

	return true;
}


//The Shutdown function will call the shutdown functions for the buffers and related data.
void ModelClass::Shutdown(OpenGLClass* OpenGL)
{
	// Release the texture used for this model.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers(OpenGL);

	return;
}

//Render is called from the GraphicsClass::Render function. This function calls RenderBuffers to put
//the vertex and index buffers on the graphics pipeline and uses the colour shader to render them.
void ModelClass::Render(OpenGLClass* OpenGL)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(OpenGL);

	return;
}


//The InitializeBuffers function is where we handle creating the vertex and index buffers.
bool ModelClass::InitializeBuffers(OpenGLClass* OpenGL)
{
	VertexType* vertices;
	unsigned int* indices;

	//create two temporary arrays to hold the vertex and index data that we will use later to populate the final buffers with.

	// Set the number of vertices in the vertex array.
	m_vertexCount = 3;

	// Set the number of indices in the index array.
	m_indexCount = 3;

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

	//Each vertex has normals associated with it for lighting calculations.
	//The normal is a line that is perpendicular to the face of the polygon so that the exact direction
	//the face is pointing can be calculated. For simplicity purposes the normal for each vertex is set along
	//the Z axis by setting each Z component to -1.0f which makes the normal point towards the viewer.
	
	// Load the vertex array with data.

	// Bottom left.
	vertices[0].x = -1.0f;  // Position.
	vertices[0].y = -1.0f;
	vertices[0].z =  0.0f;

	vertices[0].tu = 0.0f;  // Texture coordinates.
	vertices[0].tv = 0.0f;

	vertices[0].nx =  0.0f;	//Normals
	vertices[0].ny =  0.0f;
	vertices[0].nz = -1.0f;

	// Top middle.
	vertices[1].x = 0.0f;  // Position.
	vertices[1].y = 1.0f;
	vertices[1].z = 0.0f;

	vertices[1].tu = 0.5f;  // Texture coordinates.
	vertices[1].tv = 1.0f;

	vertices[1].nx =  0.0f;	//Normals
	vertices[1].ny =  0.0f;
	vertices[1].nz = -1.0f;

	// Bottom right.
	vertices[2].x =  1.0f;  // Position.
	vertices[2].y = -1.0f;
	vertices[2].z =  0.0f;

	vertices[2].tu = 1.0f;  // Texture coordinates.
	vertices[2].tv = 0.0f;

	vertices[2].nx =  0.0f;	//Normals
	vertices[2].ny =  0.0f;
	vertices[2].nz = -1.0f;

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

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
	OpenGL->glEnableVertexAttribArray(2);	//Normals

	// Specify the location and format of the position portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), 0);

	// Specify the location and format of the texture coordinate portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)NULL + (3 * sizeof(float)));

	//Specify the location and format of the normal vector portion of the vertex buffer
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(2,3,GL_FLOAT,false, sizeof(VertexType), (unsigned char*)NULL + (5 * sizeof(float)));

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

//The ShutdownBuffers function releases the buffers and attributes that were created in the InitializeBuffers function.
void ModelClass::ShutdownBuffers(OpenGLClass* OpenGL)
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

//RenderBuffers is called from the Render function. The purpose of this function is to set the vertex buffer and index buffer as
//active on the input assembler in the GPU by binding the OpenGL vertex array object. Once the GPU has an active vertex buffer
//it can then use the currently set shader to render that buffer. This function also defines how those buffers should be drawn such
//as triangles, lines, fans, and so forth.
void ModelClass::RenderBuffers(OpenGLClass* OpenGL)
{
	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	return;
}

//LoadTexture is a private function that will create the texture object and then initialize it with the input file
//name provided. This function is called during initialization.
bool ModelClass::LoadTexture(OpenGLClass* OpenGL, char* textureFilename, unsigned int textureUnit, bool wrap)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(OpenGL, textureFilename, textureUnit, wrap);
	if(!result)
	{
		return false;
	}

	return true;
}

//The ReleaseTexture function will release the texture object that was created and loaded during
//the LoadTexture function.
void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}