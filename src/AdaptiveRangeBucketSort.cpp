#include "AdaptiveRangeBucketSort.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {

std::size_t bucketIndexFor(int value, int minValue, int maxValue, std::size_t bucketCount) {
    if (minValue == maxValue) {
        return 0;
    }

    const long long range = static_cast<long long>(maxValue) - static_cast<long long>(minValue) + 1LL;
    const long long offset = static_cast<long long>(value) - static_cast<long long>(minValue);
    std::size_t index = static_cast<std::size_t>((offset * static_cast<long long>(bucketCount)) / range);

    if (index >= bucketCount) {
        index = bucketCount - 1;
    }

    return index;
}

std::vector<std::pair<int, int>> buildBucketRanges(int minValue, int maxValue, std::size_t bucketCount) {
    std::vector<std::pair<int, int>> ranges;
    ranges.reserve(bucketCount);

    const long long range = static_cast<long long>(maxValue) - static_cast<long long>(minValue) + 1LL;
    const long long buckets = static_cast<long long>(bucketCount);

    for (std::size_t i = 0; i < bucketCount; ++i) {
        const long long current = static_cast<long long>(i);
        const long long startOffset = (current * range + buckets - 1LL) / buckets;
        const long long endOffset = (((current + 1LL) * range + buckets - 1LL) / buckets) - 1LL;
        const int start = static_cast<int>(static_cast<long long>(minValue) + startOffset);
        const int end = static_cast<int>(static_cast<long long>(minValue) + std::max(startOffset, endOffset));
        ranges.push_back({start, std::min(end, maxValue)});
    }

    return ranges;
}

} // namespace

std::size_t chooseBucketCount(std::size_t n, std::size_t requestedBucketCount) {
    if (requestedBucketCount > 0) {
        return requestedBucketCount;
    }
    if (n <= 1) {
        return 1;
    }
    return std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<long double>(n)))));
}

void adaptiveRangeBucketSort(std::vector<int>& values, std::size_t requestedBucketCount) {
    if (values.empty()) {
        return;
    }

    for (int value : values) {
        if (value < 0) {
            throw std::invalid_argument("Adaptive Range Bucket Sort requiere enteros no negativos.");
        }
    }

    const auto minmax = std::minmax_element(values.begin(), values.end());
    const int minValue = *minmax.first;
    const int maxValue = *minmax.second;
    const std::size_t bucketCount = chooseBucketCount(values.size(), requestedBucketCount);

    std::vector<std::vector<int>> buckets(bucketCount);

    for (int value : values) {
        buckets[bucketIndexFor(value, minValue, maxValue, bucketCount)].push_back(value);
    }

    std::size_t index = 0;
    for (std::vector<int>& bucket : buckets) {
        std::sort(bucket.begin(), bucket.end());
        for (int value : bucket) {
            values[index++] = value;
        }
    }
}

BucketTrace buildBucketTrace(const std::vector<int>& values, std::size_t requestedBucketCount) {
    BucketTrace trace;

    if (values.empty()) {
        return trace;
    }

    for (int value : values) {
        if (value < 0) {
            throw std::invalid_argument("Adaptive Range Bucket Sort requiere enteros no negativos.");
        }
    }

    const auto minmax = std::minmax_element(values.begin(), values.end());
    trace.minValue = *minmax.first;
    trace.maxValue = *minmax.second;
    trace.bucketCount = chooseBucketCount(values.size(), requestedBucketCount);
    trace.bucketSizes.assign(trace.bucketCount, 0);
    trace.bucketRanges = buildBucketRanges(trace.minValue, trace.maxValue, trace.bucketCount);

    for (int value : values) {
        ++trace.bucketSizes[bucketIndexFor(value, trace.minValue, trace.maxValue, trace.bucketCount)];
    }

    return trace;
}
