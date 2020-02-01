#include "SuperPixelClusteringAlgorithm.h"
#include "ImageNeighbourUtil.h"
namespace SLICAP
{
	SuperPixelClusteringAlgorithm::SuperPixelClusteringAlgorithm(int wl, int wa, int wb) :
		_wl(wl), _wa(wa), _wb(wb)
	{

	}

	std::vector<int> SuperPixelClusteringAlgorithm::clustering(const SuperPixels& superPixels)
	{
		return apClustering(getSimilarity(superPixels), superPixels.superPixelInfo.size());
	}
	std::vector<int> SuperPixelClusteringAlgorithm::getSimilarity(const SuperPixels& superPixels)
	{
		int superPixelCount = superPixels.superPixelInfo.size();
		std::vector<int> similiarity = std::vector<int>(superPixelCount * superPixelCount);

		long long sumv = 0, minv = 0;
		int cIndex = 0;
		for (int i = 0; i < superPixelCount; i++)
		{
			for (int j = 0; j < superPixelCount; j++, cIndex++)
			{
				cv::Vec3i dColor = superPixels.superPixelInfo[i].getColor() - superPixels.superPixelInfo[j].getColor();
				cv::Vec2i dDis = superPixels.superPixelInfo[i].getPos() - superPixels.superPixelInfo[j].getPos();
				similiarity[cIndex] = -(_wl * dColor[0] * dColor[0] + _wa * dColor[1] * dColor[1] + _wb * dColor[2] * dColor[2]);
				sumv += similiarity[cIndex];
				minv = std::min(minv, long long(similiarity[cIndex]));
			}
		}
		sumv /= superPixelCount * superPixelCount - superPixelCount;
		for (int i = 0; i < superPixelCount; i++)
		{
			similiarity[getIndexFromPos(i, i, superPixelCount)] = sumv;
		}

		return similiarity;
	}
}