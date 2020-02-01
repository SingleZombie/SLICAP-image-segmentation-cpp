#ifndef AP_CLUSTERING_H
#define AP_CLUSTERING_H

#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>

namespace SLICAP
{
	const int INF = 0x3f3f3f3f;
	std::vector<int> apClustering(const std::vector<int>& similarity, unsigned dataCount, double lambda = 0.5);
}

#endif