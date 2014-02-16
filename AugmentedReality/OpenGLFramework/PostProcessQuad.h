#ifndef _POSTPROCESSQUAD_H_
#define _POSTPROCESSQUAD_H_

#include "textureclass.h"

class PostProcessQuad {

public:
	PostProcessQuad();
	~PostProcessQuad();

	bool render(OpenGLClass* OpenGL);

private:
	struct VertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

};

#endif