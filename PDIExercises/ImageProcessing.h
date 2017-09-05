#pragma once
#pragma warning(disable:4996)
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class ImageProcessing
{
public:

	// Constructor and destructor
	ImageProcessing(string image_path, bool gray);
	~ImageProcessing();

	// Class Members
	string get_path();
	Mat get_image();
	void zoomIn(int height, int widht);
	void zoomOut(int height, int widht);
	void darkenImage(enum EnumMathOperation type);
	void lightenImage(enum EnumMathOperation type);
	void saveImage(string dstPath);
	void showImage();

private:

	Mat _image;
	string image_path;
	float image_density;
	map<int, int> pixel_distribution;

	Mat loadImage();
	Mat loadImageGrayScale();
	void set_path(string image_path);
	void set_image(bool type);
	bool exists(string filePath);
	Mat modifiedBilinearInterpolationZoomIn(Mat originalImage, int height, int widht);
	void generateRows(Mat originalImage, Mat newImage, int height);
	void generateCols(Mat originalImage, Mat newImage, int widht);
	float calculateZoomInRate(int originalRowNumber, int newRowNumber);
	Mat modifiedBilinearInterpolationZoomOut(Mat originalImage, int height, int widht);
	void removeRows(Mat originalImage, Mat newImage, int height);
	void removeCols(Mat originalImage, Mat newImage, int widht);
	float calculateZoomOutRate(int originalRowNumber, int newRowNumber);
	int calculateNewPixelColor(enum EnumMathOperation operation, enum EnumColorOperation, int pixel);
	void calculateImagePixelQuantities();
	Mat applyColorAdjustments(enum EnumMathOperation operation, enum EnumColorOperation);

};

