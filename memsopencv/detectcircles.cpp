#include <vector>
#include "detectcircles.h"

void detectCirclesv1(cv::Mat& imgOriginal, cv::Mat& imgGr) {

	// prepare for object detection - create grayscale image + apply blurring
	cv::GaussianBlur(imgGr, imgGr, cv::Size(9, 9), 2, 2);

	std::vector<cv::Vec3f> circles2;
	cv::HoughCircles(
		imgGr, circles2, cv::HOUGH_GRADIENT,
		2,				// accumulator resolution (size of the image / 2)
		5,				// image_empty_gr.rows / 16,  // change this value to detect circles with different distances to each other
		100,			// canny high threshold
		100,			// minimum number of votes
		0, 80			// change the last two parameters (min_radius & max_radius) to detect larger circles
	);

	for (size_t i = 0; i < circles2.size(); i++)
	{
		cv::Vec3i c = circles2[i];
		cv::Point center = cv::Point(c[0], c[1]);
		int radius = c[2];
		circle(imgOriginal, center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
	}
}

void detectCirclesv2(cv::Mat& imgorig, cv::Mat& imgthres, cv::Mat& imgdraw, cv::Mat& imglines, int& iLastX, int& iLastY) {
	std::vector<std::vector<cv::Point>> contours;
	//vector<vector<Point>> applypoly;
	std::vector<std::vector<cv::Point>> hull;
	std::vector<std::vector<cv::Point>> circles;
	std::vector<cv::Vec4i> hierarchy;
	//vector<Vec3i> poly;

	cv::findContours(imgthres, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
	//applypoly.resize(contours.size());
	hull.resize(contours.size());

	std::vector<cv::Point> approxhull;
	// approximates each contour to polygon
	for (std::vector<cv::Point>cont : contours) {

		// create contours
		//approxPolyDP(Mat(cont), approx, arcLength(cont, true) * 0.02, true);
		cv::convexHull(cv::Mat(cont), approxhull);
		cv::approxPolyDP(approxhull, approxhull, cv::arcLength(approxhull, true) * 0.01, true);

		// keep only circles
		if (approxhull.size() > 10
			//&& isContourConvex(approx)
			&& fabs(cv::contourArea(approxhull)) > 1000
			) {
			circles.push_back(approxhull);
		}
	}

	// get size of biggest circle
	double tmpsize = 0;
	int tmpelement = 0;
	for (size_t i = 0; i < circles.size(); i++) {
		if (tmpsize < fabs(cv::contourArea(circles[i]))) {
			tmpsize = fabs(cv::contourArea(circles[i]));
		}
	}

	// keep biggest circle
	auto it = circles.begin();
	while (it != circles.end()) {
		if (fabs(cv::contourArea(*it)) != tmpsize) {
			it = circles.erase(it);
		}
		else {
			++it;
		}
	}

	imgdraw = cv::Mat::zeros(imgthres.size(), CV_8UC1);
	cv::drawContours(imgdraw, circles, 0, cv::Scalar(255, 255, 255), -2, cv::LINE_8, hierarchy, 0);
	//circle(imgOriginal, center, radius, Scalar(255, 0, 255), 3, LINE_AA);

	//Calculate the moments of the thresholded image
	cv::Moments moments = cv::moments(imgdraw);

	double dm01 = moments.m01;
	double dm10 = moments.m10;
	double darea = moments.m00;

	//calculate the position of the ball
	int posX = dm10 / darea;
	int posY = dm01 / darea;

	if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
	{
		//Draw a line from the previous point to the current point
		line(imglines, cv::Point(posX, posY), cv::Point(iLastX, iLastY), cv::Scalar(0, 255, 0), 1);
	}

	iLastX = posX;
	iLastY = posY;

	//std::cout << iLastX << std::endl;
}