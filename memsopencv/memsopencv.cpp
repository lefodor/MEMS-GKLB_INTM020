// https://www.opencv-srf.com/2010/09/object-detection-using-color-seperation.html
// https://www.pyimagesearch.com/2015/02/09/removing-contours-image-using-python-opencv/
// added comment2

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

RNG rng(12345);

void init(VideoCapture& , Mat& );
void colorhsvtrackbar(string& , int& , int& , int& , int& , int& , int& );
Mat  thresholdingv1(Mat&, int&, int&, int&, int&, int&, int&);
Mat  thresholdingv2(Mat&);
void calcMments(Mat&, Mat&, int&, int&);
void detectCirclesv1(Mat& , Mat& );
void detectCirclesv2(Mat&,Mat&,Mat&, Mat&, int&, int&);
//void detectCirclesv2(Mat&, Mat&); // approxPolyDP
//void detectCirclesv3(Mat&, Mat&); // min enclosing circle


int main(int argc, char* argv[])
{
	// set up global vars ------------------------------------------------------------------
	int iLowH = 22;
	int iHighH = 38;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;

	int iLastX = -1;
	int iLastY = -1;

	Mat imgHSV  ;    // HSV convert
	Mat imgLines;    // empty image + tracking lines fro colored object
	Mat imgGr   ;    // grayscale image
	Mat imgdraw ;
	VideoCapture cap(0);

	//Define names of the window
	String win_control = "Control";
	String win_orig = "Original";

	// Create a window with above names
	namedWindow(win_control, WINDOW_NORMAL);
	namedWindow(win_orig, WINDOW_NORMAL);

	// init --------------------------------------------------------------------------------
	init(cap, imgLines);

	// setup trackbar ----------------------------------------------------------------------
	colorhsvtrackbar(win_control,iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);

	// start frame -------------------------------------------------------------------------
	while (true) {

		// get video
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video 

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			//cout << "Found the end of the video" << endl;
			cout << "Video camera is disconnected" << endl;
			break;
		}

		// create HSV image
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		// create Grayscale image
		cvtColor(imgOriginal, imgGr, COLOR_BGR2GRAY); //Convert the captured frame from BGR to grayscale

		// create image with thresholding method v1
		Mat imgThres = thresholdingv1(imgHSV, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);
		//Mat imgThres = thresholdingv2(imgGr);

		// object detection
		detectCirclesv2(imgOriginal, imgThres, imgdraw, imgLines, iLastX, iLastY);

		// calculate moments and add tracking line
		//calcMments(imgdraw, imgLines, iLastX, iLastY);

		// prepare for object detection - create grayscale image + apply blurring
		// GaussianBlur(imgGr, imgGr, cv::Size(9, 9), 2, 2);
		
		// detect circles
		//detectCirclesv1(imgOriginal, imgGr);
		//detectCirclesv2(imgOriginal, imgThres, imgdraw);

		imgOriginal = imgOriginal + imgLines;
		// show video with tracking line
		imshow("Original", imgOriginal); //show the original image
		// show thresholded image
		imshow("Thresholded Image", imgThres); //show the thresholded image
		// show grayscale image
		//imshow("grayscale Image", imgGr); //show the thresholded image
		// show grayscale image
		imshow("drawn Image", imgdraw); //show the thresholded image


		// exit -------------------------------------------------------------------------------
		int comm = waitKey(10);
		if (comm == 27) {
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}
	destroyAllWindows(); //Destroy all opened windows

	return 0;

}

void init(VideoCapture& cap, Mat& imgLines) {
	// if not success, exit program
	if (cap.isOpened() == false)
	{
		//cout << "Cannot open the video file" << endl;
		cout << "Cannot open the video cam " << endl;
		cin.get(); //wait for any key press
		//return -1;
	}

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

}

void colorhsvtrackbar(string& winname, int& iLowH, int& iHighH, int& iLowS, int& iHighS, int& iLowV, int& iHighV) {

	//Create trackbars in "Control" window
	createTrackbar("LowH", winname, &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", winname, &iHighH, 179);

	createTrackbar("LowS", winname, &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", winname, &iHighS, 255);

	createTrackbar("LowV", winname, &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", winname, &iHighV, 255);
}


Mat thresholdingv1(Mat& imgHSV, int& iLowH, int& iHighH, int& iLowS, int& iHighS, int& iLowV, int& iHighV) {
	Mat imgThresholded;

	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

	//morphological opening (removes small objects from the foreground
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (removes small holes from the foreground)
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	return imgThresholded;
}

Mat thresholdingv2(Mat& imgGr) {
	Mat imgthr;

	threshold(imgGr, imgthr, 100, 200, THRESH_BINARY); //Threshold the image

	return imgthr;
}

void calcMments(Mat& imgThresholded, Mat& imgLines, int& iLastX, int& iLastY) {

	//Calculate the moments of the thresholded image
	Moments oMoments = moments(imgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
	if (dArea > 100000)
	{
		//calculate the position of the ball
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		//xcoords.push_back(posX);
		//ycoords.push_back(posY);

		if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
		{
			//Draw a line from the previous point to the current point
			line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 255, 0), 1);
		}

		iLastX = posX;
		iLastY = posY;

	}
}

void detectCirclesv1(Mat& imgOriginal, Mat& imgGr) {
	
	// prepare for object detection - create grayscale image + apply blurring
	GaussianBlur(imgGr, imgGr, cv::Size(9, 9), 2, 2);

	vector<Vec3f> circles2;
	HoughCircles(
		imgGr, circles2, HOUGH_GRADIENT,
		2,				// accumulator resolution (size of the image / 2)
		5,				// image_empty_gr.rows / 16,  // change this value to detect circles with different distances to each other
		100,			// canny high threshold
		100,			// minimum number of votes
		0, 80			// change the last two parameters (min_radius & max_radius) to detect larger circles
	);

	//cout << circles.size() << endl;

	for (size_t i = 0; i < circles2.size(); i++)
	{
		Vec3i c = circles2[i];
		Point center = Point(c[0], c[1]);
		//cout << c[0] << " " << c[1] << endl;
		// circle center
		circle(imgOriginal, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
		// circle outline
		int radius = c[2];
		circle(imgOriginal, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
	}
}

void detectCirclesv2(Mat& imgorig, Mat& imgthres, Mat& imgdraw, Mat& imglines, int& iLastX, int& iLastY) {
	vector<vector<Point>> contours;
	vector<vector<Point>> applypoly;
	//vector<vector<Point>> hull;
	vector<vector<Point>> circles;
	vector<Vec4i> hierarchy;
	//vector<Vec3i> poly;

	findContours(imgthres, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	applypoly.resize(contours.size());
	//hull.resize(contours.size());

	vector<Point> approx;
	// approximates each contour to polygon
	for(vector<Point>cont : contours){
		
		// create contours
		approxPolyDP(Mat(cont), approx, arcLength(cont, true) * 0.02, true);
		//convexHull(contours[i], hull[i]);
		
		// keep only circles
		if (approx.size() > 10
			//&& isContourConvex(approx)
			&& fabs(contourArea(approx)) > 1000
			) {
			circles.push_back(approx);
		}
	}

	// get size of biggest circle
	double tmpsize = 0;
	int tmpelement = 0;
	for (size_t i = 0; i < circles.size(); i++) {
		if (tmpsize < fabs(contourArea(circles[i]))) {
			tmpsize = fabs(contourArea(circles[i]));
		}
	}

	// keep biggest circle
	auto it = circles.begin();
	while (it != circles.end()) {
		if (fabs(contourArea(*it)) != tmpsize) {
			it = circles.erase(it);
		}
		else {
			++it;
		}
	}

	imgdraw = Mat::zeros(imgthres.size(), CV_8UC1);
	drawContours(imgdraw, circles, 0, Scalar(255, 255, 255), -2, LINE_8, hierarchy, 0);

	//Calculate the moments of the thresholded image
	Moments oMoments = moments(imgdraw);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	//calculate the position of the ball
	int posX = dM10 / dArea;
	int posY = dM01 / dArea;

	if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
	{
		//Draw a line from the previous point to the current point
		line(imglines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 255, 0), 1);
	}

	iLastX = posX;
	iLastY = posY;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu