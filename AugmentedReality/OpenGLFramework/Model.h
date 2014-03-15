
#ifndef _MODEL_H_
#define _MODEL_H_


// INCLUDES //
#include <Windows.h>
#include <fstream>
#include <gl/gl.h>
#include <gl/glu.h>
using namespace std;


// MY CLASS INCLUDES //
#include "TokenStream.h"
#include <vector>
#include "Vec3.h"
#include "SOIL.h"

class Model
{
private:

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	
	bool Initialise(char*, char*);
	void Render();
	int GetIndexCount();
	
private:
	
	void LoadTexture(char*);
	bool LoadModel(char*);
	
	private:
	
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	GLint texture;

	std::vector<float> vertex, normals, texCoords;	
};

#endif