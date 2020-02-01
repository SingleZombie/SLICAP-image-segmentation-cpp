#include "SuperPixelsGenerator.h"
#include "ImageNeighbourUtil.h"
#include <queue>
namespace SLICAP
{
	SuperPixelsGenerator::SuperPixelsGenerator(int superPixelCountK, double colorDisM, int maxIterTime) :
		_superPixelCount(superPixelCountK),
		_colorDisM(colorDisM),
		_maxIterTime(maxIterTime)
	{
		
	}
	SuperPixels SuperPixelsGenerator::generateSuperPixels(cv::Mat image)
	{
		bool adaptiveSuperPixelCount = false;
		if (_superPixelCount == -1)
		{
			adaptiveSuperPixelCount = true;
			_superPixelCount = image.cols * image.rows / 1000;
		}

		_image = image;
		_expectedDis = sqrt(_image.cols * _image.rows / _superPixelCount) + 2; // plus 2 to avoid S become too small

		convertToLAB();
		SuperPixels superPixels(_image);
		
		initSuperPixelsCenter(superPixels);
		{
			auto sumArray = std::shared_ptr<int>(new int[_superPixelCount * 6]);
			for (int i = 0; i < _maxIterTime; i++)
			{
				updateEachPixel(superPixels, sumArray);
			}
		}

		enforceConnectivity(superPixels);

		if (adaptiveSuperPixelCount)
		{
			_superPixelCount = -1;
		}

		return superPixels;
	}

	void SuperPixelsGenerator::convertToLAB()
	{
		cv::cvtColor(_image, _image, cv::COLOR_BGR2Lab);
	}
	void SuperPixelsGenerator::initSuperPixelsCenter(SuperPixels& superPixels)
	{
		const auto& gradient = calGradient();

		int actualCount = 0;
		int halfExpectedDis = _expectedDis / 2;
		for (int i = halfExpectedDis; i < _image.rows; i += _expectedDis)
		{
			for (int j = halfExpectedDis; j < _image.cols; j += _expectedDis)
			{
				actualCount++;

				int ci = i, cj = j, cIndex = getIndexFromPos(i, j, _image.cols);
				for (int d = 0; d < 8; d++)
				{
					int vi = i + neighbourX[d];
					int vj = j + neighbourY[d];
					int vIndex = getIndexFromPos(vi, vj, _image.cols);
					if (validPos(vi, vj, _image.cols, _image.rows) && gradient[cIndex] > gradient[vIndex])
					{
						ci = vi, cj = vj, cIndex = vIndex;
					}
				}

				superPixels.superPixelInfo.push_back({
					_image.at<cv::Vec3b>(ci, cj)[0],
					_image.at<cv::Vec3b>(ci, cj)[1],
					_image.at<cv::Vec3b>(ci, cj)[2],
					ci,
					cj
					});
			}
		}
		int superPixelCols = (_image.cols - halfExpectedDis) / _expectedDis;
		for (int i = 0; i < _image.rows; i++)
		{
			for (int j = 0; j < _image.cols; j++)
			{
				int cIndex = getIndexFromPos(i, j, _image.cols);
				int tagIndex = getIndexFromPos((i - halfExpectedDis) / _expectedDis, (j - halfExpectedDis) / _expectedDis, superPixelCols);
				superPixels.tag[cIndex] = tagIndex;
			}
		}

		_superPixelCount = actualCount;
	}
	std::vector<int> SuperPixelsGenerator::calGradient()
	{
		std::vector<int> res(_image.cols * _image.rows);
		for (int i = 0; i < _image.rows; i++)
		{
			for (int j = 0; j < _image.cols; j++)
			{
				cv::Vec3b y1 = validPos(i - 1, j, _image.cols, _image.rows) ? _image.at<cv::Vec3b>(i - 1, j) : cv::Vec3b(0, 0, 0);
				cv::Vec3b y2 = validPos(i + 1, j, _image.cols, _image.rows) ? _image.at<cv::Vec3b>(i + 1, j) : cv::Vec3b(0, 0, 0);
				cv::Vec3b x1 = validPos(i, j - 1, _image.cols, _image.rows) ? _image.at<cv::Vec3b>(i, j - 1) : cv::Vec3b(0, 0, 0);
				cv::Vec3b x2 = validPos(i, j + 1, _image.cols, _image.rows) ? _image.at<cv::Vec3b>(i, j + 1) : cv::Vec3b(0, 0, 0);
				y2 -= y1;
				x2 -= x1;
				res[getIndexFromPos(i, j, _image.cols)] = y2.ddot(y2)+x2.ddot(x1);
			}
		}
		return res;
	}
	void SuperPixelsGenerator::updateEachPixel(SuperPixels& superPixels, std::shared_ptr<int> sumArray)
	{
		memset(sumArray.get(), 0, sizeof(int) * 6 * _superPixelCount);
		
		for (int tagIndex = 0; tagIndex < _superPixelCount; tagIndex++)
		{
			int i = superPixels.superPixelInfo[tagIndex].i, j = superPixels.superPixelInfo[tagIndex].j;
			int cIndex = getIndexFromPos(i, j, _image.cols);
			int i0 = std::max(i - _expectedDis, 0), i1 = std::min(i + _expectedDis, _image.rows - 1);
			int j0 = std::max(j - _expectedDis, 0), j1 = std::min(j + _expectedDis, _image.cols - 1);

			for (int vi = i0; vi <= i1; vi++)
			{
				for (int vj = j0; vj <= j1; vj++)
				{
					int vIndex = getIndexFromPos(vi, vj, _image.cols);
					if (superPixels.tag[vIndex] != superPixels.tag[cIndex])
					{
						int cDistance = get5dDistance(
							superPixels.superPixelInfo[tagIndex].getColor() - cv::Vec3i(_image.at<cv::Vec3b>(vi, vj)),
							superPixels.superPixelInfo[tagIndex].getPos() - cv::Vec2i(vi, vj));
						int vDistance = get5dDistance(
							superPixels.getSuperPixelInfoOfPixel(vIndex).getColor() - cv::Vec3i(_image.at<cv::Vec3b>(vi, vj)),
							superPixels.getSuperPixelInfoOfPixel(vIndex).getPos() - cv::Vec2i(vi, vj));
						if (vDistance > cDistance)
						{
							superPixels.tag[vIndex] = tagIndex;
						}
					}
				}
			}
		}

		int cIndex = 0;
		for (int i = 0; i < _image.rows; i++)
		{
			for (int j = 0; j < _image.cols; j++, cIndex++)
			{
				int tagArrayIndex = superPixels.tag[cIndex] * 6;
				*(sumArray.get() + tagArrayIndex + 0) += _image.at<cv::Vec3b>(i, j)[0];
				*(sumArray.get() + tagArrayIndex + 1) += _image.at<cv::Vec3b>(i, j)[1];
				*(sumArray.get() + tagArrayIndex + 2) += _image.at<cv::Vec3b>(i, j)[2];
				*(sumArray.get() + tagArrayIndex + 3) += i;
				*(sumArray.get() + tagArrayIndex + 4) += j;
				*(sumArray.get() + tagArrayIndex + 5) += 1;
			}
		}

		for (int i = 0; i < _superPixelCount; i++)
		{
			int offset = 6 * i;
			int cnt = *(sumArray.get() + offset + 5);
			if (cnt != 0)
			{
				superPixels.superPixelInfo[i] = {
				*(sumArray.get() + offset + 0) / cnt,
				*(sumArray.get() + offset + 1) / cnt,
				*(sumArray.get() + offset + 2) / cnt,
				*(sumArray.get() + offset + 3) / cnt,
				*(sumArray.get() + offset + 4) / cnt
				};
			}
			else
			{
				superPixels.superPixelInfo[i] = { 0, 0, 0, 0, 0 };
			}
			
		}
	}
	void SuperPixelsGenerator::enforceConnectivity(SuperPixels& superPixels)
	{
		int expectedMinSuperPixelSize = _expectedDis * _expectedDis / 2;

		std::vector<int> newTag(_image.cols * _image.rows, -1);
		int newTagCount = 0;

		int cIndex = 0;
		for (int i = 0; i < _image.rows; i++)
		{
			for (int j = 0; j < _image.cols; j++, cIndex++)
			{
				if (newTag[cIndex] == -1)
				{
					int prevSuperPixelTag = -1;
					int oldTag = superPixels.tag[cIndex];
					for (int d = 0; d < 4; d++)
					{
						int vi = i + neighbourX[d];
						int vj = j + neighbourY[d];
						if (validPos(vi, vj, _image.cols, _image.rows))
						{
							int vTag = newTag[getIndexFromPos(vi, vj, _image.cols)];
							if (vTag != -1)
							{
								prevSuperPixelTag = vTag;
								break;
							}
						}
					}

					std::queue<int> bfsQueue;
					std::vector<int> newPixelIndice;
					bfsQueue.push(cIndex);
					newTag[cIndex] = newTagCount;
					while (!bfsQueue.empty())
					{
						int cIndex = bfsQueue.front();
						int ci, cj;
						getPosFromIndex(cIndex, _image.cols, ci, cj);
						bfsQueue.pop();
						newPixelIndice.push_back(cIndex);

						for (int d = 0; d < 4; d++)
						{
							int vi = ci + neighbourX[d];
							int vj = cj + neighbourY[d];
							int vIndex = getIndexFromPos(vi, vj, _image.cols);
							if (validPos(vi, vj, _image.cols, _image.rows))
							{
								if (newTag[vIndex] == -1 && superPixels.tag[vIndex] == oldTag)
								{
									newTag[vIndex] = newTagCount;
									bfsQueue.push(vIndex);
								}
							}
						}
					}

					if (newPixelIndice.size() < expectedMinSuperPixelSize && prevSuperPixelTag != -1)
					{
						for (auto index : newPixelIndice)
						{
							newTag[index] = prevSuperPixelTag;
						}
					}
					else
					{
						newTagCount++;
					}
				}
			}
		}

		superPixels.tag = newTag;
		newTag.clear();

		superPixels.superPixelInfo = std::vector<SuperPixel>(newTagCount);
		std::vector<int> superPixelSize = std::vector<int>(newTagCount);
		cIndex = 0;
		for (int i = 0; i < _image.rows; i++)
		{
			for (int j = 0; j < _image.cols; j++, cIndex++)
			{
				int tagIndex = superPixels.tag[cIndex];
				superPixelSize[tagIndex]++;
				superPixels.superPixelInfo[tagIndex].l += _image.at<cv::Vec3b>(i, j)[0];
				superPixels.superPixelInfo[tagIndex].a += _image.at<cv::Vec3b>(i, j)[1];
				superPixels.superPixelInfo[tagIndex].b += _image.at<cv::Vec3b>(i, j)[2];
				superPixels.superPixelInfo[tagIndex].i += i;
				superPixels.superPixelInfo[tagIndex].j += j;
			}
		}
		for (int i = 0; i < newTagCount; i++)
		{
			superPixels.superPixelInfo[i].l /= superPixelSize[i];
			superPixels.superPixelInfo[i].a /= superPixelSize[i];
			superPixels.superPixelInfo[i].b /= superPixelSize[i];
			superPixels.superPixelInfo[i].i /= superPixelSize[i];
			superPixels.superPixelInfo[i].j /= superPixelSize[i];
		}
	}

	int SuperPixelsGenerator::get5dDistance(cv::Vec3i dcolor, cv::Vec2i dpos)
	{
		return dcolor.ddot(dcolor) + dpos.ddot(dpos) / _expectedDis / _expectedDis * _colorDisM * _colorDisM;
	}
}
