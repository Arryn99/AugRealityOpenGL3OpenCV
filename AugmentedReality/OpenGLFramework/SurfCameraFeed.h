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

	int OpenCVCameraInit();
	int OpenCVCameraFrame();

private:

	VideoCapture* m_Cap;
	char m_Key;
	int m_Framecount;
	int m_MinHessian;

	std::vector<KeyPoint> m_kp_object;
	std::vector<Point2f> m_objCorners;
	std::vector<KeyPoint> m_kp_image;
	std::vector<DMatch> m_GoodMatches;
	std::vector<Point2f> m_Obj;
	std::vector<Point2f> m_Scene;
	std::vector<Point2f> m_SceneCorners;
	std::vector<vector<DMatch>> m_Matches;

	SurfDescriptorExtractor m_Extractor;
	SurfFeatureDetector m_Detector;
	FlannBasedMatcher m_Matcher;

	Mat des_object;
	Mat object;
	Mat frame;
	Mat H;
	Mat image;
	Mat des_image, img_matches;
	
};

#endif
