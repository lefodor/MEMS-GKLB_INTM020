#include "colorhsvtrackbar.h"

void colorhsvtrackbar(std::string& winname, int& iLowH, int& iHighH, int& iLowS, int& iHighS, int& iLowV, int& iHighV) {

	//Create trackbars in "Control" window
	cv::createTrackbar("LowH", winname, &iLowH, 179); //Hue (0 - 179)
	cv::createTrackbar("HighH", winname, &iHighH, 179);

	cv::createTrackbar("LowS", winname, &iLowS, 255); //Saturation (0 - 255)
	cv::createTrackbar("HighS", winname, &iHighS, 255);

	cv::createTrackbar("LowV", winname, &iLowV, 255);//Value (0 - 255)
	cv::createTrackbar("HighV", winname, &iHighV, 255);
}