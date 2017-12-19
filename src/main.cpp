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
	Mat edge;
	Mat threshold_cam;
	Mat gauss_blur;

	while (1) 
	{
		cam.read(webcam);

		cvtColor(webcam, grey, COLOR_RGB2GRAY);
		Canny(grey, edge, 70, 80, 3);
		threshold(grey, threshold_cam, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
		GaussianBlur(grey, gauss_blur, Size(19, 19), 0.0, 0);

		imshow("BLUR", gauss_blur);
		imshow("THRESHOLD", threshold_cam);
		imshow("EDGES", edge);
		imshow("GREY", grey);
		imshow("ORIGINAL", webcam);

		if (waitKey(30) >= 0) break;
	}

	return 0;
}