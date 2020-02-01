#include "APClustering.h"
#include "ImageNeighbourUtil.h"

namespace SLICAP
{
	std::vector<int> apClustering(const std::vector<int>& similarity, unsigned dataCount, double lambda)
	{
		double oneMinusLambda = 1 - lambda;
		unsigned matSize = dataCount * dataCount;
		assert(similarity.size() >= matSize);
		auto matR = std::vector<double>(matSize);
		auto matA = std::vector<double>(matSize);
		auto firstNum = std::vector<double>(matSize, -INF); // the 1st max number of s(i,k) + a(i,k);
		auto secondNum = std::vector<double>(matSize, -INF); // the 2nd max number of s(i,k) + a(i,k);
		auto sumTerm = std::vector<double>(dataCount); // the sum of sum_k{max{0, r(i,k)}}
		auto tag = std::vector<int>(dataCount, -1);

		int cIndex = 0;
		for (int i = 0; i < dataCount; i++)
		{
			for (int j = 0; j < dataCount; j++, cIndex++)
			{
				if (similarity[cIndex] > firstNum[i])
				{
					secondNum[i] = firstNum[i];
					firstNum[i] = similarity[cIndex];
				}
				else if (similarity[cIndex] > secondNum[i])
				{
					secondNum[i] = similarity[cIndex];
				}
			}
		}

		int iterTime = 0;
		int stopTime = 0;
		do
		{
			bool isSame = true;
			cIndex = 0;
			sumTerm = std::vector<double>(dataCount);
			for (int i = 0; i < dataCount; i++)
			{
				for (int j = 0; j < dataCount; j++, cIndex++)
				{
					int maxAS = firstNum[i];
					if (firstNum[i] == matA[cIndex] + similarity[cIndex])
					{
						maxAS = secondNum[i];
					}
					double newR = similarity[cIndex] - maxAS;
					if (iterTime == 0)
					{
						matR[cIndex] = newR;
					}
					else
					{
						matR[cIndex] = lambda * matR[cIndex] + oneMinusLambda * newR;
					}
					
					if (i != j)
					{
						sumTerm[j] += std::max(0.0, matR[cIndex]);
					}
				}
			}
			
			firstNum = std::vector<double>(matSize, -INF);
			secondNum = std::vector<double>(matSize, -INF);
			cIndex = 0;
			for (int i = 0; i < dataCount; i++)
			{
				int crtSumAR = -INF, crtTag = -1;
				for (int j = 0; j < dataCount; j++, cIndex++)
				{
					int rkk = matR[getIndexFromPos(j, j, dataCount)];
					double newA;
					if (i == j)
					{
						newA = sumTerm[j];
					}
					else
					{
						newA = std::min(0.0, rkk + sumTerm[j] - std::max(0.0, matR[cIndex]));
					}
					matA[cIndex] = lambda * matA[cIndex] + oneMinusLambda * newA;

					if (matA[cIndex] + similarity[cIndex] > firstNum[i])
					{
						secondNum[i] = firstNum[i];
						firstNum[i] = matA[cIndex] + similarity[cIndex];
					}
					else if (matA[cIndex] + similarity[cIndex] > secondNum[i])
					{
						secondNum[i] = matA[cIndex] + similarity[cIndex];
					}

					if (crtSumAR == -INF || crtSumAR < matA[cIndex] + matR[cIndex])
					{
						crtTag = j;
						crtSumAR = matA[cIndex] + matR[cIndex];
					}
				}
				if (crtTag != tag[i])
				{
					tag[i] = crtTag;
					isSame = false;
				}
			}
			iterTime++;
			if (isSame)
			{
				stopTime++;
			}
			else
			{
				stopTime = 0;
			}
		} 
		while (stopTime < 6);
		//while (iterTime++ < 30);

		//printf("iterTime = %d\n", iterTime);
		return tag;
	}

	
}