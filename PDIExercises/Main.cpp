#include <string>
#include <iostream>
#include "ImageProcessing.h"
#include "EnumMathOperation.h"

int main(int argc, char* argv[])
{
	try
	{
		ImageProcessing img("2.png", true);
		//img.lightenImage(EnumMathOperation::Logarithm);
		img.darkenImage(EnumMathOperation::Logarithm);
		//img.showImage();
		img.saveImage("2_log.png");

	}
	catch (const char* msg)
	{
		cout << msg << endl;
	}
	
	waitKey(0);

	return 0;
}

