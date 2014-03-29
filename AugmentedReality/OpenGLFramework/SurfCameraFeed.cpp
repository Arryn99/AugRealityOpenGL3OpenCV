#include "SurfCameraFeed.h"

SurfCameraFeed::SurfCameraFeed() {
};

SurfCameraFeed::~SurfCameraFeed() {
};

int SurfCameraFeed::Init(string filename) {

	m_MarkerObject = imread(filename, CV_LOAD_IMAGE_GRAYSCALE );

	if( !m_MarkerObject.data )
	{
		std::cout<< "Error reading object " << std::endl;
		return -1;
	}

	//Detect the keypoints using SURF Detector
	int minHessian = 500;

	m_Detector = SurfFeatureDetector(minHessian);

	m_Detector.detect( m_MarkerObject, m_MarkerObjectKeyPoints );
	m_Extractor.compute(m_MarkerObject, m_MarkerObjectKeyPoints, m_MarkerObjectDescription );
	m_Capture = VideoCapture(0);

	namedWindow("Good Matches");

	obj_corners = std::vector<Point2f>(4);

	//Get the corners from the object
	//Draw a square around the object in screen space
	obj_corners[0] = cvPoint(0,0);
	obj_corners[1] = cvPoint( m_MarkerObject.cols, 0 );
	obj_corners[2] = cvPoint( m_MarkerObject.cols, m_MarkerObject.rows );
	obj_corners[3] = cvPoint( 0, m_MarkerObject.rows );

	m_Key = 'a';
	m_Framecount = 0;
}

Mat SurfCameraFeed::CalculateHomography(Mat grayScaleFrame) {

	//use Surf detector to detect key points
	m_Detector.detect(grayScaleFrame, m_FrameKeyPoints);

	//compute a description object (feature vectors)
	m_Extractor.compute(grayScaleFrame, m_FrameKeyPoints, m_FrameDescription);

	//Matching descriptor vectors using FLANN matcher
	std::vector<vector<DMatch>> matches;
	m_Matcher.knnMatch(m_MarkerObjectDescription, m_FrameDescription, matches, 2);	

	std::vector< DMatch > good_matches;

	//Quick calculation of max and min distances between keypoints
	for(int i = 0; i < min(m_MarkerObjectDescription.rows-1,(int) matches.size()); i++)
	{
		if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
		{
			good_matches.push_back(matches[i][0]);
		}
	}

	//Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	//Draw only "good" matches
	//drawMatches( m_MarkerObject, m_MarkerObjectKeyPoints, greyScaleImage, m_FrameKeyPoints, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	if (good_matches.size() >= 4)
	{
		for( int i = 0; i < good_matches.size(); i++ )
		{
			//Get the keypoints from the good matches
			obj.push_back( m_MarkerObjectKeyPoints[ good_matches[i].queryIdx ].pt );
			scene.push_back( m_FrameKeyPoints[ good_matches[i].trainIdx ].pt );
		}
		return findHomography(obj, scene, CV_RANSAC);
	}
	return Mat();
}

/**
Analyses each frame of camera feed.
@param frame - a matrix representing the camera frame
*/
void SurfCameraFeed::AnalyseFrame(Mat frame) {

	Mat frameDescription;
	std::vector<KeyPoint> frameKeyPoints; //an array of keypoints in the image for this frame
	Mat greyScaleImage;

	//convert the camera frame to a grey scale image and save it in the image variable
	cvtColor(frame, greyScaleImage, CV_RGB2GRAY);

	Mat H = CalculateHomography(greyScaleImage);

	std::vector<Point2f> sceneCorners(4);

	if (H.cols > 0) {
		perspectiveTransform( obj_corners, sceneCorners, H);
	}

	//Draw lines between the corners (the mapped object in the scene image )
	line( frame, sceneCorners[0] + Point2f( m_MarkerObject.cols, 0), sceneCorners[1] + Point2f( m_MarkerObject.cols, 0), Scalar(0, 255, 0), 4 );
	line( frame, sceneCorners[1] + Point2f( m_MarkerObject.cols, 0), sceneCorners[2] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
	line( frame, sceneCorners[2] + Point2f( m_MarkerObject.cols, 0), sceneCorners[3] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
	line( frame, sceneCorners[3] + Point2f( m_MarkerObject.cols, 0), sceneCorners[0] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
	
	//Show detected matches
	imshow("Good Matches", frame);
}

int SurfCameraFeed::Frame() {

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