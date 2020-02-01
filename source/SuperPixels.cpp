#include "SuperPixels.h"
#include"ImageNeighbourUtil.h"

namespace SLICAP
{
	SuperPixels::SuperPixels(cv::Mat img) : tag(img.cols * img.rows), image(img)
	{

	}

	cv::Mat SuperPixels::getVisibleSegmentedImage()
	{
		cv::Mat res;
		image.copyTo(res);
		int cIndex = 0;
		for (int i = 0; i < image.rows; i++)
		{
			for (int j = 0; j < image.cols; j++, cIndex++)
			{
				bool same = true;
				int cTag = tag[cIndex];
				for (int d = 0; d < 4; d++)
				{
					int vi = i + neighbourX[d];
					int vj = j + neighbourY[d];
					if (validPos(vi, vj, image.cols, image.rows) && tag[getIndexFromPos(vi, vj, image.cols)] != cTag)
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