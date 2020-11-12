#include "thresholding.h"
cv::Mat thresholdingv1(cv::Mat& imgHSV, int& iLowH, int& iHighH, int& iLowS, int& iHighS, int& iLowV, int& iHighV) {
	cv::Mat imgThresholded;

	inRange(imgHSV, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

	//morphological opening (removes small objects from the foreground
	//erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	//dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (removes small holes from the foreground)
	//dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	//erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	return imgThresholded;
}

cv::Mat thresholdingv2(cv::Mat& imgGr) {
	cv::Mat imgthr;

	cv::threshold(imgGr, imgthr, 100, 200, cv::THRESH_BINARY); //Threshold the image

	return imgthr;
}