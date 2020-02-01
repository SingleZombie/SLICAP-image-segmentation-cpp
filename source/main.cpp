#include "main.h"

SLICAP::SuperPixelsGenerator generator(-1, 10, 10);
SLICAP::SuperPixelClusteringAlgorithm clusteringAlg;

std::string fileName;

int main(int argc, char* argv[])
{
	if (argc >= 2)
	{
		fileName = argv[1];
	}
	else
	{
		fileName = "p1.jpg";
	}
	cv::Mat mat = cv::imread(fileName);

	auto superPixels = generator.generateSuperPixels(mat);
	auto superPixelTag = clusteringAlg.clustering(superPixels);

	cv::cvtColor(mat, mat, cv::COLOR_Lab2BGR);
	cv::Mat outMat = superPixels.getVisibleSegmentedImage();
	cv::imshow("superpixels", outMat);
	cv::imwrite("superpixels.jpg", outMat);

	outMat = SLICAP::getClusteringResult(superPixels, superPixelTag);
	cv::imshow("superpixelsTag", outMat);
	cv::imwrite("superpixelsTag.jpg", outMat);

	int cIndex = 0;
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++, cIndex++)
		{
			mat.at<cv::Vec3b>(i, j) = superPixels.superPixelInfo[superPixelTag[superPixels.tag[cIndex]]].getColor();
		}
	}
	cv::cvtColor(mat, mat, cv::COLOR_Lab2BGR);
	cv::imshow("image segmentation", mat);
	cv::imwrite("segmentation.jpg", mat);

	cv::waitKey();
	return 0;
}