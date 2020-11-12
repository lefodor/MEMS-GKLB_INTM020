#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
cv::Mat  thresholdingv1(cv::Mat&, int&, int&, int&, int&, int&, int&);
cv::Mat  thresholdingv2(cv::Mat&);