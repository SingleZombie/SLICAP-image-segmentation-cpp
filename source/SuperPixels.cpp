#include "SuperPixels.h"
#include"ImageNeighbourUtil.h"

namespace SLICAP
{
	SuperPixels::SuperPixels(cv::Mat image) : tag(image.cols*image.rows), _image(image)
	{

	}

	cv::Mat SuperPixels::getVisibleSegmentedImage()
	{
		cv::Mat res;
		_image.copyTo(res);
		int cIndex = 0;
		for (int i = 0; i < _image.rows; i++)
		{
			for (int j = 0; j < _image.cols; j++, cIndex++)
			{
				bool same = true;
				int cTag = tag[cIndex];
				for (int d = 0; d < 4; d++)
				{
					int vi = i + neighbourX[d];
					int vj = j + neighbourY[d];
					if (validPos(vi, vj, _image.cols, _image.rows) && tag[getIndexFromPos(vi, vj, _image.cols)] != cTag)
					{
						same = false;
						break;
					}
				}
				if (!same)
				{
					res.at<cv::Vec3b>(i, j) = { 0, 0, 0 };
				}
			}
		}
		return res;
	}
}