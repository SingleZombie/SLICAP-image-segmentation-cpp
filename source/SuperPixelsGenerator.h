#ifndef SUPER_PIXELS_GENERATOR_H
#define SUPER_PIXELS_GENERATOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  

#include "SuperPixels.h"
#include <vector>

namespace SLICAP
{
	class SuperPixelsGenerator
	{
	public:
		SuperPixelsGenerator(int superPixelCountK = 500, double colorDisM = 10.0, int maxIterTime = 4);
		SuperPixels generateSuperPixels(cv::Mat image);
	private:
		void convertToLAB();
		void initSuperPixelsCenter(SuperPixels& superPixels);
		std::vector<int> calGradient();
		void updateEachPixel(SuperPixels& superPixels, std::shared_ptr<int> sumArray);
		void enforceConnectivity(SuperPixels& superPixels);
		int get5dDistance(cv::Vec3i dcolor, cv::Vec2i dpos);

		cv::Mat _image;
		int _superPixelCount;
		double _colorDisM;
		int _maxIterTime;
		int _expectedDis;
	};
}

#endif

