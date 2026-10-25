#include "FitMetricService.hpp"
#include "HttpServer.hpp"
#include <iostream>
#include <memory>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            port = 8080;
        }
    }

    std::cout << "========================================================\n";
    std::cout << "           FitMetric System Initializing...             \n";
    std::cout << "   Interpretation Stability & Verification Engine (ISVE)\n";
    std::cout << "========================================================\n";
    std::cout << " Object-Oriented Principles Architecture:\n";
    std::cout << "  - Abstraction:     MetricCalculator (Abstract Base Interface)\n";
    std::cout << "  - Inheritance:     BMICalculator, BMRCalculator, TDEECalculator\n";
    std::cout << "  - Encapsulation:   MeasurementRecord, Interval, EvidenceTrace\n";
    std::cout << "  - Composition:     FitMetricService, InterpretationStabilityEngine\n";
    std::cout << "  - Polymorphism:    Dynamic metric dispatch via MetricCalculator*\n";
    std::cout << "  - Exception Model: Custom Hierarchy (FitMetricException)\n";
    std::cout << "========================================================\n";

    try {
        auto repository = std::make_shared<fitmetric::HistoryRepository>("data/fitmetric_history.json");
        auto service = std::make_shared<fitmetric::FitMetricService>(repository);

        std::string webRoot = "web";
        fitmetric::HttpServer server(port, webRoot, service);
        server.start();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error starting FitMetric: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
