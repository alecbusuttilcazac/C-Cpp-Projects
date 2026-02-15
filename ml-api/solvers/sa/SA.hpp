#pragma once

#include <cstdint>
#include <functional>
#include "../TSPSolver.hpp"

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



using FN_AcceptNeighbour = std::function<bool(double, double, double, std::mt19937&)>;
bool default_acceptNeighbour(
    double candidateCost, double neighbourCost, double temperature, std::mt19937& rng
);

using FN_CreateNeighbour = std::function<std::vector<uint32_t>(
    const Tour&, uint32_t, double, std::mt19937&, uint32_t, uint32_t
)>;
std::vector<uint32_t> default_createNeighbour(
    const Tour& currentTour, uint32_t numCities, double temperature, std::mt19937& rng,
    uint32_t outerIteration, uint32_t innerIteration
);

using FN_CoolTemperature = std::function<double(double, uint32_t, const SA_Config&, std::mt19937&)>;
double default_coolTemperature(
    double currentTemperature, [[maybe_unused]] uint32_t outerIteration,
    [[maybe_unused]] const SA_Config& config, [[maybe_unused]] std::mt19937& rng
);

using FN_StopAlgorithm = std::function<bool(
    const SA_Config&, double, uint32_t, double, double, uint32_t
)>;
bool default_stopAlgorithm(
    const SA_Config& config, double currentTemperature, 
    [[maybe_unused]] uint32_t outerIteration, [[maybe_unused]] double bestCostSoFar,
    [[maybe_unused]] double bestCostPreviousIter, 
    [[maybe_unused]] uint32_t itersSinceBestImprovement
);



class SimulatedAnnealing : public TSPSolver{
public:
    class Builder;

protected:
    const SA_Config hyperparams;
    const double convergenceThreshold;
    
    FN_AcceptNeighbour fn_acceptNeighbour;
    FN_CreateNeighbour fn_createNeighbour;
    FN_CoolTemperature fn_coolTemperature;
    FN_StopAlgorithm fn_stopAlgorithm;
    
    SimulatedAnnealing(
        uint32_t RNGSeed, const DistanceMatrix& matrix, uint8_t verbosity,
        std::ostream& out, SA_Config cfg, double threshold, FN_AcceptNeighbour fn_an, 
        FN_CreateNeighbour fn_cn, FN_CoolTemperature fn_ct, FN_StopAlgorithm fn_sa, 
        FN_InitialiseTour fn_it
    );
    
    friend class Builder;

public:
    ~SimulatedAnnealing() override = default;

    const Result execute() override;
    
    // BUILDER
    class Builder : public TSPSolver::Builder{
    private:
        SA_Config hyperparams = SA_Config::balanced();
        double convergenceThreshold = 0;
        
        FN_AcceptNeighbour fn_acceptNeighbour = &default_acceptNeighbour;
        FN_CreateNeighbour fn_createNeighbour = &default_createNeighbour;
        FN_CoolTemperature fn_coolTemperature = &default_coolTemperature;
        FN_StopAlgorithm fn_stopAlgorithm = &default_stopAlgorithm;
    
    public:
        Builder& setHyperparameters(const Config& config) override;
        Builder& setSeed(uint32_t seed) override;
        Builder& setDistanceMatrix(const DistanceMatrix& matrix);
        Builder& setVerbosity(uint8_t level);
        Builder& setLogOutput(std::ostream& stream);
        Builder& setFN_initialiseTour(FN_InitialiseTour fn);
        Builder& setConvergenceLimit(double threshold);
        
        Builder& setFN_acceptNeighbour(FN_AcceptNeighbour fn);
        Builder& setFN_createNeighbour(FN_CreateNeighbour fn);
        Builder& setFN_coolTemperature(FN_CoolTemperature fn);
        Builder& setFN_stopAlgorithm(FN_StopAlgorithm fn);
        
        std::unique_ptr<TSPSolver> build() override;
    };
};