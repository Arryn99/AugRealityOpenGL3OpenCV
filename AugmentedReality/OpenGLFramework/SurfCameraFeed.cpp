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

	m_Detector.detect( m_MarkerObject, m_MarkerObject_KeyPoints );
	m_Extractor.compute(m_MarkerObject, m_MarkerObject_KeyPoints, m_MarkerObject_Description );
	m_Capture = VideoCapture(0);

	namedWindow("Good Matches");

	obj_corners = std::vector<Point2f>(4);

	//Draw a square around the object in screen space
	//Get the corners from the object
	obj_corners[0] = cvPoint(0,0);
	obj_corners[1] = cvPoint( m_MarkerObject.cols, 0 );
	obj_corners[2] = cvPoint( m_MarkerObject.cols, m_MarkerObject.rows );
	obj_corners[3] = cvPoint( 0, m_MarkerObject.rows );

	key = 'a';
	framecount = 0;
}

Mat SurfCameraFeed::CalculateHomography(Mat grayScaleFrame) {

	//use Surf detector to detect key points
	m_Detector.detect( grayScaleFrame, m_Frame_KeyPoints);

	//compute a description object (feature vectors)
	m_Extractor.compute( grayScaleFrame, m_Frame_KeyPoints, m_Frame_Description);

	//Matching descriptor vectors using FLANN matcher
	std::vector<DMatch > matches;
	m_Matcher.match(m_MarkerObject_Description, m_Frame_Description, matches);	

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < m_MarkerObject_Description.rows; i++ ) {
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist );
	printf("-- Min dist : %f \n", min_dist );

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for( int i = 0; i < m_MarkerObject_Description.rows; i++ )
	{ if( matches[i].distance < 3*min_dist )
	{ good_matches.push_back( matches[i]); }
	}

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for( int i = 0; i < good_matches.size(); i++ )
	{
		//-- Get the keypoints from the good matches
		obj.push_back( m_MarkerObject_KeyPoints[ good_matches[i].queryIdx ].pt );
		scene.push_back( m_Frame_KeyPoints[good_matches[i].trainIdx].pt );
	}

	return findHomography(obj, scene, CV_RANSAC);
}

/**
Analyses each frame of camera feed.
@param frame - a matrix representing the camera frame
*/
void SurfCameraFeed::AnalyseFrame(Mat frame) {

	Mat frame_description;
	std::vector<KeyPoint> frame_keyPoints; //an array of keypoints in the image for this frame
	Mat greyScaleImage;

	//convert the camera frame to a grey scale image and save it in the image variable
	cvtColor(frame, greyScaleImage, CV_RGB2GRAY);

	Mat H = CalculateHomography(greyScaleImage);

	std::vector<Point2f> scene_corners(4);

	perspectiveTransform( obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line( frame, scene_corners[0] + Point2f( m_MarkerObject.cols, 0), scene_corners[1] + Point2f( m_MarkerObject.cols, 0), Scalar(0, 255, 0), 4 );
	line( frame, scene_corners[1] + Point2f( m_MarkerObject.cols, 0), scene_corners[2] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
	line( frame, scene_corners[2] + Point2f( m_MarkerObject.cols, 0), scene_corners[3] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
	line( frame, scene_corners[3] + Point2f( m_MarkerObject.cols, 0), scene_corners[0] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );

	//-- Show detected matches
	imshow("Good Matches", frame );

}
int SurfCameraFeed::Frame() {

	Mat frame;
	m_Capture >> frame;

	if (framecount < 5)
	{
		framecount++;
		return 0;
	}

	AnalyseFrame(frame);

	key = waitKey(1);
	return 0;
}