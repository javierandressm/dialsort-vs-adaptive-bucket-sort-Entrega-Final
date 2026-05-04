#ifndef DIAL_SORT_H
#define DIAL_SORT_H

#include <cstddef>
#include <vector>

struct DialTrace {
    std::vector<std::size_t> counts;
    std::vector<int> sortedPreview;
};

void dialSort(std::vector<int>& values, int universe);
DialTrace buildDialTrace(const std::vector<int>& values, int universe, std::size_t previewLimit = 30);

#endif
