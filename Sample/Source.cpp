#include "KMeans.h"
#include <chrono>
#include <iterator>
#include <iostream>

int main()
{
	constexpr std::size_t IteratorCount = 2000;
	constexpr std::size_t ClusterCount = 3;

	std::vector<std::vector<double>> MultiplicationTable =
	{
		{1,  2,  3,  4,  5,  6,  7,  8,  9 },
		{2,  4,  6,  8, 10, 12, 14, 16, 18 },
		{3,  6,  9, 12, 15, 18, 21, 24, 27 },
		{4,  8, 12, 16, 20, 24, 28, 32, 36 },
		{5, 10, 15, 20, 25, 30, 35, 40, 45 },
		{6, 12, 18, 24, 30, 36, 42, 48, 54 },
		{7, 14, 21, 28, 35, 42, 49, 56, 63 },
		{8, 16, 24, 32, 40, 48, 56, 64, 72 },
		{9, 18, 27, 36, 45, 54, 63, 72, 81 }
	};

	Clustering::KMeans<double> KMS(std::begin(MultiplicationTable), std::end(MultiplicationTable), ClusterCount, std::mt19937(std::random_device()()));

	std::size_t Iterator;
	for (Iterator = 0; Iterator < IteratorCount; Iterator++)
	{
		bool _bResult = KMS.Run(MultiplicationTable.begin(), MultiplicationTable.end());

		if (!_bResult)
		{
			break;
		}
	}

	for (auto& Data : MultiplicationTable)
	{
		auto ClusterIndex = KMS.GetDataCluster(Data.begin(), Data.end());
		std::cout << ClusterIndex << std::endl;
	}
	std::cout << std::endl;

	for (auto& Center : KMS.GetCenter())
	{
		copy(Center.begin(), Center.end(), std::ostream_iterator<double>(std::cout, " "));
		std::cout << std::endl;
	}
	std::cout << std::endl << "Iterator: " << Iterator << std::endl;

	return 0;
}
