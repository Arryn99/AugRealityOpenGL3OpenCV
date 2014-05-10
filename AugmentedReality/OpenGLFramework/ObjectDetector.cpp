#include "ObjectDetector.h"
#define M_PI       3.14159265358979323846
#include <boost/thread.hpp>
#include "RPP.h"

void ObjectDetectorResults::getObjectPosition(double& x, double& y, double& z) {
	if (m_translation.cols > 0) {
		x = m_translation.at<double>(0, 0);
		y = m_translation.at<double>(1, 0);
		z = m_translation.at<double>(2, 0);
	}
}

ObjectDetector::ObjectDetector() {
	m_fov = 60.0 * M_PI / 180.0;
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

	m_Obj_corners = std::vector<Point2f>(4);

	//Get the corners from the object
	//Draw a square around the object in screen space
	m_Obj_corners[0] = cvPoint(0, 0);
	m_Obj_corners[1] = cvPoint(m_MarkerObject.cols, 0);
	m_Obj_corners[2] = cvPoint(m_MarkerObject.cols, m_MarkerObject.rows);
	m_Obj_corners[3] = cvPoint(0, m_MarkerObject.rows);

	SetARObject(m_MarkerObject);
}

std::vector<vector<DMatch>> matches;

bool ObjectDetector::detectObject(Mat& grayScaleFrame) {
	std::vector< DMatch > good_matches;
	
	//use Surf detector to detect key points
	m_Detector.detect(grayScaleFrame, m_FrameKeyPoints);

	//compute a description object (feature vectors)
	m_Extractor.compute(grayScaleFrame, m_FrameKeyPoints, m_FrameDescription);
	
	//Matching descriptor vectors using FLANN matcher
	m_Matcher.knnMatch(m_MarkerObjectDescription, m_FrameDescription, matches, 2);

	//Quick calculation of max and min distances between keypoints
	for (int i = 0; i < min(m_MarkerObjectDescription.rows - 1, (int)matches.size()); i++)
	{
		if ((int)matches[i].size() <= 2 && (int)matches[i].size() > 0) {
			if (matches[i][0].distance < 0.95 * (matches[i][1].distance)) {
				good_matches.push_back(matches[i][0]);
			}
		}
	}

	//Localize the object

	if (good_matches.size() >= 4)
	{
		m_Obj.clear();
		m_Obj.resize(0);
		m_Scene.clear();
		m_Scene.resize(0);
		for (int i = 0; i < good_matches.size(); i++)
		{
			//Get the keypoints from the good matches
			m_Obj.push_back(m_MarkerObjectKeyPoints[good_matches[i].queryIdx].pt);
			m_Scene.push_back(m_FrameKeyPoints[good_matches[i].trainIdx].pt);
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
	//convert the camera frame to a grey scale image and save it in the image variable
	cvtColor(frame, m_greyScaleImage, CV_RGB2GRAY);

	if (detectObject(m_greyScaleImage)) {
		m_Homography = findHomography(m_Obj, m_Scene, CV_RANSAC);
		if (PoseEstimation(m_Homography)){
			UpdateAlphaBetaTracker();
		}
	}
	else{
		m_Homography.empty();
		//m_tracker.Reset();
	}
}

void ObjectDetector::drawDetections(Mat& frame) {

	if (m_Homography.cols > 0) {
		std::vector<Point2f> sceneCorners(4);
		perspectiveTransform(m_Obj_corners, sceneCorners, m_Homography);

		//Draw lines between the corners (the mapped object in the scene image )
		line(frame, sceneCorners[0], sceneCorners[1], Scalar(0, 255, 0), 4);
		line(frame, sceneCorners[1], sceneCorners[2], Scalar(0, 255, 0), 4);
		line(frame, sceneCorners[2], sceneCorners[3], Scalar(0, 255, 0), 4);
		line(frame, sceneCorners[3], sceneCorners[0], Scalar(0, 255, 0), 4);
	}

}

void ObjectDetector::UpdateParameters()
{
	m_focal = m_cx / tan(m_fov*0.5*M_PI / 180.0);

	m_camera_intrinsics = cv::Mat::eye(3, 3, CV_64F);

	m_camera_intrinsics.at<double>(0, 0) = m_focal;
	m_camera_intrinsics.at<double>(1, 1) = m_focal;
	m_camera_intrinsics.at<double>(0, 2) = m_cx;
	m_camera_intrinsics.at<double>(1, 2) = m_cy;

	m_inv_camera_intrinsics = m_camera_intrinsics.inv();

	m_vfov = atan(m_cy / m_focal)*180.0 / M_PI*2.0;
}

void ObjectDetector::SetCameraCentre(double cx, double cy)
{
	m_cx = cx;
	m_cy = cy;
	UpdateParameters();
}

bool ObjectDetector::PoseEstimation(const cv::Mat &H)
{
	boost::posix_time::ptime t1, t2;

	t1 = boost::posix_time::microsec_clock::local_time();

	// H * m_AR_object_image_pts = 4 corner points
	// I use the 4 corners instead of ALL the points.
	// Why do I do this? It's faster, but maybe less accurate?
	cv::Mat image_pts = m_inv_camera_intrinsics * H * m_AR_object_image_pts;

	double obj_err, img_err;
	int it;

	bool status = RPP::Rpp(m_AR_oject_worlds_pts, image_pts, m_AR_object_rotation, m_AR_object_translation, it, obj_err, img_err);

	if (!status) {
		return false;
	}

	t2 = boost::posix_time::microsec_clock::local_time();

	cout << "RPP: " << (t2 - t1).total_milliseconds() << " ms" << endl;

	return true;
}


void ObjectDetector::SetARObject(const cv::Mat &AR_object)
{
	int m_AR_object_width = AR_object.cols;
	int m_AR_object_height = AR_object.rows;

	// Find the largest dimension needed for creating temporary warped images
	int size = (int)sqrt(m_AR_object_width*m_AR_object_width*0.25 + m_AR_object_height*m_AR_object_height*0.25) * 2;
	size = (size / 4) * 4 + 4;

	cv::Mat AR_object_grey = AR_object;
	//cv::cvtColor(AR_object, AR_object_grey, CV_BGR2GRAY);

	// Initialse other matrices
	m_AR_object_image_pts = cv::Mat::zeros(3, 4, CV_64F);

	// 4 corner points of the model
	// top left
	m_AR_object_image_pts.at<double>(0, 0) = 0;
	m_AR_object_image_pts.at<double>(1, 0) = 0;
	m_AR_object_image_pts.at<double>(2, 0) = 1;

	// top right
	m_AR_object_image_pts.at<double>(0, 1) = m_AR_object_width;
	m_AR_object_image_pts.at<double>(1, 1) = 0;
	m_AR_object_image_pts.at<double>(2, 1) = 1;

	// bottom left
	m_AR_object_image_pts.at<double>(0, 2) = m_AR_object_width;
	m_AR_object_image_pts.at<double>(1, 2) = m_AR_object_height;
	m_AR_object_image_pts.at<double>(2, 2) = 1;

	// bottom right
	m_AR_object_image_pts.at<double>(0, 3) = 0;
	m_AR_object_image_pts.at<double>(1, 3) = m_AR_object_height;
	m_AR_object_image_pts.at<double>(2, 3) = 1;

	double scale = 1.0 / (double)m_AR_object_width;
	cv::Mat normalise_2D_mat = cv::Mat::eye(3, 3, CV_64F);
	normalise_2D_mat.at<double>(0, 0) = scale;
	normalise_2D_mat.at<double>(1, 1) = scale;
	normalise_2D_mat.at<double>(0, 2) = -scale*m_AR_object_width*0.5;
	normalise_2D_mat.at<double>(1, 2) = -scale*m_AR_object_height*0.5;

	m_AR_oject_worlds_pts = normalise_2D_mat*m_AR_object_image_pts;

	for (int i = 0; i < m_AR_oject_worlds_pts.cols; i++) {
		m_AR_oject_worlds_pts.at<double>(2, i) = 0.0; // z value
	}

}

void ObjectDetector::GetYPR(double &yaw, double &pitch, double &roll){
	if (m_AR_object_rotation.rows > 0){
		GetYPR(m_AR_object_rotation, yaw, pitch, roll);
	}
}

void ObjectDetector::GetYPR(const cv::Mat &rot, double &yaw, double &pitch, double &roll)
{
	// http://planning.cs.uiuc.edu/node103.html
	yaw = atan2(rot.at<double>(1, 0), rot.at<double>(0, 0));
	pitch = atan2(-rot.at<double>(2, 0), sqrt(rot.at<double>(2, 1)*rot.at<double>(2, 1) + rot.at<double>(2, 2)*rot.at<double>(2, 2)));
	roll = atan2(rot.at<double>(2, 1), rot.at<double>(2, 2));
}

void ObjectDetector::getObjectPosition(double& x, double& y, double& z) {
	if (m_AR_object_translation.cols > 0) {
		x = m_AR_object_translation.at<double>(0, 0);
		y = m_AR_object_translation.at<double>(1, 0);
		z = m_AR_object_translation.at<double>(2, 0);
	}
}

void ObjectDetector::UpdateAlphaBetaTracker()
{
	double yaw = 0, pitch = 0, roll = 0;
	double x, y, z;

	x = m_AR_object_translation.at<double>(0, 0);
	y = m_AR_object_translation.at<double>(1, 0);
	z = m_AR_object_translation.at<double>(2, 0);

	GetYPR(m_AR_object_rotation, yaw, pitch, roll);

		//m_tracker.SetState(x, y, z, yaw, pitch, roll);

//	if (m_tracker.Ready()) {
	//	m_tracker.GetCorrectedState(&x, &y, &z, &yaw, &pitch, &roll);

		m_AR_object_rotation = MakeRotation3x3(yaw, pitch, roll);
		m_AR_object_translation.at<double>(0, 0) = x;
		m_AR_object_translation.at<double>(1, 0) = y;
		m_AR_object_translation.at<double>(2, 0) = z;

		cv::Point2i ret_corners[4];
		ProjectModel(x, y, z, yaw, pitch, roll, ret_corners);
	//}

	ObjectDetectorResults result = ObjectDetectorResults(m_AR_object_rotation, m_AR_object_translation);
	m_Results.push_back(result);
}

void ObjectDetector::ProjectModel(double x, double y, double z, double yaw, double pitch, double roll, cv::Point2i ret_corners[4])
{
	cv::Mat rot = MakeRotation3x3(yaw, pitch, roll);

	cv::Mat model_pts = rot*m_AR_oject_worlds_pts;

	for (int i = 0; i < 4; i++) {
		model_pts.at<double>(0, i) += x;
		model_pts.at<double>(1, i) += y;
		model_pts.at<double>(2, i) += z;
	}

	cv::Mat pts = m_camera_intrinsics*model_pts;

	for (int i = 0; i < 4; i++) {
		ret_corners[i].x = (int)(pts.at<double>(0, i) / pts.at<double>(2, i) + 0.5);
		ret_corners[i].y = (int)(pts.at<double>(1, i) / pts.at<double>(2, i) + 0.5);
	}
}

cv::Mat ObjectDetector::MakeRotation3x3(double yaw, double pitch, double roll)
{
	cv::Mat rot(3, 3, CV_64F);

	double sina = sin(yaw);
	double cosa = cos(yaw);

	double sinb = sin(pitch);
	double cosb = cos(pitch);

	double sinc = sin(roll);
	double cosc = cos(roll);

	rot.at<double>(0, 0) = cosa*cosb;
	rot.at<double>(0, 1) = cosa*sinb*sinc - sina*cosc;
	rot.at<double>(0, 2) = cosa*sinb*cosc + sina*sinc;

	rot.at<double>(1, 0) = sina*cosb;
	rot.at<double>(1, 1) = sina*sinb*sinc + cosa*cosc;
	rot.at<double>(1, 2) = sina*sinb*cosc - cosa*sinc;

	rot.at<double>(2, 0) = -sinb;
	rot.at<double>(2, 1) = cosb*sinc;
	rot.at<double>(2, 2) = cosb*cosc;

	return rot;
}

ObjectDetectorResults* ObjectDetector::getResults() {
	if (m_Results.size() > 0) {
		ObjectDetectorResults result = m_Results[m_Results.size() - 1];
		m_Results.pop_back();
		return &result;
	}
	else {
		return NULL;
	}
}
