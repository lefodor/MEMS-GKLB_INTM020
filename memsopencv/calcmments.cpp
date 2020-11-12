#include "calcmments.h"

void calcMments(cv::Mat& imgThresholded, cv::Mat& imgLines, int& iLastX, int& iLastY) {

	//Calculate the moments of the thresholded image
	cv::Moments oMoments = moments(imgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
	if (dArea > 100000)
	{
		//calculate the position of the ball
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
		{
			//Draw a line from the previous point to the current point
			line(imgLines, cv::Point(posX, posY), cv::Point(iLastX, iLastY), cv::Scalar(0, 255, 0), 1);
		}

		iLastX = posX;
		iLastY = posY;

	}
}