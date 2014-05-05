#ifndef _OBEJECTDETECTOR_H_
#define _OBEJECTDETECTOR_H_

#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class ObjectDetectorResults  {
public:
	ObjectDetectorResults(Mat& rotation, Mat& translation){
		m_rotation = rotation;
		m_translation = translation;
	}

	void getObjectPosition(double& x, double& y, double& z);

	double x, y, z;
private:
	Mat m_rotation;
	Mat m_translation;
};



class ObjectDetector {
public:
	ObjectDetector();
	~ObjectDetector();

	int OpenCVCamera();
	int Init(string);
	int Frame();

	//analyses a camera frame and returns a matrix representing a marker's position. 
	void AnalyseFrame(Mat& frame);

	//calculates a homography matrix from m_MarkerObject & grayScaleFrame
	bool detectObject(Mat& grayScaleFrame);
	void drawDetections(Mat& frame);

	void SetCameraCentre(double cx, double cy);
	bool PoseEstimation(const cv::Mat &H);
	void SetARObject(const cv::Mat &AR_object);

	void GetYPR(const cv::Mat &rot, double &yaw, double &pitch, double &roll);
	void GetYPR(double &yaw, double &pitch, double &roll);

	void getObjectPosition(double& x, double& y, double& z);
	void UpdateAlphaBetaTracker();
	void ProjectModel(double x, double y, double z, double yaw, double pitch, double roll, cv::Point2i ret_corners[4]);
	
	cv::Mat MakeRotation3x3(double yaw, double pitch, double roll);
	ObjectDetectorResults* getResults();

	cv::Mat m_camera_intrinsics; // 3x3 camera matrix
	cv::Mat m_inv_camera_intrinsics; // 3x3 camera matrix
private:
	VideoCapture m_Capture;
	void UpdateParameters();

	/*=========== OpenCV utility objects ============*/

	SurfFeatureDetector m_Detector;
	SurfDescriptorExtractor m_Extractor;
	FlannBasedMatcher m_Matcher;

	/*=========== Variables calculated during init function ============*/

	//the image we are looking for
	Mat m_MarkerObject;
	//key points extracted during init function for marker object
	std::vector<KeyPoint> m_MarkerObjectKeyPoints;
	//description object for m_MarkerObject (feature vector) calculated during init function
	Mat m_MarkerObjectDescription;

	/*=========== Variables calculated during update function ============*/
	Mat m_greyScaleImage;
	Mat m_FrameDescription;
	std::vector<KeyPoint> m_FrameKeyPoints; //an array of keypoints in the image for this frame

	//array of 2D points representing corners of image
	std::vector<Point2f> m_Obj_corners;
	std::vector<Point2f> m_Obj;
	std::vector<Point2f> m_Scene;

	Mat m_Homography;

	/*=========== camera intrinsics ============*/
	
	double m_fov; // horizontal field of view in degrees
	double m_focal; // calculated from m_fov and model size
	double m_cx, m_cy; // camera optical centre
	double m_vfov; // for OpenGL, vertical field of view in degrees

	/*====================== nar variables ==============================*/
	cv::Mat m_AR_object_image_pts; // 4 corners, image points, never changes
	cv::Mat m_AR_oject_worlds_pts; // 4 corners, world points, constantly changing

	// From pose estimation
	cv::Mat m_AR_object_rotation;
	cv::Mat m_AR_object_translation;

	std::vector<ObjectDetectorResults> m_Results;

};

#endif
