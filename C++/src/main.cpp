#include <opencv2/opencv.hpp>
#include <ostream>

using namespace cv;
using namespace std;

/*
	Made without background subtraction
	is too dependent on lightning level.

	In fact to make it work right, every
	time starting the program, HSV values'
	range would have to be adjusted depending
	on the current lightning.

	Face recognition using haar cascades
	is slow and imprecise as well.
*/

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
	vector<int> hullIndexes;
	vector<Vec4i> convexityDefects;

	//CascadeClassifier frontalFace = CascadeClassifier("haarcascades/frontal-face-extended.xml");

	//this values works for well lighted first plan (hand) only
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

		//cvtColor(webcam, grey, COLOR_RGB2GRAY);
		cvtColor(webcam, grey_hsv, CV_BGR2HSV);
		inRange(grey_hsv, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), grey_hsv);
		medianBlur(grey_hsv, grey_hsv, 5);

		/*
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

		//Objects detetced in pov
		//cout <<contours.size()<<endl;

		for (size_t i = 1; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) > contourArea(contours[lrgContour]))
				lrgContour = i;
		}

		//contours as edges in red
		drawContours(webcam, contours, lrgContour, Scalar(0, 0, 255), 1);

		//convex hull
		if (!contours.empty())
		{
			convexHull(Mat(contours[lrgContour]), hull[0], false);
			drawContours(webcam, hull, 0, Scalar(255, 0, 0), 2);

			if (hull[0].size() > 2)
			{
				convexHull(Mat(contours[lrgContour]), hullIndexes, true);
				cv::convexityDefects(Mat(contours[lrgContour]), hullIndexes, convexityDefects);

				Rect boundingBox = boundingRect(hull[0]);
				rectangle(webcam, boundingBox, Scalar(0, 0, 255), 1);

				//calculating center of mass for the object
				Point centerOfMass = Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
				//centerOfMass point in black
				circle(webcam, centerOfMass, 4, Scalar(0, 0, 0), CV_FILLED);

				//lines conecting convex and defect points
				for (size_t i = 0; i < convexityDefects.size(); i++)
				{
					Point p1 = contours[lrgContour][convexityDefects[i][0]];
					Point p2 = contours[lrgContour][convexityDefects[i][1]];
					Point p3 = contours[lrgContour][convexityDefects[i][2]];

					line(webcam, p1, p3, Scalar(0, 255, 0), 1);
					line(webcam, p3, p2, Scalar(0, 255, 0), 1);
				}
			}
		}

		imshow("trackbar", grey_hsv);
		imshow("ORIGINAL", webcam);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}