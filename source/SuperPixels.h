#ifndef SUPER_PIXELS_H
#define SUPER_PIXELS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

#include "SuperPixel.h"

namespace SLICAP
{
	
	class SuperPixels
	{
	public:
		SuperPixels(cv::Mat image);

		cv::Mat getVisibleSegmentedImage();

		const SuperPixel& getSuperPixelInfoOfPixel(int pixelIndex) const
		{
			return superPixelInfo[tag[pixelIndex]];
		}

		std::vector<int> tag;
		std::vector<SuperPixel> superPixelInfo;
	private:
		cv::Mat _image;
		
	};
}


#endif

