#ifndef CLUSTERING_RESULT_DISPLAY
#define CLUSTERING_RESULT_DISPLAY

#include "SuperPixels.h"

namespace SLICAP
{
	cv::Mat getClusteringResult(const SuperPixels& superPixels, const std::vector<int> clusterTag);
}

#endif