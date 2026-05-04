#include "benchmark.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void printHelp() {
    std::cout
        << "Uso:\n"
        << "  ./sort_benchmark demo\n"
        << "  ./sort_benchmark benchmark\n"
        << "  ./sort_benchmark benchmark --n 1000000 --u 1000000 --dist uniform --runs 5\n"
        << "  ./sort_benchmark visualize --n 100 --u 1000 --dist repeated\n\n"
        << "Opciones:\n"
        << "  --n <lista>          Ejemplo: 1000000 o 100000,500000\n"
        << "  --u <lista>          Ejemplo: 1000000 o 10000,100000\n"
        << "  --dist <nombre>      uniform, nearly_sorted o repeated\n"
        << "  --runs <numero>      Repeticiones por configuracion\n"
        << "  --buckets <numero>   Buckets para Adaptive Range Bucket Sort; 0 usa sqrt(n)\n"
        << "  --out <archivo.csv>  Ruta del CSV de salida\n";
}

std::string requireValue(int& index, int argc, char* argv[], const std::string& option) {
    if (index + 1 >= argc) {
        throw std::invalid_argument("Falta valor para " + option);
    }
    ++index;
    return argv[index];
}

std::vector<std::string> splitCommaList(const std::string& text) {
    std::vector<std::string> parts;
    std::stringstream stream(text);
    std::string part;

    while (std::getline(stream, part, ',')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }

    return parts;
}

std::vector<std::size_t> parseSizeList(const std::string& text) {
    std::vector<std::size_t> values;
    for (const std::string& part : splitCommaList(text)) {
        values.push_back(static_cast<std::size_t>(std::stoull(part)));
    }
    if (values.empty()) {
        throw std::invalid_argument("La lista de n no puede estar vacia.");
    }
    return values;
}

std::vector<int> parseUniverseList(const std::string& text) {
    std::vector<int> values;
    for (const std::string& part : splitCommaList(text)) {
        const long parsed = std::stol(part);
        if (parsed < 0) {
            throw std::invalid_argument("U debe ser mayor o igual que cero.");
        }
        values.push_back(static_cast<int>(parsed));
    }
    if (values.empty()) {
        throw std::invalid_argument("La lista de U no puede estar vacia.");
    }
    return values;
}

void applyBenchmarkOptions(BenchmarkConfig& config, int argc, char* argv[]) {
    for (int i = 2; i < argc; ++i) {
        const std::string option = argv[i];

        if (option == "--n") {
            config.nValues = parseSizeList(requireValue(i, argc, argv, option));
        } else if (option == "--u") {
            config.universeValues = parseUniverseList(requireValue(i, argc, argv, option));
        } else if (option == "--dist") {
            config.distributions = {parseDistribution(requireValue(i, argc, argv, option))};
        } else if (option == "--runs") {
            config.runs = static_cast<std::size_t>(std::stoull(requireValue(i, argc, argv, option)));
        } else if (option == "--buckets") {
            config.bucketCount = static_cast<std::size_t>(std::stoull(requireValue(i, argc, argv, option)));
        } else if (option == "--out") {
            config.outputCsvPath = requireValue(i, argc, argv, option);
        } else if (option == "--help" || option == "-h") {
            printHelp();
            std::exit(0);
        } else {
            throw std::invalid_argument("Opcion desconocida: " + option);
        }
    }
}

void runVisualizeMode(int argc, char* argv[]) {
    std::size_t n = 100;
    int universe = 1000;
    Distribution distribution = Distribution::Repeated;
    std::size_t bucketCount = 0;

    for (int i = 2; i < argc; ++i) {
        const std::string option = argv[i];

        if (option == "--n") {
            n = static_cast<std::size_t>(std::stoull(requireValue(i, argc, argv, option)));
        } else if (option == "--u") {
            const long parsed = std::stol(requireValue(i, argc, argv, option));
            if (parsed < 0) {
                throw std::invalid_argument("U debe ser mayor o igual que cero.");
            }
            universe = static_cast<int>(parsed);
        } else if (option == "--dist") {
            distribution = parseDistribution(requireValue(i, argc, argv, option));
        } else if (option == "--buckets") {
            bucketCount = static_cast<std::size_t>(std::stoull(requireValue(i, argc, argv, option)));
        } else if (option == "--help" || option == "-h") {
            printHelp();
            std::exit(0);
        } else {
            throw std::invalid_argument("Opcion desconocida: " + option);
        }
    }

    runVisualization(n, universe, distribution, bucketCount);
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cout << "No se indico modo. Ejecutando demo por defecto.\n\n";
            runDemo();
            return 0;
        }

        const std::string mode = argv[1];

        if (mode == "demo") {
            runDemo();
        } else if (mode == "benchmark") {
            BenchmarkConfig config = defaultBenchmarkConfig();
            applyBenchmarkOptions(config, argc, argv);
            runBenchmark(config);
        } else if (mode == "visualize") {
            runVisualizeMode(argc, argv);
        } else if (mode == "--help" || mode == "-h") {
            printHelp();
        } else {
            throw std::invalid_argument("Modo desconocido: " + mode);
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n\n";
        printHelp();
        return 1;
    }

    return 0;
}
