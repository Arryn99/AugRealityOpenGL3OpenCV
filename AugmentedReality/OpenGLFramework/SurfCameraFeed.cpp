#include "SurfCameraFeed.h"

SurfCameraFeed::SurfCameraFeed() {

	m_Cap = new VideoCapture(0);
	m_Key = 'a';
	m_Framecount = 0;
	m_MinHessian = 500;

	m_objCorners = std::vector<Point2f>(4); 
	m_Detector = SurfFeatureDetector (m_MinHessian);
	object = imread( "Photo2.jpg", CV_LOAD_IMAGE_GRAYSCALE );

	m_SceneCorners = std::vector<Point2f>(4);
}

SurfCameraFeed::~SurfCameraFeed() {
}

int SurfCameraFeed::OpenCVCameraInit() {
	if( !object.data ) {
		std::cout<< "Error reading object " << std::endl;
		return -1;
	}

	m_Detector.detect( object, m_kp_object );
	m_Extractor.compute( object, m_kp_object, des_object );
	namedWindow("Good Matches");

	//Get the corners from the object
	m_objCorners[0] = cvPoint(0,0);
	m_objCorners[1] = cvPoint( object.cols, 0 );
	m_objCorners[2] = cvPoint( object.cols, object.rows );
	m_objCorners[3] = cvPoint( 0, object.rows );

	while (m_Key != 27) {
		OpenCVCameraFrame();
	}
}

int SurfCameraFeed::OpenCVCameraFrame() {

	*m_Cap >> frame;

	if (m_Framecount < 5) {
		m_Framecount++;
		return 0;
	}
	
	cvtColor(frame, image, CV_RGB2GRAY);
	m_Detector.detect( image, m_kp_image );
	m_Extractor.compute( image, m_kp_image, des_image );
	m_Matcher.knnMatch(des_object, des_image, m_Matches, 2);

	//THIS LOOP IS SENSITIVE TO SEGFAULTS
	for(int i = 0; i < min(des_image.rows-1,(int) m_Matches.size()); i++) {
		if((m_Matches[i][0].distance < 0.6*(m_Matches[i][1].distance)) && ((int) m_Matches[i].size()<=2 && (int) m_Matches[i].size()>0)) {
			m_GoodMatches.push_back(m_Matches[i][0]);
		}
	}

	//Draw only "good" matches
	drawMatches( object, m_kp_object, image, m_kp_image, m_GoodMatches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	if (m_GoodMatches.size() >= 4) {
		for( int i = 0; i < m_GoodMatches.size(); i++ ) {
			//Get the keypoints from the good matches
			m_Obj.push_back( m_kp_object[ m_GoodMatches[i].queryIdx ].pt );
			m_Scene.push_back( m_kp_image[ m_GoodMatches[i].trainIdx ].pt );
		}

		H = findHomography( m_Obj, m_Scene, CV_RANSAC );

		perspectiveTransform( m_objCorners, m_SceneCorners, H);

		//Draw lines between the corners (the mapped object in the scene image )
		line( img_matches, m_SceneCorners[0] + Point2f( object.cols, 0), m_SceneCorners[1] + Point2f( object.cols, 0), Scalar(0, 255, 0), 4 );
		line( img_matches, m_SceneCorners[1] + Point2f( object.cols, 0), m_SceneCorners[2] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
		line( img_matches, m_SceneCorners[2] + Point2f( object.cols, 0), m_SceneCorners[3] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
		line( img_matches, m_SceneCorners[3] + Point2f( object.cols, 0), m_SceneCorners[0] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
	}

	//Show detected matches
	imshow( "Good Matches", img_matches );

	m_Key = waitKey(1);

	return 0;
}