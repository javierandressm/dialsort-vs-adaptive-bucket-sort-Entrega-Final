#ifndef ADAPTIVE_RANGE_BUCKET_SORT_H
#define ADAPTIVE_RANGE_BUCKET_SORT_H

#include <cstddef>
#include <utility>
#include <vector>

struct BucketTrace {
    int minValue = 0;
    int maxValue = 0;
    std::size_t bucketCount = 0;
    std::vector<std::size_t> bucketSizes;
    std::vector<std::pair<int, int>> bucketRanges;
};

std::size_t chooseBucketCount(std::size_t n, std::size_t requestedBucketCount = 0);
void adaptiveRangeBucketSort(std::vector<int>& values, std::size_t requestedBucketCount = 0);
BucketTrace buildBucketTrace(const std::vector<int>& values, std::size_t requestedBucketCount = 0);

#endif
