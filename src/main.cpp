#include <opencv2/opencv.hpp>
#include <ostream>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture cam(0);
	if (!cam.isOpened())
		cout << "Camera was not found";

	Mat webcam;
	Mat grey;
	Mat grey_hsv;
	Mat edge;
	Mat bgdModel, fgdModel;

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<vector<Point>> hull(1);
	vector< Rect_<int> > facesDetected;

	//add profile-face haarcascade
	//CascadeClassifier frontalFace = CascadeClassifier("haarcascades/frontal-face-extended.xml");

	//make dependencies on lightning
	//this values works for medium light in the room
	int minH = 0,
		minS = 0, 
		minV = 100,	//the lowest value the more noise (about 100 is ok)
		maxH = 255,	// > 180 
		maxS = 125, 
		maxV = 255;

	//Trackbar for adjustation HSV
	namedWindow("trackbar");
	createTrackbar("MinH", "trackbar", &minH, 180);
	createTrackbar("MinS", "trackbar", &minS, 255);
	createTrackbar("MinV", "trackbar", &minV, 180);
	createTrackbar("MaxH", "trackbar", &maxH, 255);
	createTrackbar("MaxS", "trackbar", &maxS, 255);
	createTrackbar("MaxV", "trackbar", &maxV, 255);

	while (1) 
	{
		cam.read(webcam);

		//colors and blur changes

		//for face detection
		cvtColor(webcam, grey, COLOR_RGB2GRAY);
		//for hand detection
		cvtColor(webcam, grey_hsv, CV_BGR2HSV);
		inRange(grey_hsv, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), grey_hsv);
		medianBlur(grey_hsv, grey_hsv, 5);

		/*

		//detecting face with haar cascades is not so efficient
		frontalFace.detectMultiScale(grey, facesDetected);

		for (int i = 0; i < facesDetected.size(); i++)
		{
			Rect face_i = facesDetected[i];
			Mat face = grey(face_i);
			rectangle(webcam, face_i, CV_RGB(0, 255, 0), 1);

			//grabCut evokes crash
			//grabCut(grey, grey_hsv, face_i, bgdModel, fgdModel, 1);
		}

		*/

		//detecting contours
		findContours(grey_hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		size_t lrgContour = 0;

		for (size_t i = 1; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) > contourArea(contours[lrgContour]))
				lrgContour = i;
		}
		drawContours(webcam, contours, lrgContour, Scalar(0, 0, 255), 1);

		//convex hull
		if (!contours.empty())
		{
			convexHull(Mat(contours[lrgContour]), hull[0], false);
			drawContours(webcam, hull, 0, Scalar(255, 0, 0), 2);
		}

		imshow("trackbar", grey_hsv);
		imshow("ORIGINAL", webcam);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}