#include "KMeans.h"
#include <chrono>
#include <iterator>
#include <iostream>

using namespace std;
using namespace clustering;

int main(void)
{
    vector<vector<double>> MultiplicationTable =
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

    KMeans<double>::size_type _Cluster = 3;
    KMeans<double> KMS(_Cluster, MultiplicationTable.begin(), MultiplicationTable.end(),
        default_random_engine::result_type(chrono::system_clock::now().time_since_epoch().count()));

    size_t _Iterator;
    for (_Iterator = 0; _Iterator < 2000; _Iterator++)
    {
        bool _bResult = KMS.Run(MultiplicationTable.begin(), MultiplicationTable.end());

        if (!_bResult)
        {
            break;
        }
    }

    for (auto& _vData : MultiplicationTable)
    {
        auto ClusterIndex = KMS.GetDataCluster(_vData.begin(), _vData.end());
        cout << ClusterIndex << endl;
    }
    cout << endl;

    for (auto& _Center : KMS.GetCenter())
    {
        copy(_Center.begin(), _Center.end(), ostream_iterator<double>(cout, " "));
        cout << endl;
    }
    cout << endl << "Iterator: " << _Iterator << endl;

    return EXIT_SUCCESS;
}
