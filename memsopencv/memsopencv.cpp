//https://www.opencv-srf.com/2010/09/object-detection-using-color-seperation.html
// added comment2

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

void init(VideoCapture& , Mat& );
void colorhsvtrackbar(string& , int& , int& , int& , int& , int& , int& );
Mat  thresholdingv1(Mat&, int&, int&, int&, int&, int&, int&);
void calcMments(Mat&, Mat&, int&, int&);
void detectCirclesv1(Mat& , Mat& );


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

	Mat imgHSV;   // HSV convert
	Mat imgLines; // empty image + tracking lines fro colored object
	Mat imgGr;    // grayscale image
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
		cvtColor(imgOriginal, imgGr, COLOR_BGR2GRAY); //Convert the captured frame from BGR to HSV

		// create image with thresholding method v1
		Mat imgThresholdedv1 = thresholdingv1(imgHSV, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);

		// calculate moments and add tracking line
		calcMments(imgThresholdedv1, imgLines, iLastX, iLastY);
		imgOriginal = imgOriginal + imgLines;

		// prepare for object detection - create grayscale image + apply blurring
		cvtColor(imgOriginal, imgGr, COLOR_BGR2GRAY); //Convert the captured frame from BGR to HSV
		GaussianBlur(imgGr, imgGr, cv::Size(9, 9), 2, 2);
		
		// detect circles
		detectCirclesv1(imgOriginal, imgGr);

		// show video with tracking line
		imshow("Original", imgOriginal); //show the original image
		// show thresholded image
		imshow("Thresholded Image", imgThresholdedv1); //show the thresholded image


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

void calcMments(Mat& imgThresholded, Mat& imgLines, int& iLastX, int& iLastY) {

	//Calculate the moments of the thresholded image
	Moments oMoments = moments(imgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
	if (dArea > 10000)
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu