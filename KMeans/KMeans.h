#pragma once
#include <algorithm>
#include <functional>
#include <random>
#include <vector>
#include <numeric>

namespace Clustering
{
	template<typename Type>
	class KMeans
	{
	public:
		typedef typename std::vector<Type>::size_type SizeType;
		typedef typename std::vector<Type>::iterator Iterator;

		KMeans() = default;
		~KMeans() = default;

		KMeans(SizeType Dimension, SizeType Cluster) :
			mPrevCenter(Cluster, std::vector<Type>(Dimension)),
			mCenter(Cluster, std::vector<Type>(Dimension)),
			mCenterDataCount(Cluster)
		{
		}

		template<typename ForwardIteratorType, typename... ArgsType>
		KMeans(ForwardIteratorType First, ForwardIteratorType Last, SizeType Cluster, ArgsType&&... Args) :
			KMeans(First->size(), Cluster)
		{
			ResetCenterFromData(First, Last, std::forward<ArgsType>(Args)...);
		}

		template<typename ForwardIteratorType, typename... ArgsType>
		void ResetCenterFromData(ForwardIteratorType First, ForwardIteratorType Last, ArgsType&&... Args)
		{
			std::vector<std::reference_wrapper<const std::vector<Type>>> RandomData(First, Last);
			std::shuffle(RandomData.begin(), RandomData.end(), std::default_random_engine(std::forward<ArgsType>(Args)...));
			std::copy_n(RandomData.begin(), mPrevCenter.size(), mPrevCenter.begin());
		}

		template<typename ForwardIteratorType>
		auto GetDataCluster(ForwardIteratorType First, ForwardIteratorType Last)
		{
			auto FirstCenter = std::begin(mPrevCenter);
			auto LastCenter = std::end(mPrevCenter);

			auto MinDistance = EuclideanDistanceSquared(First, Last, FirstCenter->begin());
			auto FoundCenter = FirstCenter;

			while (++FirstCenter != LastCenter)
			{
				auto Distance = EuclideanDistanceSquared(First, Last, FirstCenter->begin());
				if (MinDistance > Distance)
				{
					MinDistance = Distance;
					FoundCenter = FirstCenter;
				}
			}

			return distance(mPrevCenter.begin(), FoundCenter);
		}

		auto GetCenter()
		{
			return mCenter;
		}

		template<typename... ArgsType>
		bool Run(ArgsType&&... Args)
		{
			UpdateCenter(std::forward<ArgsType>(Args)...);
			swap(mPrevCenter, mCenter);

			bool Converged = mPrevCenter == mCenter;
			return !Converged;
		}

	private:
		std::vector<std::vector<Type>> mPrevCenter;
		std::vector<std::vector<Type>> mCenter;
		std::vector<Type> mCenterDataCount;

		template<typename InputIterator1, typename InputIterator2>
		static Type EuclideanDistanceSquared(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2)
		{
			return std::transform_reduce(First1, Last1, First2, Type(0), std::plus<Type>(),
				std::bind(std::pow<Type, Type>, std::bind(std::minus<Type>(), std::placeholders::_1, std::placeholders::_2), 2));
		}

		template<typename... ArgsType>
		static Type EuclideanDistance(ArgsType&&... Args)
		{
			return sqrt(EuclideanDistanceSquared(forward<ArgsType>(Args)...));
		}

		template<typename ForwardIteratorType>
		void CenterAddData(ForwardIteratorType First, ForwardIteratorType Last)
		{
			auto EuclideanDistanceSquaredSequence = std::vector<Type>(std::size(mPrevCenter));
			std::transform(std::begin(mPrevCenter), std::end(mPrevCenter), std::begin(EuclideanDistanceSquaredSequence), [First, Last](const std::vector<Type>& PrevCenter)
				{
					return std::transform_reduce(First, Last, std::begin(PrevCenter), Type(0), std::plus<Type>(),
						std::bind(std::pow<Type, Type>, std::bind(std::minus<Type>(), std::placeholders::_1, std::placeholders::_2), 2));
				});

			auto MinimumIndex = std::distance(std::begin(EuclideanDistanceSquaredSequence), std::min_element(std::begin(EuclideanDistanceSquaredSequence), std::end(EuclideanDistanceSquaredSequence)));
			std::transform(First, Last, std::begin(mCenter[MinimumIndex]), std::begin(mCenter[MinimumIndex]), std::plus<Type>());

			mCenterDataCount[MinimumIndex]++;
		}

		template<typename ForwardIteratorType, typename OutputIterator>
		void CenterDivideCount(ForwardIteratorType First, ForwardIteratorType Last, OutputIterator Result)
		{
			for (; First != Last; ++First, ++Result)
			{
				const auto Count = *First;
				for (auto& Value : *Result)
				{
					Value /= Count;
				}
			}
		}

		template<typename ForwardIteratorType>
		void UpdateCenter(ForwardIteratorType First, ForwardIteratorType Last)
		{
			// Center set zero
			std::for_each(std::begin(mCenter), std::end(mCenter), [](std::vector<Type>& Center)
				{
					std::fill(std::begin(Center), std::end(Center), Type(0));
				});
			std::fill(mCenterDataCount.begin(), mCenterDataCount.end(), Type(0));

			// Assignment and Update
			std::for_each(First, Last, [this](const std::vector<Type>& Point)
				{
					auto EuclideanDistanceSquaredSequence = std::vector<Type>(std::size(mPrevCenter));
					std::transform(std::begin(mPrevCenter), std::end(mPrevCenter), std::begin(EuclideanDistanceSquaredSequence), [&Point](const std::vector<Type>& PrevCenter)
						{
							return std::transform_reduce(std::begin(Point), std::end(Point), std::begin(PrevCenter), Type(0), std::plus<Type>(),
								std::bind(std::pow<Type, Type>, std::bind(std::minus<Type>(), std::placeholders::_1, std::placeholders::_2), 2));
						});

					auto MinimumIndex = std::distance(std::begin(EuclideanDistanceSquaredSequence), std::min_element(std::begin(EuclideanDistanceSquaredSequence), std::end(EuclideanDistanceSquaredSequence)));
					std::transform(std::begin(Point), std::end(Point), std::begin(mCenter[MinimumIndex]), std::begin(mCenter[MinimumIndex]), std::plus<Type>());

					mCenterDataCount[MinimumIndex]++;
				});

			std::transform(std::begin(mCenterDataCount), std::end(mCenterDataCount), std::begin(mCenter), std::begin(mCenter), [](const Type DataCount, std::vector<Type>& PrevCenter)
				{
					std::transform(std::begin(PrevCenter), std::end(PrevCenter), std::begin(PrevCenter), std::bind(std::divides<Type>(), std::placeholders::_1, DataCount));
					return PrevCenter;
				});
		}
	};
}
