// https://www.opencv-srf.com/2010/09/object-detection-using-color-seperation.html
// https://www.pyimagesearch.com/2015/02/09/removing-contours-image-using-python-opencv/
// added comment2

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <time.h>
#include <bitset>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <Windows.h>
#include "init.h"
#include "colorhsvtrackbar.h"
#include "thresholding.h"
#include "detectcircles.h"
#include "getangle.h"

#include"SerialPort.h"

using namespace cv;
using namespace std;

RNG rng(12345);


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

	int iLastX_sent = -1;
	int iLastY_sent = -1;
	
	int last_anglex = 0;
	int anglex = 0;
	int ydist = 2;

	string sx_last, sy_last;
	string sx, sy, rout;

	Mat imgHSV  ;    // HSV convert
	Mat imgLines;    // empty image + tracking lines fro colored object
	Mat imgGr   ;    // grayscale image
	Mat imgdraw ;
	VideoCapture cap(0);

	//Define names of the window
	String win_control = "Control";
	String win_orig = "Original";

	// Create a window with above names
	namedWindow(win_control, WINDOW_AUTOSIZE);
	namedWindow(win_orig, WINDOW_AUTOSIZE);

	// arduino
	//char port[] = "\\\\.\\COM3";
	//SerialPort arduino(port);

	char port[] = "\\\\.\\COM3";
	SerialPort arduino(port);

	// init --------------------------------------------------------------------------------
	init(cap, imgLines);

	if (arduino.isConnected()) {
		std::cout << "Connection made" << std::endl;
	}
	else {
		std::cout << "Error in port name" << std::endl;
	}

	// setup trackbar ----------------------------------------------------------------------
	colorhsvtrackbar(win_control,iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);

	// start frame -------------------------------------------------------------------------
	unsigned int fcnt = 0; // frame counter: used to send data to arduino at every nth frame
	while (true) {
		
		// frame counter

		// get video
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video 
		//cout << "height: " << cap.get(CAP_PROP_FRAME_HEIGHT) << " width: " << cap.get(CAP_PROP_FRAME_WIDTH) << endl;

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
		//cvtColor(imgOriginal, imgGr, COLOR_BGR2GRAY); //Convert the captured frame from BGR to grayscale

		// create image with thresholding method v1
		Mat imgThres = thresholdingv1(imgHSV, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);
		//Mat imgThres = thresholdingv2(imgGr);

		// object detection
		detectCirclesv2(imgOriginal, imgThres, imgdraw, imgLines, iLastX, iLastY);

		imgOriginal = imgOriginal + imgLines;

		int frameh = cap.get(CAP_PROP_FRAME_WIDTH) / 2;
		int framev = cap.get(CAP_PROP_FRAME_HEIGHT) / 2;
		drawMarker(imgOriginal,Point(framev, frameh),Scalar(0,0,255),0,10,1,8); // draw marker in middle of image

		sx = to_string(getangle(frameh, iLastX, ydist));
		sy = to_string(getangle(framev, iLastY, ydist));

		if (fcnt == 0) {
			sx_last = sx;
			sy_last = sy;
		}

		// show video with tracking line
		imshow("Original", imgOriginal); //show the original image
		// show thresholded image
		imshow("Thresholded Image", imgThres); //show the thresholded image
		// show grayscale image
		imshow("drawn Image", imgdraw); //show the thresholded image

		//-------- Send the position to Arduino --------

		// exit -------------------------------------------------------------------------------
		int comm = waitKey(10);

		/* data setup for function recvWithStartEndMarkers()*/
		rout = "<" + sx + "." + sy + ">";
		int dtlen = rout.size();
		char* rsen = new char[dtlen];
		char* rrec = new char[dtlen];
		copy(rout.begin(), rout.end(), rsen);
		

		/* data setup for function recvWithEndMarker2()
		rout = sx + "." + sy ;
		int dtlen = rout.size() +1;
		char* rsen = new char[dtlen];
		copy(rout.begin(), rout.end(), rsen);
		rsen[rout.size()] = '\n';
		*/
		

		if (/*fcnt % 10 == 0 &&*/ rout != "<1.1>") {
			WriteFile(arduino.getSerial(), rsen, dtlen, NULL, NULL);
			//arduino.ReadData( rrec, dtlen);
			//std::cout << rsen << endl;
			for (int i = 0; i < dtlen; i++) { std::cout << rsen[i] << ' '; } std::cout << std::endl;
		}

		delete[] rsen;
		delete[] rrec;
		fcnt++;

		// exit -------------------------------------------------------------------------------
		if (comm == 27) {
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}

	destroyAllWindows(); //Destroy all opened windows

	return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu