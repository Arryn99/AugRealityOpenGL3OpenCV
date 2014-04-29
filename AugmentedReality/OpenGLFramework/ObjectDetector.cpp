#include "ObjectDetector.h"

ObjectDetector::ObjectDetector() {
};

ObjectDetector::~ObjectDetector() {
};

int ObjectDetector::Init(string filename) {

	m_MarkerObject = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

	if (!m_MarkerObject.data)
	{
		std::cout << "Error reading object " << std::endl;
		return -1;
	}

	//Detect the keypoints using SURF Detector
	int minHessian = 500;

	m_Detector = SurfFeatureDetector(minHessian);

	m_Detector.detect(m_MarkerObject, m_MarkerObjectKeyPoints);
	m_Extractor.compute(m_MarkerObject, m_MarkerObjectKeyPoints, m_MarkerObjectDescription);
	m_Capture = VideoCapture(0);

	obj_corners = std::vector<Point2f>(4);

	//Get the corners from the object
	//Draw a square around the object in screen space
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(m_MarkerObject.cols, 0);
	obj_corners[2] = cvPoint(m_MarkerObject.cols, m_MarkerObject.rows);
	obj_corners[3] = cvPoint(0, m_MarkerObject.rows);

	m_Key = 'a';
	m_Framecount = 0;
}
std::vector<vector<DMatch>> matches;

bool ObjectDetector::detectObject(Mat& grayScaleFrame) {

	//use Surf detector to detect key points
	m_Detector.detect(grayScaleFrame, m_FrameKeyPoints);

	//compute a description object (feature vectors)
	m_Extractor.compute(grayScaleFrame, m_FrameKeyPoints, m_FrameDescription);

	//Matching descriptor vectors using FLANN matcher

	m_Matcher.knnMatch(m_MarkerObjectDescription, m_FrameDescription, matches, 2);

	std::vector< DMatch > good_matches;

	//Quick calculation of max and min distances between keypoints
	for (int i = 0; i < min(m_MarkerObjectDescription.rows - 1, (int)matches.size()); i++)
	{
		if ((int)matches[i].size() <= 2 && (int)matches[i].size() > 0) {
			if (matches[i][0].distance < 0.6*(matches[i][1].distance)) {
				good_matches.push_back(matches[i][0]);
			}
		}
	}

	//Localize the object
	obj.clear();
	scene.clear();

	if (good_matches.size() >= 4)
	{
		for (int i = 0; i < good_matches.size(); i++)
		{
			//Get the keypoints from the good matches
			obj.push_back(m_MarkerObjectKeyPoints[good_matches[i].queryIdx].pt);
			scene.push_back(m_FrameKeyPoints[good_matches[i].trainIdx].pt);
		}
		return true;
	}
	return false;
}

/**
Analyses each frame of camera feed.
@param frame - a matrix representing the camera frame
*/
void ObjectDetector::AnalyseFrame(Mat& frame) {

	Mat frameDescription;
	std::vector<KeyPoint> frameKeyPoints; //an array of keypoints in the image for this frame
	Mat greyScaleImage;

	//convert the camera frame to a grey scale image and save it in the image variable
	cvtColor(frame, greyScaleImage, CV_RGB2GRAY);

	if (detectObject(greyScaleImage)) {
		mHomography = findHomography(obj, scene, CV_RANSAC);
	}
	else{
		mHomography.empty();
	}
}

void ObjectDetector::drawDetections(Mat& frame) {
	
	if (mHomography.cols > 0) {
		std::vector<Point2f> sceneCorners(4);
		perspectiveTransform(obj_corners, sceneCorners, mHomography);

		//Draw lines between the corners (the mapped object in the scene image )
		line(frame, sceneCorners[0], sceneCorners[1], Scalar(0, 255, 0), 4);
		line(frame, sceneCorners[1], sceneCorners[2], Scalar(0, 255, 0), 4);
		line(frame, sceneCorners[2], sceneCorners[3], Scalar(0, 255, 0), 4);
		line(frame, sceneCorners[3], sceneCorners[0], Scalar(0, 255, 0), 4);
	}

}

int ObjectDetector::Frame() {

	Mat frame;
	m_Capture >> frame;

	if (m_Framecount < 5)
	{
		m_Framecount++;
		return 0;
	}

	AnalyseFrame(frame);

	m_Key = waitKey(1);
	return 0;
}