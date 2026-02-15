#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>

#include "solvers/sa/SA.hpp"
#include "include/ReadData.hpp"

static double coolTemperature_fast(double currentTemperature, 
    [[maybe_unused]] uint32_t outerIteration, const SA_Config& config, 
    [[maybe_unused]] std::mt19937& rng
){
    constexpr double someFormula = 1.0;
    return currentTemperature * config.coolingRate * someFormula;
}

static bool stopEarlyOnStall(
    const SA_Config& config, double currentTemperature, uint32_t outerIteration,
    [[maybe_unused]] double bestCostSoFar, [[maybe_unused]] double bestCostPreviousIter,
    [[maybe_unused]] uint32_t itersSinceBestImprovement
){
    if(currentTemperature <= config.minimumTemperature)
        return true;
    return outerIteration >= 5000;
}

int main(){
    // DistanceMatrix matrix = readDoubleMatrix("data/ftv170.atsp", 8, 171);
    DistanceMatrix matrix = buildDistanceMatrix(readIntegerCSV("data/pr1002.tsp", 7, ' '));
    SA_Config hyperparams = SA_Config::balanced();
    hyperparams.minimumTemperature = 0.0005;
    const uint32_t seed = 42;

    std::unique_ptr<TSPSolver> solver = SimulatedAnnealing::Builder()
        .setSeed(seed)
        .setDistanceMatrix(matrix)
        .setVerbosity(2)
        // .setLogOutput(logFile)
        .setHyperparameters(hyperparams)
        .setFN_coolTemperature(&coolTemperature_fast)
        .setFN_stopAlgorithm(&stopEarlyOnStall)
        .build();

    auto wallStart = std::chrono::high_resolution_clock::now();
    Result result = solver->execute();
    auto wallEnd = std::chrono::high_resolution_clock::now();
    const double wallMs = std::chrono::duration_cast<std::chrono::microseconds>(
        wallEnd - wallStart).count() / 1000.0;

    // std::cout << "\nBest tour cost: " << result.solution.cost << "\n";
    std::cout << "Iterations: " << result.iterations << "\n";
    std::cout << "Execution time (ms): " << result.executionTime << "\n";
    std::cout << "Wall time (ms): " << wallMs << "\n";
    // std::cout << "Tour: " << result.solution.tour;

    return 0;
}
