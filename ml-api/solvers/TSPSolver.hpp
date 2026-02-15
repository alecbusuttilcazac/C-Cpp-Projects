// "interface" which all ML algorithms (and methods) must adhere to

#pragma once

#include <functional>
#include <random>

#include "../include/Utils.hpp"

using FN_InitialiseTour = std::function<Tour(const DistanceMatrix&, std::mt19937&, uint32_t)>;
static constexpr uint32_t RANDOM_SEED = 0;

struct Config{
    virtual ~Config() = default;
    static Config fast();
    static Config balanced();
    static Config thorough();
};

class TSPSolver{
protected:
    mutable std::mt19937 rng;
    const DistanceMatrix matrix;
    const uint8_t verbosity;
    std::ostream* out = &std::cout;
    
    FN_InitialiseTour fn_initialiseTour;
    
    TSPSolver(
        uint32_t seed, const DistanceMatrix& matrix, uint8_t verbosity, 
        std::ostream& out, FN_InitialiseTour fn_nn);
    
    static double calculateTourDistance(
        const std::vector<uint32_t>& tour, const DistanceMatrix& distanceMatrix);
    static Tour default_nearestNeighbour(
        const DistanceMatrix& distanceMatrix, std::mt19937& rng, uint32_t startingCity);
	
public:
    virtual ~TSPSolver() = 0;
    virtual const Result execute() = 0;
    
    template<typename... Args>
    inline void log(uint8_t requiredVerbosity, Args&&... args){
        ::log(*out, verbosity, requiredVerbosity, args...);
    }
    inline void log(uint8_t requiredVerbosity, std::ostringstream& oss){
        ::log(*out, verbosity, requiredVerbosity, oss);
    }
    inline void log(uint8_t requiredVerbosity, std::ostream& stream){
        if(auto* oss = dynamic_cast<std::ostringstream*>(&stream)){
            ::log(*out, verbosity, requiredVerbosity, *oss);
            oss->clear();
        }
    }
    inline void printTour(uint8_t requiredVerbosity, std::ostream& stream, const std::vector<uint32_t>& tour){
        if(verbosity >= requiredVerbosity)
            ::printTour(stream, tour);
    }
    
    // BUILDER
    class Builder{
    protected:
        uint32_t RNGSeed = RANDOM_SEED;
        DistanceMatrix distanceMatrix;
        uint8_t verbosity = 1;
        std::ostream* out = &(std::cout);
        
        FN_InitialiseTour fn_initialiseTour = &default_nearestNeighbour;
    
    public:
        virtual ~Builder() = default;
        
        virtual Builder& setHyperparameters(const Config& config) = 0;
        virtual Builder& setSeed(uint32_t seed);
        Builder& setDistanceMatrix(const DistanceMatrix& matrix);
        Builder& setVerbosity(uint8_t level);
        Builder& setLogOutput(std::ostream& stream);
        
        Builder& setFN_initialiseTour(FN_InitialiseTour fn);
        
        virtual std::unique_ptr<TSPSolver> build() = 0;
    };
};