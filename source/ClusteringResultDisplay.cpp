#include "ClusteringResultDisplay.h"
#include "ImageNeighbourUtil.h"
namespace SLICAP
{
	cv::Mat getClusteringResult(const SuperPixels& superPixels, const std::vector<int> clusterTag)
	{
		cv::Mat res;
		superPixels.image.copyTo(res);
		int cIndex = 0;
		for (int i = 0; i < res.rows; i++)
		{
			for (int j = 0; j < res.cols; j++, cIndex++)
			{
				bool same = true;
				int cTag = clusterTag[superPixels.tag[cIndex]];
				for (int d = 0; d < 4; d++)
				{
					int vi = i + neighbourX[d];
					int vj = j + neighbourY[d];
					if (validPos(vi, vj, res.cols, res.rows) && clusterTag[superPixels.tag[getIndexFromPos(vi, vj, res.cols)]] != cTag)
					{
						same = false;
						break;
					}
				}
				if (!same)
				{
					res.at<cv::Vec3b>(i, j) = { 0, 0, 0 };
				}
				else if (superPixels.tag[cIndex] == cTag)
				{
					res.at<cv::Vec3b>(i, j) = { 0, 0, 255 };
				}
			}
		}
		return res;
	}
}