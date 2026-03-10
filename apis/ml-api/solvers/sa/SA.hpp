#pragma once

#include "../TSPSolver.hpp"

#include <cstdint>
#include <functional>
#include <chrono>



struct SA_Config : public Config{
public:
    double initialTemperature;
    double coolingRate;
    int iterationsPerTemperature;
    double minimumTemperature;
    
    static SA_Config fast(){
        SA_Config conf;
        conf.initialTemperature = 2000.0;
        conf.coolingRate = 0.985;
        conf.iterationsPerTemperature = 2000;
        conf.minimumTemperature = 0.01;
        return conf;
    }
    
    static SA_Config balanced(){
        SA_Config conf;
        conf.initialTemperature = 6500.0;
        conf.coolingRate = 0.994;
        conf.iterationsPerTemperature = 7000;
        conf.minimumTemperature = 0.5;
        return conf;
    }
    
    static SA_Config thorough(){
        SA_Config conf;
        conf.initialTemperature = 15000.0;
        conf.coolingRate = 0.9986;
        conf.iterationsPerTemperature = 20000;
        conf.minimumTemperature = 0.5;
        return conf;
    }
};



using FN_AcceptNeighbour = std::function<
bool( // returns bool reflecting if neighbour is accepted or not
    double, // cost of current candidate
    double, // cost of generated neighbour
    double, // current temperature
    std::mt19937& // rng reference
)>;
bool default_acceptNeighbour(
    double candidateCost,           double neighbourCost, 
    double temperature,             std::mt19937& rng
);


using FN_CreateNeighbour = std::function<
std::vector<uint32_t>( // returns generated neighbour tour
    const Tour&, // current (original) tour
    uint32_t, // number of cities required in tour
    double, // current temperature
    std::mt19937&, // rng reference
    uint32_t, // outer iteration of the SA algorithm (temperature iteration)
    uint32_t // inner iteration of the SA algorithm (neighbour generation iteration)
)>;
std::vector<uint32_t> default_createNeighbour(
    const Tour& currentTour,        uint32_t numCities, 
    double temperature,             std::mt19937& rng,
    uint32_t outerIteration,        uint32_t innerIteration
);


using FN_CoolTemperature = std::function<
double(
    double, // current temperature
    uint32_t, // outer iteration of the SA algorithm (temperature iteration)
    const SA_Config&, // SA hyperparameters struct reference
    std::mt19937& // rng reference
)>;
double default_coolTemperature(
    double currentTemperature,                  OPTIONAL uint32_t outerIteration,
    OPTIONAL const SA_Config& config,   OPTIONAL std::mt19937& rng
);


using FN_StopAlgorithm = std::function<bool(
    const SA_Config&, // SA hyperparameters struct reference
    double, // current temperature
    uint32_t, // outer iteration of the SA algorithm (temperature iteration)
    double, // best cost found so far
    double, // previous best cost
    uint32_t, // number of iterations since last improvement
    double  // elapsed time in seconds
)>;
bool default_stopAlgorithm(
    const SA_Config& config,
    double currentTemperature,
    OPTIONAL uint32_t outerIteration,
    OPTIONAL double bestCostSoFar,
    OPTIONAL double bestCostPreviousIter,
    OPTIONAL uint32_t itersSinceBestImprovement,
    OPTIONAL double elapsedSeconds
);



class SimulatedAnnealing : public TSPSolver{
public:
    class Builder;

protected:
    const SA_Config hyperparams;
    
    FN_AcceptNeighbour fn_acceptNeighbour;
    FN_CreateNeighbour fn_createNeighbour;
    FN_CoolTemperature fn_coolTemperature;
    FN_StopAlgorithm fn_stopAlgorithm;
    
    SimulatedAnnealing(
        uint32_t RNGSeed,           const DistanceMatrix& matrix,
        uint8_t verbosity,          uint8_t timingMode,
        std::ostream& out,          SA_Config cfg,
        
        FN_AcceptNeighbour fn_an,   FN_CreateNeighbour fn_cn,
        FN_CoolTemperature fn_ct,   FN_StopAlgorithm fn_sa,
        FN_InitialiseTour fn_it,    FN_SetStartCity fn_sc
    );
    
    friend class Builder;

public:
    ~SimulatedAnnealing() override = default;

    const Result execute() override;
    
    // BUILDER
    class Builder : public TSPSolver::Builder{
    private:
        SA_Config hyperparams = SA_Config::balanced();
        
        FN_AcceptNeighbour fn_acceptNeighbour = &default_acceptNeighbour;
        FN_CreateNeighbour fn_createNeighbour = &default_createNeighbour;
        FN_CoolTemperature fn_coolTemperature = &default_coolTemperature;
        FN_StopAlgorithm fn_stopAlgorithm = &default_stopAlgorithm;
    
    public:
        Builder& setHyperparameters(const Config& config) override;
            // Builder& setSeed(uint32_t seed) override;
            // Builder& setDistanceMatrix(const DistanceMatrix& matrix);
            // Builder& setVerbosity(uint8_t level);
            // Builder& setLogOutput(std::ostream& stream);
        Builder& setConvergenceLimit(double threshold);
        
            // Builder& setFN_initialiseTour(FN_InitialiseTour fn);
            // Builder& setFN_setStartCity(FN_SetStartCity fn);
        Builder& setFN_acceptNeighbour(FN_AcceptNeighbour fn);
        Builder& setFN_createNeighbour(FN_CreateNeighbour fn);
        Builder& setFN_coolTemperature(FN_CoolTemperature fn);
        Builder& setFN_stopAlgorithm(FN_StopAlgorithm fn);
        
        std::unique_ptr<TSPSolver> build() override;
    };
};