#ifndef SUPER_PIXEL_H
#define SUPER_PIXEL_H
#include <cassert>
#include <opencv2/core/core.hpp>
namespace SLICAP
{
	struct SuperPixel
	{
		int l, a, b, i, j;
		cv::Vec3i getColor() const { return { l, a, b }; }
		cv::Vec2i getPos() const { return { i, j }; }
	};
}

#endif


