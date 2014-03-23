#ifndef _SURFCAMERA_H_
#define _SURFCAMERA_H_

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

class SurfCameraFeed {
public:
	SurfCameraFeed();
	~SurfCameraFeed();

	int OpenCVCamera();
	int Init(string);
	int Frame();

	//analyses a camera frame and returns a matrix representing a markers position. 
	void AnalyseFrame(Mat frame);

private:
	Mat m_MarkerObject;
	SurfFeatureDetector m_Detector;
	std::vector<KeyPoint> m_KeyPointObject;
	SurfDescriptorExtractor m_Extractor;
    Mat m_DescriptionObject;
	FlannBasedMatcher m_Matcher;

	VideoCapture m_Capture;

	std::vector<Point2f> obj_corners;

	char key;
    int framecount;
};

#endif
