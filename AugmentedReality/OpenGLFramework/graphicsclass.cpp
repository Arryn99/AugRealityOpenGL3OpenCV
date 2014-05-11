#include "graphicsclass.h"
#include <opencv2\core\core.hpp>
#include <boost/thread.hpp>

//object detector
ObjectDetector m_ObjectDetector;
bool detecting = false;

void tryToFindObject(Mat& cameraFrame){
	while (detecting) {
		m_ObjectDetector.AnalyseFrame(cameraFrame);
	}
	detecting = false;
}

void printVector(glm::vec3& lightDirection, const char* print) {
	char numstr[21];
	sprintf(numstr, print);
	OutputDebugStringA(numstr);
	sprintf(numstr, " x %f ", lightDirection.x);
	OutputDebugStringA(numstr);
	sprintf(numstr, "y %f ", lightDirection.y);
	OutputDebugStringA(numstr);
	sprintf(numstr, "z %f \n", lightDirection.z);
	OutputDebugStringA(numstr);
}

void printValue(float distance, const char* print) {
	char numstr[21];
	sprintf(numstr, print);
	OutputDebugStringA(numstr);
	sprintf(numstr, ": %f \n", distance);
	OutputDebugStringA(numstr);
}


GraphicsClass::GraphicsClass()
{
	m_OpenGL = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;

	m_ScreenQuad = 0;
	m_ScreenTextureShader = 0;

	m_videoCapture = VideoCapture(0);
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(OpenGLClass* OpenGL, HWND hwnd)
{
	bool result;


	// Store a pointer to the OpenGL class object.
	m_OpenGL = OpenGL;

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, 0.0f);

	////////////////
	// Create the screen quad.
	m_ScreenQuad = new ScreenQuad;
	if (!m_ScreenQuad)
	{
		return false;
	}


	Mat object = imread("Photo2.jpg");
	cv::flip(object, object, 0);
	m_ScreenQuad->Initialize(m_OpenGL, object, 0, false);


	// Create the texture shader object.
	m_ScreenTextureShader = new ScreenTextureShader;
	if (!m_ScreenTextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_ScreenTextureShader->Initialize(m_OpenGL, hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	////////////////

	//m_model.Initialise("Models/drone.obj", "Models/EvilDrone_Diff.jpg");

	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_OpenGL, "cube.txt", "opengl.tga", 1, true);
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_ObjModel = new ModelClass;
	if (!m_ObjModel)
	{
		return false;
	}

	// Initialize the model object.
	result = m_ObjModel->InitializeObj(m_OpenGL, "Models/robot/drone.obj", "Models/robot/EvilDrone_Diff.png", 1, true);
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_OpenGL, hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	m_ObjectDetector.Init("businessCard.jpg");

	return true;
}


void GraphicsClass::Shutdown()
{
	detecting = false;

	// Release the texture shader object.
	if (m_ScreenTextureShader)
	{
		m_ScreenTextureShader->Shutdown(m_OpenGL);
		delete m_ScreenTextureShader;
		m_ScreenTextureShader = 0;
	}

	// Release the screen quad
	if (m_ScreenQuad)
	{
		m_ScreenQuad->Shutdown(m_OpenGL);
		delete m_ScreenQuad;
		m_ScreenQuad = 0;
	}

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown(m_OpenGL);
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown(m_OpenGL);
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the pointer to the OpenGL class object.
	m_OpenGL = 0;

	return;
}


bool GraphicsClass::Frame(int frameCount)
{
	bool result;
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += 0.0174532925f * 2.0f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	} 

	m_videoCapture >> cameraFrame;

	if (!detecting) {
			detecting = true;
			boost::thread objectDetectionThread(&tryToFindObject, cameraFrame);
	}


	m_ObjectDetector.drawDetections(cameraFrame);
	m_ObjectDetector.SetCameraCentre(cameraFrame.cols / 2, cameraFrame.rows / 2);

	// Render the graphics scene.
	result = Render(rotation);

	if (!result)
	{
		return false;
	}

	return true;
}

double x, y, z;
double yaw, pitch, roll;

glm::vec3 objectPosition = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 markerPosition = glm::vec3(0.0f, 0.0f, 0.0f);

float degrees = 0;
bool GraphicsClass::Render(float rotation)
{
	glm::mat4 worldMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 lightDirection;
	glm::mat4 diffuseLightColor;

	// Clear the buffers to begin the scene.
	m_OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();
	m_ScreenQuad->updateTexture(m_OpenGL, cameraFrame);

	// Set the texture shader as the current shader program and set the matrices that it will use for rendering.
	m_ScreenTextureShader->SetShader(m_OpenGL);
	m_ScreenTextureShader->SetShaderParameters(m_OpenGL, glm::value_ptr(worldMatrix), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), 0);

	//disable depth test
	glDisable(GL_DEPTH_TEST);

	// Render the model using the texture shader.
	m_ScreenQuad->Render(m_OpenGL);

	//enable depth test
	glEnable(GL_DEPTH_TEST);

	// Get the world, view, and projection matrices from the opengl and camera objects.
	m_Camera->GetViewMatrix(glm::value_ptr(viewMatrix));
	m_OpenGL->GetProjectionMatrix(glm::value_ptr(projectionMatrix));

	// Get the light properties.
	m_Light->GetDirection(glm::value_ptr(lightDirection));
	m_Light->GetDiffuseColor(glm::value_ptr(diffuseLightColor));

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	//
	ObjectDetectorResults* result = m_ObjectDetector.getResults();
	if (result != NULL) {
		m_ObjectDetector.GetYPR(yaw, pitch, roll);
		m_ObjectDetector.getObjectPosition(x, y, z);

		markerPosition = glm::vec3(x, y, z);
		markerPosition *= glm::vec3(-60, -50, 1); // correct for co-ordinate space.
	}
	
	float eularDegreesX = (float)glm::degrees(yaw);
	float eularDegreesY = (float)glm::degrees(pitch);
	float eularDegreesZ = (float)glm::degrees(roll);
	cout << "yaw" << eularDegreesX << "pitch" << eularDegreesY << "roll" << eularDegreesZ;

	glm::mat4 rotateX;
	glm::mat4 rotateY;
	glm::mat4 rotateZ;

	rotateX = glm::rotate(rotateX, -eularDegreesZ, glm::vec3(1.0f, 0.0f, 0.0f));
	rotateY = glm::rotate(rotateY, -eularDegreesY, glm::vec3(0.0f, 1.0f, 0.0f));
	rotateZ = glm::rotate(rotateZ, eularDegreesX, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 rotate = rotateX * rotateY  * rotateZ;// *rotateZ;

	// smooth between the object position and the marker position .
	float distance = glm::distance(objectPosition, markerPosition);
	if (distance  < 1000) {
		printValue(distance, "distance");
		float t = distance / 100.0f;
		printValue(t, "t");
		objectPosition = glm::mix(objectPosition, markerPosition, t);
	}
	else{
		objectPosition = markerPosition;
	}

	worldMatrix *= glm::translate(worldMatrix, objectPosition);
	worldMatrix *= glm::scale(worldMatrix, glm::vec3(30, 30, 30));
    worldMatrix *= rotate;

	// Set the light shader as the current shader program and set the matrices that it will use for rendering.
	m_LightShader->SetShader(m_OpenGL);
	m_LightShader->SetShaderParameters(m_OpenGL,  glm::value_ptr(worldMatrix), glm::value_ptr(viewMatrix),  glm::value_ptr(projectionMatrix), 1,  glm::value_ptr(lightDirection),  glm::value_ptr(diffuseLightColor));


	m_ObjModel->Render(m_OpenGL);

	/*glm::mat4 planet;
	degrees += 10;

	if (degrees > 180) {
		degrees = 10;
	}
	float sine = glm::sin(glm::radians(degrees *2));
	float sine2 = sine * sine;

	planet *= glm::rotate(planet, degrees, glm::vec3(0.0f, 0.0f, 1.0f));
	planet *= glm::translate(planet, objectPosition + glm::vec3(0, 1+ sine2 * 2, 0));*/
	
	//planet *= glm::scale(planet, glm::vec3(10, 10, 10));
	//planet *= rotate;
	


	//m_LightShader->SetShaderParameters(m_OpenGL, glm::value_ptr(planet), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), 1, glm::value_ptr(lightDirection), glm::value_ptr(diffuseLightColor));
	//m_ObjModel->Render(m_OpenGL);

	// Present the rendered scene to the screen.
	m_OpenGL->EndScene();

	return true;
}