#include "init.h"
int init(cv::VideoCapture& cap, cv::Mat& imgLines) {
	// if not success, exit program
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

	return 0;
}

std::string getLine() {
	std::string result;
	getline(std::cin, result);
	return result;
}