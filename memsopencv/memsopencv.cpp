// https://www.opencv-srf.com/2010/09/object-detection-using-color-seperation.html
// https://www.pyimagesearch.com/2015/02/09/removing-contours-image-using-python-opencv/
// added comment2

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <time.h>
#include <bitset>
#include <Windows.h>

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "colorhsvtrackbar.h"
#include "thresholding.h"
#include "detectcircles.h"
#include "getangle.h"
#include "SerialPort.h"

#define MOVING_AVERAGE 5

//using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{
	// set up global vars ------------------------------------------------------------------
	// starting values for trackbar
	int iLowH = 22;
	int iHighH = 38;

	int iLowS = 80;
	int iHighS = 150;

	int iLowV = 60;
	int iHighV = 255;

	int iLastX = -1;
	int iLastY = -1;
	
	// camera distance to object
	const int ydist = 20;

	// values for moving average - used for smoothing results
	int* movavgx = new int[MOVING_AVERAGE];
	int* movavgy = new int[MOVING_AVERAGE];
	int sumx=-1, sumy=-1;
	int sumx_last = -1, sumy_last = -1;

	// coordinates
	string sx, sy, rout;

	// arduino communication
	char port[] = "\\\\.\\COM3";
	SerialPort arduino(port);

	// image processing variables
	cv::Mat imgHSV  ;      // HSV convert
	cv::Mat imgLines;      // empty image + tracking lines from colored object
	//cv::Mat imgGr   ;    // grayscale image
	cv::Mat imgdraw ;
	cv::VideoCapture cap(0);

	//Define names of the window
	cv::String win_control = "Control";
	cv::String win_orig = "Original";

	// Create a window with above names
	cv::namedWindow(win_control, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(win_orig, cv::WINDOW_AUTOSIZE);

	// init --------------------------------------------------------------------------------

	if (cap.isOpened() == false)
	{
		//cout << "Cannot open the video file" << endl;
		std::cout << "Cannot open the video cam " << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	//Capture a temporary image from the camera
	cv::Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	imgLines = cv::Mat::zeros(imgTmp.size(), CV_8UC3);

	if (arduino.isConnected()) {
		std::cout << "Connection made" << std::endl;
	}
	else {
		std::cout << "Error in port name" << std::endl;
	}

	// setup trackbar - used for manual calibration ----------------------------------------
	colorhsvtrackbar(win_control,iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);

	// start frame -------------------------------------------------------------------------
	unsigned int fcnt = 0; // frame counter: used to send data to arduino at every nth frame
	while (true) {

		// get video
		cv::Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video 

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			cout << "Video camera is disconnected" << endl;
			break;
		}

		// create HSV image
		cvtColor(imgOriginal, imgHSV, cv::COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		// create Grayscale image
		// cvtColor(imgOriginal, imgGr, COLOR_BGR2GRAY); //Convert the captured frame from BGR to grayscale

		// create image with thresholding method v1
		cv::Mat imgThres = thresholdingv1(imgHSV, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);

		// object detection
		detectCirclesv2(imgOriginal, imgThres, imgdraw, imgLines, iLastX, iLastY);

		imgOriginal = imgOriginal + imgLines;

		int frameh = cap.get(cv::CAP_PROP_FRAME_WIDTH) / 2;
		int framev = cap.get(cv::CAP_PROP_FRAME_HEIGHT) / 2;
		//drawMarker(imgOriginal,cv::Point(framev, frameh), cv::Scalar(0,0,255),0,10,1,8); // draw marker in middle of image

		// moving average of angles
			// data
		sumx_last = sumx;
		sumy_last = sumy;
		sumx = 0;
		sumy = 0;
			if (fcnt == 0) {
				for (int i = 0; i < MOVING_AVERAGE; i++) {
					*(movavgx + i) = frameh;
					*(movavgy + i) = framev;
				}
				sumx_last = frameh;
				sumy_last = framev;
			}
			else {
				// shift values
				if (iLastX > 0 && iLastY > 0) {
					for (int i = MOVING_AVERAGE - 2; i >= 0; i--) {
						movavgx[i + 1] = movavgx[i];
						movavgy[i + 1] = movavgy[i];
					}

					movavgx[0] = iLastX;
					movavgy[0] = iLastY;
				}
			}

			// moving average
			for (int i = 0; i < MOVING_AVERAGE; i++) {
				sumx += movavgx[i];
				sumy += movavgy[i];
			}
			sumx = sumx / MOVING_AVERAGE;
			sumy = sumy / MOVING_AVERAGE;
		
		sx = to_string(getangle(frameh, sumx, ydist,true));
		sy = to_string(getangle(framev, sumy, ydist,false));

		// show video with tracking line
		imshow("Original", imgOriginal); //show the original image
		// show thresholded image
		imshow("Thresholded Image", imgThres); //show the thresholded image
		// show grayscale image
		imshow("drawn Image", imgdraw); //show the thresholded image

		//-------- Send the position to Arduino -----------------------------------------------

		// exit -------------------------------------------------------------------------------
		int comm = cv::waitKey(10);

		/* data setup for function recvWithStartEndMarkers()*/
		rout = "<" + sy + "." + sx + ">";
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

		if ( rout != "<1.1>" ) {
			WriteFile(arduino.getSerial(), rsen, dtlen, NULL, NULL);
			// std::cout << rout << endl;
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

	delete[] movavgx;
	delete[] movavgy;

	cv::destroyAllWindows(); //Destroy all opened windows

	return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu