#include "ImageProcessing.h"
#include "EnumMathOperation.h"
#include "EnumColorOperation.h"

ImageProcessing::ImageProcessing(string image_path, bool gray_scale = 0)
{
	this->set_path(image_path);
	this->set_image(gray_scale);
}


ImageProcessing::~ImageProcessing()
{
}

void ImageProcessing::set_path(string image_path)
{
	if (exists(image_path))
	{
		this->image_path = image_path;
	}
	else
	{
		throw "The file does not exists";
	}
}

string ImageProcessing::get_path()
{
	return this->image_path;
}

Mat ImageProcessing::get_image()
{
	return this->_image;
}

void ImageProcessing::set_image(bool type)
{
	if (type)
	{
		this->_image = this->loadImageGrayScale();
	}
	else
	{
		this->_image = this->loadImage();
	}
}

Mat ImageProcessing::loadImage()
{
	return imread(this->get_path());
}

Mat ImageProcessing::loadImageGrayScale()
{
	return imread(this->get_path(), 0);
}

void ImageProcessing::saveImage(string dstPath)
{
	imwrite(dstPath, this->_image);
}

void ImageProcessing::showImage()
{
	namedWindow(this->get_path(), CV_WINDOW_AUTOSIZE);
	imshow(this->get_path(), this->get_image());
}

void ImageProcessing::zoomIn(int height, int widht)
{
	this->_image = modifiedBilinearInterpolationZoomIn(this->get_image().clone(), height, widht);
}

Mat ImageProcessing::modifiedBilinearInterpolationZoomIn(Mat originalImage, int height, int widht)
{
	Mat toRowsImage(height, originalImage.cols, DataType<uchar>::type);
	Mat finalImage(height, widht, DataType<uchar>::type);

	generateRows(originalImage, toRowsImage, height);
	generateCols(toRowsImage, finalImage, widht);

	return finalImage;
}

void ImageProcessing::generateRows(Mat originalImage, Mat newImage, int heigth)
{
	int zoomRate = (int)(calculateZoomInRate(originalImage.rows, heigth));

	for (int col = 0; col < originalImage.cols; col++)
	{
		int offset = -1;
		for (int row = 0; row < originalImage.rows; row++)
		{
			int zoomIterator = zoomRate;

			int lower = 0;
			float op = 0;

			if (row + 1 < originalImage.rows && offset < heigth)
			{
				Scalar firstPixel = originalImage.at<uchar>(row, col);
				int firstPixelValue = firstPixel[0];
				Scalar secondPixel = originalImage.at<uchar>(row + 1, col);
				int secondPixelValue = secondPixel[0];

				while (!((offset + ((originalImage.rows - row) * zoomIterator)) < heigth))
				{
					zoomIterator--;
				}

				newImage.at<uchar>(++offset, col) = firstPixelValue;

				if (firstPixelValue < secondPixelValue)
				{
					op = secondPixelValue - firstPixelValue;
					lower = firstPixelValue;
				}
				else
				{
					op = firstPixelValue - secondPixelValue;
					lower = secondPixelValue;
				}

				op /= zoomRate;
				int newValue = lower + op;
				int iteratorAux = zoomIterator;

				while (iteratorAux > 0)
				{
					offset++;
					newImage.at<uchar>(offset, col) = newValue;
					newValue += op;
					iteratorAux--;
				}

			}
			else
			{
				Scalar lastPixel = originalImage.at<uchar>(row, col);
				int lastPixelValue = lastPixel[0];

				while ((offset + 1) < heigth)
				{
					offset++;
					newImage.at<uchar>(offset, col) = lastPixelValue + op;
				}

			}
		}
	}
}

void ImageProcessing::generateCols(Mat originalImage, Mat newImage, int widht)
{
	int zoomRate = (int)(calculateZoomInRate(originalImage.cols, widht));

	for (int row = 0; row < originalImage.rows; row++)
	{
		int offset = -1;
		for (int col = 0; col < originalImage.cols; col++)
		{
			int zoomIterator = zoomRate;

			int lower = 0;
			float op = 0;

			if (col + 1 < originalImage.cols && offset < widht)
			{
				Scalar firstPixel = originalImage.at<uchar>(row, col);
				int firstPixelValue = firstPixel[0];
				Scalar secondPixel = originalImage.at<uchar>(row, col + 1);
				int secondPixelValue = secondPixel[0];

				while (!((offset + ((originalImage.cols - col) * zoomIterator)) < widht))
				{
					zoomIterator--;
				}

				newImage.at<uchar>(row, ++offset) = firstPixelValue;

				if (firstPixelValue < secondPixelValue)
				{
					op = secondPixelValue - firstPixelValue;
					lower = firstPixelValue;
				}
				else
				{
					op = firstPixelValue - secondPixelValue;
					lower = secondPixelValue;
				}

				op /= zoomRate;
				int newValue = lower + op;
				int iteratorAux = zoomIterator;

				while (iteratorAux > 0)
				{
					offset++;
					newImage.at<uchar>(row, offset) = newValue;
					newValue += op;
					iteratorAux--;
				}

				//TODO Melhorar resultados - ordenar array de row + 1 até row + offset para suavizar os pixels gerados
			}
			else
			{
				Scalar lastPixel = originalImage.at<uchar>(row, col);
				int lastPixelValue = lastPixel[0];

				while ((offset + 1) < widht)
				{
					offset++;
					newImage.at<uchar>(row, offset) = lastPixelValue + op;
				}

			}
		}
	}
}

float ImageProcessing::calculateZoomInRate(int originalValue, int newValue)
{
	return (float)(newValue - 1) / (float)(originalValue - 1);
}

float ImageProcessing::calculateZoomOutRate(int originalValue, int newValue)
{
	return (float)(originalValue - 1) / (float)(newValue - 1);
}

bool ImageProcessing::exists(string filePath)
{
	FILE *file;
	if ((file = fopen(filePath.c_str(), "r")))
	{
		fclose(file);
		return true;
	}
	return false;
}

void ImageProcessing::removeRows(Mat originalImage, Mat newImage, int height)
{
	float zoomRate = this->calculateZoomOutRate(originalImage.rows, height);
	int intPart = (int)zoomRate;
	int decimalPart = (int)(zoomRate * 100) % 100;
	int lowerPartCount = -1;
	int upperPartCount = -1;

	if (decimalPart > 0)
	{
		lowerPartCount = (int)((float)(newImage.rows * ((float)(100 - decimalPart) / (float)100)));
		upperPartCount = newImage.rows - lowerPartCount;
	}

	zoomRate++;

	for (int col = 0; col < newImage.cols; col++)
	{
		float zoomRateAux = zoomRate;
		int offset = 0;
		for (int row = 0; row < newImage.rows; row++)
		{
			if (offset + 1 < originalImage.rows)
			{
				int newPixelValue = 0;
				int zoomRateIterator = (int)zoomRateAux;

				if (row > upperPartCount && zoomRateIterator > 1)
				{
					zoomRateIterator--;
				}

				int div = zoomRateIterator;

				while (zoomRateIterator > 0)
				{
					Scalar currentPixel = originalImage.at<uchar>(offset++, col);
					newPixelValue += currentPixel[0];
					zoomRateIterator--;
				}

				newImage.at<uchar>(row, col) = newPixelValue / div;
			}
			else
			{
				Scalar lastPixel = originalImage.at<uchar>(offset, col);
				int lastPixelValue = lastPixel[0];
				newImage.at<uchar>(row, col) = lastPixelValue;
			}
		}
	}
}

void ImageProcessing::removeCols(Mat originalImage, Mat newImage, int newWidth)
{
	float zoomRate = this->calculateZoomOutRate(originalImage.cols, newWidth);
	int intPart = (int)zoomRate;
	int decimalPart = (int)(zoomRate * 100) % 100;
	int lowerPartCount = -1;
	int upperPartCount = -1;

	if (decimalPart > 0)
	{
		lowerPartCount = (int)((float)(newImage.cols * ((float)(100 - decimalPart) / (float)100)));
		upperPartCount = newImage.cols - lowerPartCount;
	}

	zoomRate++;

	for (int row = 0; row < newImage.rows; row++)
	{
		float zoomRateAux = zoomRate;
		int offset = 0;
		for (int col = 0; col < newImage.cols; col++)
		{
			if (offset + 1 < originalImage.cols)
			{
				int newPixelValue = 0;
				int zoomRateIterator = (int)zoomRateAux;

				if (col > upperPartCount && zoomRateIterator > 1)
				{
					zoomRateIterator--;
				}

				int div = zoomRateIterator;

				while (zoomRateIterator > 0)
				{
					Scalar currentPixel = originalImage.at<uchar>(row, offset++);
					newPixelValue += currentPixel[0];
					zoomRateIterator--;
				}

				newImage.at<uchar>(row, col) = newPixelValue / div;
			}
			else
			{
				Scalar lastPixel = originalImage.at<uchar>(row, offset);
				int lastPixelValue = lastPixel[0];
				newImage.at<uchar>(row, col) = lastPixelValue;
			}
		}
	}
}

Mat ImageProcessing::modifiedBilinearInterpolationZoomOut(Mat originalImage, int newHeigth, int newWidth)
{
	Mat toRowsImage(newHeigth, originalImage.cols, DataType<uchar>::type);
	Mat finalImage(newHeigth, newWidth, DataType<uchar>::type);

	this->removeRows(originalImage, toRowsImage, newHeigth);
	this->removeCols(toRowsImage, finalImage, newWidth);

	return finalImage;
}

void ImageProcessing::zoomOut(int heiht, int widht)
{
	this->_image = this->modifiedBilinearInterpolationZoomOut(this->get_image().clone(), heiht, widht);
}

void ImageProcessing::darkenImage(EnumMathOperation operation)
{
	this->_image = this->applyColorAdjustments(operation, EnumColorOperation::Darken);
}

Mat ImageProcessing::applyColorAdjustments(EnumMathOperation operation, EnumColorOperation color)
{
	Mat originalImage = this->get_image().clone();

	for (int row = 0; row < originalImage.rows; row++)
	{
		for (int col = 0; col < originalImage.cols; col++)
		{
			Scalar currentPixel = originalImage.at<uchar>(row, col);
			int pixel = currentPixel[0];

			int newValue = this->calculateNewPixelColor(operation, color, pixel);
			originalImage.at<uchar>(row, col) = newValue;
		}
	}

	return originalImage;
}

int ImageProcessing::calculateNewPixelColor(EnumMathOperation operation, EnumColorOperation color, int pixel)
{
	int newValue = 0;

	switch (operation)
	{
	case EnumMathOperation::Logarithm:
	{
		if (color == EnumColorOperation::Darken)
		{
			double c = log(256) / (double)(255);

			newValue = pow(exp(pixel), c) - 1;
		}
		else
		{
			newValue = ((float)255 / log((float)(1 + 255))) * log((float)(1 + pixel));
		}

		break;
	}
	case EnumMathOperation::Gamma:
	{
		if (color == EnumColorOperation::Darken)
		{
			//255 max value in gray scale image (8 bpp), gamma fixed in 2
			newValue = 255 * (pow((float)pixel / (float)255, 2));
		}
		else
		{
			//255 max value in gray scale image (8 bpp), gamma fixed in 0.5
			newValue = 255 * (pow((float)pixel / (float)255, 0.5));
		}
		break;
	}
	case EnumMathOperation::HistogramEqualization:
	{
		if (this->image_density == 0)
		{
			this->image_density = (float)(256 - 1) / (float)(this->get_image().rows * this->get_image().cols);
		}

		this->calculateImagePixelQuantities();

		int cumulativeQuantity = 0;
		map<int, int>::iterator it;

		for (it = this->pixel_distribution.begin(); it != this->pixel_distribution.end(); it++)
		{
			if (it->first <= pixel)
			{
				cumulativeQuantity += it->second;
			}
			else
			{
				break;
			}
		}

		newValue = this->image_density * cumulativeQuantity;
		break;
	}

	}

	return newValue;
}

void ImageProcessing::lightenImage(EnumMathOperation operation)
{
	this->_image = this->applyColorAdjustments(operation, EnumColorOperation::Lighten);
}

void ImageProcessing::calculateImagePixelQuantities()
{

	if (this->pixel_distribution.empty())
	{

		for (int i = 0; i < this->get_image().rows; i++)
		{
			for (int j = 0; j < this->get_image().cols; j++)
			{

				Scalar pixel = this->get_image().at<uchar>(i, j);
				int pixelValue = pixel[0];

				if (this->pixel_distribution.count(pixelValue) == 0)
				{
					this->pixel_distribution[pixelValue] = 1;
				}
				else
				{
					int count = this->pixel_distribution.find(pixelValue)->second;
					this->pixel_distribution[pixelValue] = ++count;
				}

			}
		}

	}

}