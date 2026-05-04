#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <cstddef>
#include <string>
#include <vector>

enum class Distribution {
    Uniform,
    NearlySorted,
    Repeated
};

struct BenchmarkConfig {
    std::vector<std::size_t> nValues;
    std::vector<int> universeValues;
    std::vector<Distribution> distributions;
    std::size_t runs = 5;
    std::size_t bucketCount = 0;
    std::string outputCsvPath = "results/benchmark_results.csv";
};

Distribution parseDistribution(const std::string& name);
std::string distributionName(Distribution distribution);

BenchmarkConfig defaultBenchmarkConfig();
void runDemo();
void runVisualization(std::size_t n, int universe, Distribution distribution, std::size_t bucketCount = 0);
void runBenchmark(const BenchmarkConfig& config);

#endif
