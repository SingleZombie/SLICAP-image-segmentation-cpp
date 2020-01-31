#include "main.h"

SLICAP::SuperPixelsGenerator generator;

int main(int argc, char* argv[])
{
	cv::Mat mat = cv::imread("p1.jpg");
	auto superPixels = generator.generateSuperPixels(mat);
	cv::cvtColor(mat, mat, cv::COLOR_Lab2BGR);
	cv::Mat outMat = superPixels.getVisibleSegmentedImage();
	cv::imshow("p1.jpg", outMat);
	cv::imwrite("output.jpg", outMat);
	cv::waitKey();
	return 0;
}