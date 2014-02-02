////////////////////////////////////////////////////////////////////////////////
// Filename: CameraClass.h
// This camera class to lets OpenGL know from where and also how we are viewing the scene. The camera class will keep track of
// where the camera is and its current rotation. It will use the position and rotation information to generate a view matrix which
// will be passed into the GLSL shaders for rendering.
// The CameraClass header is quite simple with just four functions that will be used.
// The SetPosition and SetRotation functions will be used to set the position and rotation of the camera object.
// Render will be used to create the view matrix based on the position and rotation of the camera.
// And finally GetViewMatrix will be used to retrieve the view matrix from the camera object so that the shaders can use it for rendering.
////////////////////////////////////////////////////////////////////////////////

#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <math.h>

class CameraClass
{
private:
	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void Render();
	void GetViewMatrix(float*);

private:
	void MatrixRotationYawPitchRoll(float*, float, float, float);
	void TransformCoord(VectorType&, float*);
	void BuildViewMatrix(VectorType, VectorType, VectorType);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_viewMatrix[16];
};

#endif