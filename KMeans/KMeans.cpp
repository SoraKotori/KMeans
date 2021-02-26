#include <chrono>
#include <iterator>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>
#include <numeric>
#include <utility>

auto SquareOfDifference = [](auto&& Left, auto&& Right)
{
	return (std::forward<decltype(Left)>(Left) - std::forward<decltype(Right)>(Right)) * (std::forward<decltype(Left)>(Left) - std::forward<decltype(Right)>(Right));
};

int main()
{
	typedef double ValueType;

	constexpr std::size_t IteratorCount = 2000;
	constexpr std::size_t ClusterCount = 3;

	std::vector<std::vector<ValueType>> MultiplicationTable =
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

	auto SizeN = std::size(MultiplicationTable.front());
	std::mt19937 RandomEngine(std::random_device{}());

	std::vector<std::vector<ValueType>> PrevCenter(ClusterCount, std::vector<ValueType>(SizeN));
	std::vector<std::vector<ValueType>> Center(PrevCenter);
	std::vector<ValueType> CenterDataCount(ClusterCount);

	std::vector<std::reference_wrapper<const std::vector<ValueType>>> RandomData(std::begin(MultiplicationTable), std::end(MultiplicationTable));
	std::shuffle(std::begin(RandomData), std::end(RandomData), RandomEngine);
	std::copy_n(std::begin(RandomData), std::size(PrevCenter), std::begin(PrevCenter));

	std::size_t Iterator;
	for (Iterator = 0; Iterator < IteratorCount; Iterator++)
	{
		// Center set zero
		std::for_each(std::begin(Center), std::end(Center), [](std::vector<ValueType>& Center)
			{
				std::fill(std::begin(Center), std::end(Center), ValueType(0));
			});
		std::fill(CenterDataCount.begin(), CenterDataCount.end(), ValueType(0));

		// Assignment and Update
		std::for_each(std::begin(MultiplicationTable), std::end(MultiplicationTable), [&](const std::vector<ValueType>& Point)
			{
				auto EuclideanDistanceSquaredSequence = std::vector<ValueType>(std::size(PrevCenter));
				std::transform(std::begin(PrevCenter), std::end(PrevCenter), std::begin(EuclideanDistanceSquaredSequence), [&Point](const std::vector<ValueType>& PrevCenter)
					{
						return transform_reduce(std::begin(Point), std::end(Point), std::begin(PrevCenter), ValueType(0), std::plus<>(), SquareOfDifference);
					});

				auto MinimumIndex = std::distance(std::begin(EuclideanDistanceSquaredSequence), std::min_element(std::begin(EuclideanDistanceSquaredSequence), std::end(EuclideanDistanceSquaredSequence)));
				std::transform(std::begin(Point), std::end(Point), std::begin(Center[MinimumIndex]), std::begin(Center[MinimumIndex]), std::plus<>());

				CenterDataCount[MinimumIndex]++;
			});

		std::transform(std::begin(CenterDataCount), std::end(CenterDataCount), std::begin(Center), std::begin(Center), [](const ValueType DataCount, std::vector<ValueType>& PrevCenter)
			{
				std::transform(std::begin(PrevCenter), std::end(PrevCenter), std::begin(PrevCenter), std::bind(std::divides<>(), std::placeholders::_1, DataCount));
				return PrevCenter;
			});

		swap(PrevCenter, Center);
		
		if (PrevCenter == Center)
		{
			break;
		}
	}

	for (auto& Data : MultiplicationTable)
	{
		auto FirstCenter = std::begin(PrevCenter);
		auto LastCenter = std::end(PrevCenter);

		auto MinDistance = transform_reduce(std::begin(Data), std::end(Data), FirstCenter->begin(), ValueType(0), std::plus<>(), SquareOfDifference);
		auto FoundCenter = FirstCenter;

		while (++FirstCenter != LastCenter)
		{
			auto Distance = transform_reduce(std::begin(Data), std::end(Data), FirstCenter->begin(), ValueType(0), std::plus<>(), SquareOfDifference);
			if (MinDistance > Distance)
			{
				MinDistance = Distance;
				FoundCenter = FirstCenter;
			}
		}

		auto ClusterIndex = std::distance(PrevCenter.begin(), FoundCenter);
		std::cout << ClusterIndex << std::endl;
	}
	std::cout << std::endl;

	for (auto& Center : Center)
	{
		copy(Center.begin(), Center.end(), std::ostream_iterator<double>(std::cout, " "));
		std::cout << std::endl;
	}
	std::cout << std::endl << "Iterator: " << Iterator << std::endl;

	return 0;
}
