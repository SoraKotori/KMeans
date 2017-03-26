#pragma once
#include <algorithm>
#include <functional>
#include <random>
#include <vector>

namespace clustering
{
    using namespace std;
    using namespace placeholders;

    template<typename _Type>
    class KMeans
    {
    public:
        typedef typename vector<_Type>::size_type size_type;
        typedef typename vector<_Type>::iterator iterator;

        KMeans() = default;
        ~KMeans() = default;

        KMeans(size_type _Cluster, size_type _Dimension) :
            _PrevCenter(_Cluster, vector<_Type>(_Dimension)),
            _Center(_Cluster, vector<_Type>(_Dimension)),
            _CenterDataCount(_Cluster)
        {
        }

        template<typename _ForwardIterator, typename... _Args>
        KMeans(size_type _Cluster, _ForwardIterator __first, _ForwardIterator __last, _Args&&... __args) :
            KMeans(_Cluster, __first->size())
        {
            ResetCenterFromData(__first, __last, forward<_Args>(__args)...);
        }

        template<typename _ForwardIterator, typename... _Args>
        void ResetCenterFromData(_ForwardIterator __first, _ForwardIterator __last, _Args&&... __args)
        {
            vector<reference_wrapper<const vector<_Type>>> _RandomData(__first, __last);
            shuffle(_RandomData.begin(), _RandomData.end(), default_random_engine(forward<_Args>(__args)...));
            copy_n(_RandomData.begin(), _PrevCenter.size(), _PrevCenter.begin());
        }

        template<typename _ForwardIterator>
        auto GetDataCluster(_ForwardIterator __first, _ForwardIterator __last)
        {
            auto _FirstCenter = _PrevCenter.begin();
            auto _LastCenter = _PrevCenter.end();

            if (_FirstCenter == _LastCenter)
            {
                return typename iterator_traits<iterator>::difference_type(-1);
            }

            auto _MinDistance = SumSquares(__first, __last, _FirstCenter->begin(), _Type(0));
            auto _FoundCenter = _FirstCenter;

            while (++_FirstCenter != _LastCenter)
            {
                auto _Distance = SumSquares(__first, __last, _FirstCenter->begin(), _Type(0));
                if (_MinDistance > _Distance)
                {
                    _MinDistance = _Distance;
                    _FoundCenter = _FirstCenter;
                }
            }

            return distance(_PrevCenter.begin(), _FoundCenter);
        }

        auto GetCenter()
        {
            return _Center;
        }

        template<typename... _Args>
        bool Run(_Args&&... __args)
        {
            UpdateCenter(forward<_Args>(__args)...);
            swap(_PrevCenter, _Center);

            bool _Converged = _PrevCenter == _Center;
            return !_Converged;
        }

    private:
        vector<vector<_Type>> _PrevCenter;
        vector<vector<_Type>> _Center;
        vector<_Type> _CenterDataCount;

        template<typename _InputIterator1, typename _InputIterator2>
        static _Type SumSquares(_InputIterator1 __first1, _InputIterator1 __last1,
            _InputIterator2 __first2, _Type __init)
        {
            for (; __first1 != __last1; ++__first1, ++__first2)
            {
                _Type __minus = *__first1 - *__first2;
                __init += __minus * __minus;
            }

            return __init;
        }

        template<typename... _Args>
        static _Type EuclideanDistance(_Args&&... __args)
        {
            return sqrt(SumSquares(forward<_Args>(__args)...));
        }

        template<typename _ForwardIterator>
        void CenterAddData(_ForwardIterator __first, _ForwardIterator __last)
        {
            auto _CenterIndex = GetDataCluster(__first, __last);
            _CenterDataCount[_CenterIndex]++;

            auto __result = _Center[_CenterIndex].begin();
            for (; __first != __last; ++__first, ++__result)
            {
                *__result += *__first;
            }
        }

        template<typename _ForwardIterator, typename _OutputIterator>
        void CenterDivideCount(_ForwardIterator __first, _ForwardIterator __last, _OutputIterator __result)
        {
            for (; __first != __last; ++__first, ++__result)
            {
                const auto _Count = *__first;
                for (auto& _Value : *__result)
                {
                    _Value /= _Count;
                }
            }
        }

        template<typename _ForwardIterator>
        void UpdateCenter(_ForwardIterator __first, _ForwardIterator __last)
        {
            // Center set zero
            for (auto& _vCenter : _Center)
            {
                fill(_vCenter.begin(), _vCenter.end(), _Type(0));
            }
            fill(_CenterDataCount.begin(), _CenterDataCount.end(), _Type(0));

            // Assignment and Update
            for (; __first != __last; ++__first)
            {
                CenterAddData(__first->begin(), __first->end());
            }

            CenterDivideCount(_CenterDataCount.begin(), _CenterDataCount.end(), _Center.begin());
        }
    };
}
