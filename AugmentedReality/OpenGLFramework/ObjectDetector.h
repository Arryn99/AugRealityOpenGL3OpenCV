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

class ObjectDetector {
public:
	ObjectDetector();
	~ObjectDetector();

	int OpenCVCamera();
	int Init(string);
	int Frame();

	//analyses a camera frame and returns a matrix representing a marker's position. 
	void AnalyseFrame(Mat frame);

	//calculates a homography matrix from m_MarkerObject & grayScaleFrame
	Mat CalculateHomography(Mat& grayScaleFrame);

private:
	VideoCapture m_Capture;

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
	Mat m_FrameDescription;
	std::vector<KeyPoint> m_FrameKeyPoints; //an array of keypoints in the image for this frame

	//array of 2D points representing corners of image
	std::vector<Point2f> obj_corners;

	char m_Key;
    int m_Framecount;
};

#endif
