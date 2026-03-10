#include "SA.hpp"

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>
#include <chrono>
#include <iostream>
#include <sstream>



// ===============================================================================
// ----------------------------< METHOD DEFINITIONS >-----------------------------
// ===============================================================================

SimulatedAnnealing::SimulatedAnnealing(
    uint32_t RNGSeed,               const DistanceMatrix& matrix, 
    uint8_t verbosity,              uint8_t timingMode,
    std::ostream& out,              SA_Config cfg,
    
    FN_AcceptNeighbour fn_an,       FN_CreateNeighbour fn_cn, 
    FN_CoolTemperature fn_ct,       FN_StopAlgorithm fn_sa,
    FN_InitialiseTour fn_it,        FN_SetStartCity fn_sc
)
:   TSPSolver(RNGSeed, matrix, verbosity, timingMode, out, fn_it, fn_sc),
    hyperparams(cfg),
    fn_acceptNeighbour(fn_an),
    fn_createNeighbour(fn_cn),
    fn_coolTemperature(fn_ct),
    fn_stopAlgorithm(fn_sa)
{}

SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setHyperparameters
(const Config& config){
    const auto* saConfig = dynamic_cast<const SA_Config*>(&config);
    if(saConfig)
        hyperparams = *saConfig;
    return *this;
}

// SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setSeed(uint32_t seed){
//     TSPSolver::Builder::setSeed(seed);
//     return *this;
// }

// SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setDistanceMatrix(
//     const DistanceMatrix& matrix
// ){
//     TSPSolver::Builder::setDistanceMatrix(matrix);
//     return *this;
// }

// SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setVerbosity(uint8_t level){
//     TSPSolver::Builder::setVerbosity(level);
//     return *this;
// }

// SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setLogOutput
// (std::ostream& stream){
//     TSPSolver::Builder::setLogOutput(stream);
//     return *this;
// }

// SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setFN_initialiseTour
// (FN_InitialiseTour fn){
//     TSPSolver::Builder::setFN_initialiseTour(fn);
//     return *this;
// }

SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setFN_acceptNeighbour
(FN_AcceptNeighbour fn){
    fn_acceptNeighbour = fn;
    return *this;
}

SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setFN_createNeighbour
(FN_CreateNeighbour fn){
    fn_createNeighbour = fn;
    return *this;
}

SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setFN_coolTemperature
(FN_CoolTemperature fn){
    fn_coolTemperature = fn;
    return *this;
}

SimulatedAnnealing::Builder& SimulatedAnnealing::Builder::setFN_stopAlgorithm
(FN_StopAlgorithm fn){
    fn_stopAlgorithm = fn;
    return *this;
}

std::unique_ptr<TSPSolver> SimulatedAnnealing::Builder::build(){
    return std::unique_ptr<TSPSolver>(
        new SimulatedAnnealing(
            RNGSeed,                    distanceMatrix,
            verbosity,                  timingMode,
            *out,                       hyperparams, 
            
            fn_acceptNeighbour,         fn_createNeighbour,
            fn_coolTemperature,         fn_stopAlgorithm,
            fn_initialiseTour,          fn_setStartCity
        )
    );
}

const Result SimulatedAnnealing::execute(){
    auto startTime = std::chrono::steady_clock::now();
    std::ostringstream oss;
    
    size_t numCities = matrix.size();
    Result result;
    Tour currentTour;
    Tour bestTour;
    uint32_t outerIter = 0;
    double temperature = hyperparams.initialTemperature;
    double bestCostPreviousIter = INFINITY;
    uint32_t itersSinceBestImprovement = 0;
    double elapsed = -1.0;
    bool stoppingAlgorithm = false;
    
    // by default will randomise the starting city
    currentTour = fn_initialiseTour(matrix, rng, fn_selectStartCity(matrix, rng));
    bestTour = currentTour;
    result.solution = currentTour;
    
    log(2, oss << "Initial Tour = " << currentTour.tour);
    
    do{
        if(outerIter % 10 == 0)
            log(2, "outerIter=", outerIter, ", TEMPERATURE=", temperature);
        else
            log(3, "outerIter=", outerIter, ", TEMPERATURE=", temperature);
        
        for(int innerIter = 0; innerIter < hyperparams.iterationsPerTemperature; innerIter++){
            Tour neighbour;
            neighbour.tour = fn_createNeighbour(
                currentTour, static_cast<uint32_t>(numCities), temperature, this->rng,
                outerIter, static_cast<uint32_t>(innerIter)
            );
            neighbour.cost = calculateTourDistance(neighbour.tour, matrix);
            
            // Evaluate neighbour and possibly accept
            bool acceptNeighbour = fn_acceptNeighbour(
                currentTour.cost, neighbour.cost, temperature, this->rng);
            if(acceptNeighbour){
                currentTour = neighbour;
                if(neighbour.cost < bestTour.cost){
                    bestTour = neighbour;
                    itersSinceBestImprovement = 0;
                    bestCostPreviousIter = bestTour.cost;
                }
            }
            
            log(4, "    NeighbourCost=", neighbour.cost, 
                " Accepted=", acceptNeighbour ? "Y" : "N");
            
            if(timingMode == 2){
                // calculate elapsed time in seconds
                elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(
                    std::chrono::steady_clock::now() - startTime
                ).count();
                
                stoppingAlgorithm = fn_stopAlgorithm(
                    hyperparams,            temperature,
                    outerIter,              bestTour.cost, 
                    bestCostPreviousIter,   itersSinceBestImprovement, 
                    elapsed
                );
                
                if(stoppingAlgorithm)
                    break;
            }
        }
        
        if(stoppingAlgorithm)
            break;
        
        bestCostPreviousIter = bestTour.cost;
        itersSinceBestImprovement++;
        temperature = fn_coolTemperature(temperature, outerIter, hyperparams, rng);
        
        if(outerIter % 10 == 0)
            log(2, "        BestTourCost=", bestTour.cost);
        else
            log(3, "        BestTourCost=", bestTour.cost);
        log(4, oss << "        BestTour=" << bestTour.tour);
        
        if(timingMode == 1) // calculate elapsed time in seconds
            elapsed = std::chrono::duration_cast<std::chrono::duration<double>>
                (std::chrono::steady_clock::now() - startTime).count();
        
        stoppingAlgorithm = fn_stopAlgorithm(
            hyperparams,                temperature, 
            outerIter,                  bestTour.cost, 
            bestCostPreviousIter,       itersSinceBestImprovement, 
            elapsed // -1 if no stopping condition is indicated
        );
        
        outerIter++;
    }while(!stoppingAlgorithm);
    
    auto endTime = std::chrono::steady_clock::now();
    result.solution = bestTour;
    result.iterations = outerIter;
    result.success = true;
    result.executionTime = (
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
    ).count() / 1000.0;
    
    log(1, oss << "BestTour=" << bestTour.tour);
    log(1, oss << "BestTourCost=" << bestTour.cost);
    
    return result;
}




bool default_stopAlgorithm(
    const SA_Config& config, double currentTemperature, 
    OPTIONAL uint32_t outerIteration, OPTIONAL double bestCostSoFar,
    OPTIONAL double bestCostPreviousIter, 
    OPTIONAL uint32_t itersSinceBestImprovement,
    OPTIONAL double elapsedTime
){
    return currentTemperature <= config.minimumTemperature;
}

double default_coolTemperature(
    double currentTemperature, OPTIONAL uint32_t outerIteration,
    OPTIONAL const SA_Config& config, OPTIONAL std::mt19937& rng)
{
    return currentTemperature * config.coolingRate;
}

bool default_acceptNeighbour(double candidateCost, double neighbourCost, 
    double temperature, OPTIONAL std::mt19937& rng
){    
    if(neighbourCost < candidateCost)
        return true;
    
    return std::uniform_real_distribution<>(0.0, 1.0)(rng) 
           < std::exp(-(neighbourCost - candidateCost) / temperature);
}

std::vector<uint32_t> default_createNeighbour(
    const Tour& currentTour, uint32_t numCities, OPTIONAL double temperature, 
    OPTIONAL std::mt19937& rng, OPTIONAL uint32_t outerIteration,
    OPTIONAL uint32_t innerIteration
){
    std::vector<uint32_t> neighbour = currentTour.tour;
    if(std::uniform_real_distribution<>(0.0, 1.0)(rng) < 0.7){
        int pos1 = std::uniform_int_distribution<>(0, static_cast<int>(numCities) - 2)(rng);
        int pos2 = std::uniform_int_distribution<>(pos1 + 1, static_cast<int>(numCities) - 1)(rng);
        std::reverse(neighbour.begin() + pos1, neighbour.begin() + pos2 + 1);
    }else{
        int city1 = std::uniform_int_distribution<>(0, static_cast<int>(numCities) - 1)(rng);
        int city2;
        do city2 = std::uniform_int_distribution<>(0, static_cast<int>(numCities) - 1)(rng);
        while(city1 == city2);
        std::swap(neighbour[city1], neighbour[city2]);
    }
    
    return neighbour;
}