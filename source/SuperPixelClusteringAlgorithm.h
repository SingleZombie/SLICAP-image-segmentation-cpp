#ifndef SUPER_PIXEL_CLUSTERING_ALGORITHM_H
#define SUPER_PIXEL_CLUSTERING_ALGORITHM_H
#include "APClustering.h"
#include "SuperPixels.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> 
namespace SLICAP
{
	class SuperPixelClusteringAlgorithm
	{
	public:
		SuperPixelClusteringAlgorithm(int wl = 3, int wa = 10, int wb = 10);
		std::vector<int> clustering(const SuperPixels& superPixels);
	private:
		std::vector<int> getSimilarity(const SuperPixels& superPixels);
		int _wl, _wa, _wb;
	};
}
#endif

