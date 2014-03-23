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

	m_Detector.detect( m_MarkerObject, m_KeyPointObject );
	m_Extractor.compute(m_MarkerObject, m_KeyPointObject, m_DescriptionObject );
	m_Capture = VideoCapture(0);

	namedWindow("Good Matches");

	obj_corners = std::vector<Point2f>(4);

	//Get the corners from the object
	obj_corners[0] = cvPoint(0,0);
	obj_corners[1] = cvPoint( m_MarkerObject.cols, 0 );
	obj_corners[2] = cvPoint( m_MarkerObject.cols, m_MarkerObject.rows );
	obj_corners[3] = cvPoint( 0, m_MarkerObject.rows );

	key = 'a';
	framecount = 0;
}

Mat SurfCameraFeed::CalculateHomography() {
}
/**
Analyses each frame of camera feed.
@param frame - a matrix representing the camera frame
*/
void SurfCameraFeed::AnalyseFrame(Mat frame) {

	Mat des_image, img_matches;
	std::vector<KeyPoint> kp_image; //an array of keypoints in the image for this frame
	std::vector<vector<DMatch > > matches;
	std::vector<DMatch > good_matches;
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	std::vector<Point2f> scene_corners(4);
	Mat H;
	Mat greyScaleImage;

	//convert the camera frame to a grey scale image and save it in the image variable
	cvtColor(frame, greyScaleImage, CV_RGB2GRAY);

	//use Surf detector to detect key points
	m_Detector.detect( greyScaleImage, kp_image);

	//compute a description object (feature vectors)
	m_Extractor.compute( greyScaleImage, kp_image, des_image);

	//uses flann matcher to find similar points in computed description objects
	m_Matcher.knnMatch(m_DescriptionObject, des_image, matches, 2);

	/*Draw only "good" matches (i.e. whose distance is less than 2*min_dist, or a small arbitary value ( 0.02 )
	in the event that min_dist is very small) - radiusMatch can also be used here.
	*/
	for(int i = 0; i < min(des_image.rows-1,(int) matches.size()); i++)
	{
		if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
		{
			good_matches.push_back(matches[i][0]);
		}
	}

	//Draw only "good" matches
	drawMatches( m_MarkerObject, m_KeyPointObject, greyScaleImage, kp_image, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	if (good_matches.size() >= 4)
	{
		for( int i = 0; i < good_matches.size(); i++ )
		{
			//Get the keypoints from the good matches
			obj.push_back( m_KeyPointObject[ good_matches[i].queryIdx ].pt );
			scene.push_back( kp_image[ good_matches[i].trainIdx ].pt );
		}

		H = findHomography( obj, scene, CV_RANSAC );

		perspectiveTransform( obj_corners, scene_corners, H);

		//Draw lines between the corners (the mapped object in the scene image )
		line( img_matches, scene_corners[0] + Point2f( m_MarkerObject.cols, 0), scene_corners[1] + Point2f( m_MarkerObject.cols, 0), Scalar(0, 255, 0), 4 );
		line( img_matches, scene_corners[1] + Point2f( m_MarkerObject.cols, 0), scene_corners[2] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
		line( img_matches, scene_corners[2] + Point2f( m_MarkerObject.cols, 0), scene_corners[3] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
		line( img_matches, scene_corners[3] + Point2f( m_MarkerObject.cols, 0), scene_corners[0] + Point2f( m_MarkerObject.cols, 0), Scalar( 0, 255, 0), 4 );
	}
	//Show detected matches
	imshow( "Good Matches", img_matches );

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