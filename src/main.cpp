#include <opencv2/opencv.hpp>
#include <ostream>

using namespace cv;
using namespace std;


//TODO:	face masking

int main()
{
	VideoCapture cam(0);
	if (!cam.isOpened())
		cout << "Camera was not found";

	Mat webcam;
	Mat grey_hsv;
	Mat edge;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

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

		cvtColor(webcam, grey_hsv, CV_BGR2HSV);
		inRange(grey_hsv, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), grey_hsv);
		medianBlur(grey_hsv, grey_hsv, 5);
		//Detecting contours
		findContours(grey_hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		size_t lrgContour = 0;

		for (size_t i = 1; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) > contourArea(contours[lrgContour]))
				lrgContour = i;
		}

		drawContours(webcam, contours, lrgContour, Scalar(0, 0, 255), 1);

		imshow("trackbar", grey_hsv);
		imshow("ORIGINAL", webcam);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}