#ifndef IMAGE_NEIGHBOUR_UTIL
#define IMAGE_NEIGHBOUR_UTIL
const int neighbourX[] = { 0, 0, 1, -1, 1, -1, 1, -1 };
const int neighbourY[] = { 1, -1, 0, 0, 1, 1, -1, -1 };
inline bool validPos(int i, int j, int width, int height)
{
	return i >= 0 && j >= 0 && i < height && j < width;
}
inline int getIndexFromPos(int i, int j, int width)
{
	return i * width + j;
}
inline void getPosFromIndex(int index, int width, int& i, int& j)
{
	i = index / width;
	j = index % width;
}
#endif