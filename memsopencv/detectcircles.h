#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
void detectCirclesv1(cv::Mat&, cv::Mat& );
void detectCirclesv2(cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, int&, int&);