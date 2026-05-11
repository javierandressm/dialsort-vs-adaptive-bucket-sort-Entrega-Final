#include "benchmark.h"

#include "AdaptiveRangeBucketSort.h"
#include "DialSort.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>

namespace {

struct RunResult {
    std::size_t run = 0;
    double timeMs = 0.0;
    bool sorted = false;
};

struct BenchmarkSummaryRow {
    std::string algorithm;
    std::size_t n = 0;
    int universe = 0;
    Distribution distribution = Distribution::Uniform;
    double meanMs = 0.0;
    double stddevMs = 0.0;
    double throughput = 0.0;
    bool allSorted = false;
};

std::vector<Distribution> allDistributions() {
    return {Distribution::Uniform, Distribution::NearlySorted, Distribution::Repeated};
}

std::vector<int> generateDataset(std::size_t n, int universe, Distribution distribution, std::uint64_t seed) {
    if (universe < 0) {
        throw std::invalid_argument("U debe ser mayor o igual que cero.");
    }

    std::mt19937_64 rng(seed);
    std::vector<int> values;
    values.reserve(n);

    if (distribution == Distribution::Uniform) {
        std::uniform_int_distribution<int> dist(0, universe);
        for (std::size_t i = 0; i < n; ++i) {
            values.push_back(dist(rng));
        }
        return values;
    }

    if (distribution == Distribution::NearlySorted) {
        if (n == 0) {
            return values;
        }
        if (n == 1) {
            values.push_back(universe / 2);
            return values;
        }

        for (std::size_t i = 0; i < n; ++i) {
            const long long numerator = static_cast<long long>(i) * static_cast<long long>(universe);
            values.push_back(static_cast<int>(numerator / static_cast<long long>(n - 1)));
        }

        std::uniform_int_distribution<std::size_t> indexDist(0, n - 1);
        const std::size_t swaps = std::max<std::size_t>(1, n / 100);
        for (std::size_t i = 0; i < swaps; ++i) {
            std::swap(values[indexDist(rng)], values[indexDist(rng)]);
        }
        return values;
    }

    const std::size_t possibleValues = static_cast<std::size_t>(universe) + 1;
    const std::size_t paletteSize = std::max<std::size_t>(1, std::min<std::size_t>(32, possibleValues));
    std::uniform_int_distribution<int> valueDist(0, universe);

    std::vector<int> palette;
    palette.reserve(paletteSize);
    for (std::size_t i = 0; i < paletteSize; ++i) {
        palette.push_back(valueDist(rng));
    }
    std::sort(palette.begin(), palette.end());
    palette.erase(std::unique(palette.begin(), palette.end()), palette.end());

    std::uniform_int_distribution<std::size_t> paletteDist(0, palette.size() - 1);
    for (std::size_t i = 0; i < n; ++i) {
        values.push_back(palette[paletteDist(rng)]);
    }

    return values;
}

bool isSorted(const std::vector<int>& values) {
    return std::is_sorted(values.begin(), values.end());
}

double mean(const std::vector<double>& values) {
    if (values.empty()) {
        return 0.0;
    }

    double total = 0.0;
    for (double value : values) {
        total += value;
    }
    return total / static_cast<double>(values.size());
}

double standardDeviation(const std::vector<double>& values, double meanValue) {
    if (values.size() <= 1) {
        return 0.0;
    }

    double sum = 0.0;
    for (double value : values) {
        const double diff = value - meanValue;
        sum += diff * diff;
    }
    return std::sqrt(sum / static_cast<double>(values.size() - 1));
}

std::uint64_t seedFor(std::size_t n, int universe, Distribution distribution, std::size_t run) {
    std::uint64_t seed = 20260504ULL;
    seed = seed * 1315423911ULL + static_cast<std::uint64_t>(n);
    seed = seed * 1315423911ULL + static_cast<std::uint64_t>(universe);
    seed = seed * 1315423911ULL + static_cast<std::uint64_t>(distribution);
    seed = seed * 1315423911ULL + static_cast<std::uint64_t>(run);
    return seed;
}

double measureDialSort(std::vector<int>& values, int universe) {
    const auto start = std::chrono::high_resolution_clock::now();
    dialSort(values, universe);
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double measureBucketSort(std::vector<int>& values, std::size_t bucketCount) {
    const auto start = std::chrono::high_resolution_clock::now();
    adaptiveRangeBucketSort(values, bucketCount);
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void printVector(const std::vector<int>& values, std::size_t limit = 40) {
    std::cout << '[';
    const std::size_t displayCount = std::min(limit, values.size());
    for (std::size_t i = 0; i < displayCount; ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << values[i];
    }
    if (displayCount < values.size()) {
        std::cout << ", ...";
    }
    std::cout << "]\n";
}

std::string bar(std::size_t value, std::size_t maxValue, std::size_t width = 40) {
    if (value == 0 || maxValue == 0) {
        return "";
    }
    const std::size_t length = std::max<std::size_t>(1, value * width / maxValue);
    return std::string(length, '#');
}

void showDialVisualization(const std::vector<int>& values, int universe) {
    const DialTrace trace = buildDialTrace(values, universe);
    const std::size_t groups = std::min<std::size_t>(16, trace.counts.size());
    std::vector<std::size_t> grouped(groups, 0);

    for (std::size_t value = 0; value < trace.counts.size(); ++value) {
        const std::size_t group = std::min(groups - 1, value * groups / trace.counts.size());
        grouped[group] += trace.counts[value];
    }

    const std::size_t maxCount = *std::max_element(grouped.begin(), grouped.end());

    std::cout << "\nDialSort - conteos por valor o rango\n";
    for (std::size_t group = 0; group < groups; ++group) {
        const std::size_t start = group * trace.counts.size() / groups;
        const std::size_t end = ((group + 1) * trace.counts.size() / groups) - 1;
        std::cout << "Rango [" << std::setw(4) << start << ", " << std::setw(4) << end << "] | "
                  << bar(grouped[group], maxCount) << ' ' << grouped[group] << " elementos\n";
    }

    std::cout << "Reconstruccion ordenada, vista previa: ";
    printVector(trace.sortedPreview, 30);
}

void showBucketVisualization(const std::vector<int>& values, std::size_t bucketCount) {
    const BucketTrace trace = buildBucketTrace(values, bucketCount);
    if (trace.bucketSizes.empty()) {
        std::cout << "\nAdaptive Range Bucket Sort - no hay datos.\n";
        return;
    }

    const std::size_t maxCount = *std::max_element(trace.bucketSizes.begin(), trace.bucketSizes.end());

    std::cout << "\nAdaptive Range Bucket Sort - distribucion por buckets\n";
    std::cout << "Rango observado: [" << trace.minValue << ", " << trace.maxValue << "]\n";
    std::cout << "Buckets usados: " << trace.bucketCount << "\n";

    for (std::size_t i = 0; i < trace.bucketSizes.size(); ++i) {
        std::cout << "Bucket " << std::setw(3) << i
                  << " [" << std::setw(4) << trace.bucketRanges[i].first
                  << ", " << std::setw(4) << trace.bucketRanges[i].second
                  << "] | "
                  << bar(trace.bucketSizes[i], maxCount) << ' '
                  << trace.bucketSizes[i] << " elementos\n";
    }
}

void writeCsvRows(
    std::ofstream& csv,
    const std::string& algorithm,
    std::size_t n,
    int universe,
    Distribution distribution,
    const std::vector<RunResult>& results
) {
    std::vector<double> times;
    times.reserve(results.size());
    for (const RunResult& result : results) {
        times.push_back(result.timeMs);
    }

    const double meanMs = mean(times);
    const double stddevMs = standardDeviation(times, meanMs);
    const double throughput = meanMs > 0.0 ? static_cast<double>(n) / (meanMs / 1000.0) : 0.0;

    for (const RunResult& result : results) {
        csv << algorithm << ','
            << n << ','
            << universe << ','
            << distributionName(distribution) << ','
            << result.run << ','
            << std::fixed << std::setprecision(6) << result.timeMs << ','
            << meanMs << ','
            << stddevMs << ','
            << throughput << ','
            << (result.sorted ? "true" : "false") << '\n';
    }
}

BenchmarkSummaryRow buildSummaryRow(
    const std::string& algorithm,
    std::size_t n,
    int universe,
    Distribution distribution,
    const std::vector<RunResult>& results
) {
    std::vector<double> times;
    times.reserve(results.size());
    bool allSorted = true;
    for (const RunResult& result : results) {
        times.push_back(result.timeMs);
        allSorted = allSorted && result.sorted;
    }

    const double meanMs = mean(times);
    const double stddevMs = standardDeviation(times, meanMs);
    const double throughput = meanMs > 0.0 ? static_cast<double>(n) / (meanMs / 1000.0) : 0.0;

    return {algorithm, n, universe, distribution, meanMs, stddevMs, throughput, allSorted};
}

void printBenchmarkSummary(const std::vector<BenchmarkSummaryRow>& summaryRows) {
    if (summaryRows.empty()) {
        return;
    }

    std::cout << "\nResumen final por configuracion\n";
    for (const BenchmarkSummaryRow& row : summaryRows) {
        std::cout << "  " << row.algorithm
                  << " | n=" << row.n
                  << ", U=" << row.universe
                  << ", dist=" << distributionName(row.distribution)
                  << ", mean_ms=" << std::fixed << std::setprecision(3) << row.meanMs
                  << ", stddev_ms=" << row.stddevMs
                  << ", throughput=" << std::setprecision(0) << row.throughput
                  << " reg/s"
                  << ", sorted=" << (row.allSorted ? "true" : "false")
                  << '\n';
    }
}

} // namespace

Distribution parseDistribution(const std::string& name) {
    if (name == "uniform") {
        return Distribution::Uniform;
    }
    if (name == "nearly_sorted" || name == "nearly-sorted" || name == "casi_ordenada") {
        return Distribution::NearlySorted;
    }
    if (name == "repeated" || name == "repetidos") {
        return Distribution::Repeated;
    }
    throw std::invalid_argument("Distribucion desconocida: " + name);
}

std::string distributionName(Distribution distribution) {
    switch (distribution) {
        case Distribution::Uniform:
            return "uniform";
        case Distribution::NearlySorted:
            return "nearly_sorted";
        case Distribution::Repeated:
            return "repeated";
    }
    return "unknown";
}

BenchmarkConfig defaultBenchmarkConfig() {
    BenchmarkConfig config;
    config.nValues = {100000, 500000, 1000000, 5000000, 10000000};
    config.universeValues = {10000, 100000, 1000000, 10000000};
    config.distributions = allDistributions();
    config.runs = 5;
    return config;
}

void runDemo() {
    const int universe = 20;
    const std::vector<int> original = {12, 4, 0, 19, 4, 7, 12, 3, 2, 20, 1, 7, 15, 8, 6};

    std::cout << "Demo con arreglo pequeno\n";
    std::cout << "Original: ";
    printVector(original);

    std::vector<int> dialData = original;
    dialSort(dialData, universe);
    std::cout << "DialSort: ";
    printVector(dialData);
    std::cout << "Ordenado correctamente: " << (isSorted(dialData) ? "true" : "false") << '\n';

    std::vector<int> bucketData = original;
    adaptiveRangeBucketSort(bucketData, 5);
    std::cout << "Adaptive Range Bucket Sort: ";
    printVector(bucketData);
    std::cout << "Ordenado correctamente: " << (isSorted(bucketData) ? "true" : "false") << '\n';

    showDialVisualization(original, universe);
    showBucketVisualization(original, 5);
}

void runVisualization(std::size_t n, int universe, Distribution distribution, std::size_t bucketCount) {
    const std::vector<int> values = generateDataset(n, universe, distribution, 20260504ULL);

    std::cout << "Visualizacion interna\n";
    std::cout << "n=" << n
              << ", U=" << universe
              << ", distribucion=" << distributionName(distribution) << '\n';
    std::cout << "Datos, vista previa: ";
    printVector(values);

    showDialVisualization(values, universe);
    showBucketVisualization(values, bucketCount);
}

void runBenchmark(const BenchmarkConfig& config) {
    if (config.runs == 0) {
        throw std::invalid_argument("Las repeticiones deben ser mayores que cero.");
    }

    const std::filesystem::path outputPath(config.outputCsvPath);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    std::ofstream csv(config.outputCsvPath);
    if (!csv) {
        throw std::runtime_error("No se pudo crear el CSV de resultados.");
    }

    csv << "algorithm,n,U,distribution,run,time_ms,mean_ms,stddev_ms,throughput_records_sec,is_sorted\n";
    std::vector<BenchmarkSummaryRow> summaryRows;

    for (std::size_t n : config.nValues) {
        for (int universe : config.universeValues) {
            for (Distribution distribution : config.distributions) {
                std::vector<RunResult> dialResults;
                std::vector<RunResult> bucketResults;

                std::cout << "Configuracion: n=" << n
                          << ", U=" << universe
                          << ", dist=" << distributionName(distribution)
                          << ", runs=" << config.runs << '\n';

                for (std::size_t run = 1; run <= config.runs; ++run) {
                    const std::vector<int> dataset = generateDataset(
                        n,
                        universe,
                        distribution,
                        seedFor(n, universe, distribution, run)
                    );

                    std::vector<int> dialData = dataset;
                    const double dialMs = measureDialSort(dialData, universe);
                    dialResults.push_back({run, dialMs, isSorted(dialData)});

                    std::vector<int> bucketData = dataset;
                    const double bucketMs = measureBucketSort(bucketData, config.bucketCount);
                    bucketResults.push_back({run, bucketMs, isSorted(bucketData)});

                    std::cout << "  Run " << run
                              << ": DialSort=" << std::fixed << std::setprecision(3) << dialMs << " ms"
                              << ", AdaptiveRangeBucketSort=" << bucketMs << " ms\n";
                }

                writeCsvRows(csv, "DialSort", n, universe, distribution, dialResults);
                writeCsvRows(csv, "AdaptiveRangeBucketSort", n, universe, distribution, bucketResults);
                summaryRows.push_back(buildSummaryRow("DialSort", n, universe, distribution, dialResults));
                summaryRows.push_back(buildSummaryRow("AdaptiveRangeBucketSort", n, universe, distribution, bucketResults));
            }
        }
    }

    printBenchmarkSummary(summaryRows);
    std::cout << "Resultados guardados en " << config.outputCsvPath << '\n';
}
