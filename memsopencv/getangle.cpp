// https://www.pixelto.net/px-to-cm-converter#:~:text=Assuming%20the%20pixel%20density%20is,are%2096%20pixels%20per%20inch.&text=So%20there%20are%2096%20pixels,0.026458333%20centimeters%20in%20a%20pixel.

#include "getangle.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

int getangle(int& lastpt, int& pt, int& ydist) {
	double x = ((double)pt - (double)lastpt) * 0.026458333; // 1 pixel = .026458333 cm
	return ( 180 - (int)(atan2(ydist,x) * 180/ M_PI ));
}