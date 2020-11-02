//https://www.opencv-srf.com/2010/09/object-detection-using-color-seperation.html
// added comment2

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	//open the video file for reading
	//VideoCapture cap("C:/Users/leven/opencv/HerdOfDeerRunningOnSnowyRoad.mp4");
	VideoCapture cap(0);
	//VideoCapture cap("http://192.168.0.102:4747/video?x.mjpeg");

	// if not success, exit program
	if (cap.isOpened() == false)
	{
		//cout << "Cannot open the video file" << endl;
		cout << "Cannot open the video cam " << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	//Define names of the window
	String window_name_of_original_video = "Control";

	// Create a window with above names
	namedWindow(window_name_of_original_video, WINDOW_NORMAL);

	int iLowH = 22;
	int iHighH = 38;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	int iLastX = -1;
	int iLastY = -1;

	vector<int> xcoords;
	vector<int> ycoords;

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat imgLinesFade = Mat::zeros(imgTmp.size(), CV_8UC3);

	while (true)
	{
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video 
		Mat imgOriginalKeep;
		bool bSuccesskeep = cap.read(imgOriginalKeep);

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			//cout << "Found the end of the video" << endl;
			cout << "Video camera is disconnected" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		//morphological opening (removes small objects from the foreground
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

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

			xcoords.push_back(posX);
			ycoords.push_back(posY);

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				//Draw a line from the previous point to the current point
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 255, 0), 1);
			}

			iLastX = posX;
			iLastY = posY;

		}

		imgOriginal = imgOriginal + imgLines;

		/* run HoughCircles function on 1 image
		// color pixels back to original
		if (xcoords.size() > 10) {
			for (int i = 1; i < int(xcoords.size() / 2); i++) {
				Vec3b& color  = imgOriginal.at<Vec3b>(ycoords[i], xcoords[i]);
				Vec3b& color2 = imgOriginalKeep.at<Vec3b>(ycoords[i], xcoords[i]);

				color[0] = color2[0];
				color[1] = color2[1];
				color[2] = color2[2];
			}
		}
		*/

		//show the frames in the created windows
		//imshow("Original", imgOriginal); //show the original image

		/*
		// Create empty image
		Mat image_empty(600, 800, CV_8UC3, Scalar(0, 0, 0));
		//Mat image_empty = imread("C:/Users/leven/opencv/JGRiM.jpg");

		// Check for failure
		if (image_empty.empty())
		{
			cout << "Could not open or find the image" << endl;
			cin.get(); //wait for any key press
			return -1;
		}

		Mat image_empty_gr;

		String windowName2 = "Window with Blank Image"; //Name of the window

		namedWindow(windowName2, WINDOW_AUTOSIZE); // Create a window

		circle(image_empty, Point(400, 300), 50, Scalar(100, 250, 30), -1, 8, 0);

		cvtColor(image_empty, image_empty_gr, COLOR_BGR2GRAY);

		GaussianBlur(image_empty_gr, image_empty_gr, cv::Size(9, 9), 2, 2);

		vector<Vec3f> circles;
		HoughCircles(
			image_empty_gr, circles, HOUGH_GRADIENT,
			2,				// accumulator resolution (size of the image / 2)
			5,				// image_empty_gr.rows / 16,  // change this value to detect circles with different distances to each other
			100,			// canny high threshold
			100,			// minimum number of votes
			0, 1000			// change the last two parameters (min_radius & max_radius) to detect larger circles
		);

		for (size_t i = 0; i < circles.size(); i++)
		{
			Vec3i c = circles[i];
			Point center = Point(c[0], c[1]);
			cout << c[0] << " " << c[1] << endl;
			// circle center
			circle(image_empty, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
			// circle outline
			int radius = c[2];
			circle(image_empty, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
		}

		*/

		// Run HoughCircles on video
		Mat imgGr;
		cvtColor(imgOriginal, imgGr, COLOR_BGR2GRAY); //Convert the captured frame from BGR to HSV

		GaussianBlur(imgGr, imgGr, cv::Size(9, 9), 2, 2);

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

		/*
		// check if color coincides with object shape
		vector<Vec3f> circles2;
		int* circlestodrop = new int[];
		for (int i = 0; i < circles2.size(); i++) {

		}
		*/

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

		// imshow section 
		//imshow(windowName2, image_empty); // Show our image inside the created window.

		//show the frames in the created windows
		imshow("Original", imgOriginal); //show the original image

		// show thresholded image
		imshow("Thresholded Image", imgThresholded); //show the thresholded image

		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		int comm = waitKey(10);
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