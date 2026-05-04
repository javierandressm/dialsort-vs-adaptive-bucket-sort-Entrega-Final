#include "DialSort.h"

#include <algorithm>
#include <stdexcept>

namespace {

std::vector<std::size_t> buildCounts(const std::vector<int>& values, int universe) {
    if (universe < 0) {
        throw std::invalid_argument("DialSort requiere U >= 0.");
    }

    std::vector<std::size_t> counts(static_cast<std::size_t>(universe) + 1, 0);

    for (int value : values) {
        if (value < 0 || value > universe) {
            throw std::invalid_argument("DialSort recibio un valor fuera de [0, U].");
        }
        ++counts[static_cast<std::size_t>(value)];
    }

    return counts;
}

void rebuildFromCounts(std::vector<int>& values, const std::vector<std::size_t>& counts) {
    std::size_t index = 0;

    for (std::size_t value = 0; value < counts.size(); ++value) {
        for (std::size_t repetition = 0; repetition < counts[value]; ++repetition) {
            values[index++] = static_cast<int>(value);
        }
    }
}

} // namespace

void dialSort(std::vector<int>& values, int universe) {
    std::vector<std::size_t> counts = buildCounts(values, universe);
    rebuildFromCounts(values, counts);
}

DialTrace buildDialTrace(const std::vector<int>& values, int universe, std::size_t previewLimit) {
    DialTrace trace;
    trace.counts = buildCounts(values, universe);

    std::vector<int> sorted = values;
    rebuildFromCounts(sorted, trace.counts);

    const std::size_t limit = std::min(previewLimit, sorted.size());
    trace.sortedPreview.assign(sorted.begin(), sorted.begin() + static_cast<std::ptrdiff_t>(limit));

    return trace;
}
