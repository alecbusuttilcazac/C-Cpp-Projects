// "interface" which all ML algorithms (and methods) must adhere to

#pragma once

#include <functional>
#include <random>

#include "../include/Utils.hpp"

#define OPTIONAL [[maybe_unused]]


//  std::function FN_InitialiseTour:
//      called when tours are to be initialised.

using FN_InitialiseTour = std::function<
Tour( // returns initialised tour
    const DistanceMatrix&, // DistanceMatrix reference
    std::mt19937&, // rng reference
    uint32_t // starting city 
)>;

using FN_SetStartCity = std::function<
uint32_t( // returns starting city
    OPTIONAL const DistanceMatrix&, // distance matrix reference
    OPTIONAL std::mt19937& // rng reference
)>;
uint32_t default_setStartCity(
    OPTIONAL const DistanceMatrix& matrix, OPTIONAL std::mt19937& rng
);


static constexpr uint32_t RANDOM_SEED = 0; // random by default


//  struct Config:
//      ALL hyperparameter (config) structs MUST inherit this and implement
//      all 3 static functions initialising default values accordingly.

struct Config{
    virtual ~Config() = default;
    static Config fast(); // faster, but poorer performance
    static Config balanced(); // efficient and somewhat optimal
    static Config thorough(); // slower, but best performance
};


//  TSPSolver:
//      ALL solvers MUST inherit this and at least implement:
//          ->  ~TSPSolver() [Destructor]
//          ->  execute() [runs algorithm]
//          ->  Respective Builder class
//      This is similar to a Java interface/contract.
//      Default values for variables are set in the Builder class. Adjacent
//      Comments also show the default value.

class TSPSolver{
protected:
    mutable std::mt19937 rng; // 0
    const DistanceMatrix matrix; // required
    const uint8_t verbosity; // 0
    const uint8_t timingMode; // 0
    std::ostream* out; // &std::cout
    
    FN_InitialiseTour fn_initialiseTour; // &default_nearestNeighbour
    FN_SetStartCity fn_selectStartCity; // &default_setStartCity
    
    TSPSolver(
        uint32_t seed,              const DistanceMatrix& matrix, 
        uint8_t verbosity,          uint8_t timingMode,
        std::ostream& out,          
        
        FN_InitialiseTour fn_nn,    FN_SetStartCity fn_sc
    );
    
    Tour TSPSolver::makeTour(
        const std::vector<uint32_t>& tour
    );
    static double calculateTourDistance( // not overridable, Euclidean
        const std::vector<uint32_t>& tour,     const DistanceMatrix& distanceMatrix
    );
    static Tour default_nearestNeighbour(
        const DistanceMatrix& distanceMatrix,  std::mt19937& rng, 
        uint32_t startingCity
    );
	
public:
    virtual ~TSPSolver() = 0;
    virtual const Result execute() = 0;
    
    //  member wrapper functions which automatically pass current verbosity and
    //  set output for ease of use 
    
    template<typename... Args>
    inline void log(uint8_t requiredVerbosity, Args&&... args){
        //  eg. log(2, "String: ", str);
        ::log(*out, verbosity, requiredVerbosity, args...);
    }
    
    inline void log(uint8_t requiredVerbosity, std::ostringstream& oss){
        //  eg. log(2, oss << ...)
        ::log(*out, verbosity, requiredVerbosity, oss);
        oss.clear(); // automatically clears stream
    }
    
    // >DEPRECATED
    // inline void log(uint8_t requiredVerbosity, std::ostream& stream){
    //     if(auto* oss = dynamic_cast<std::ostringstream*>(&stream)){
    //         ::log(*out, verbosity, requiredVerbosity, *oss);
    //         oss->clear();
    //     }
    // }
    
    //  wrapper function for printing tours
    inline void printTour(
        uint8_t requiredVerbosity,      std::ostream& stream, 
        const std::vector<uint32_t>& tour
    ){
        if(verbosity >= requiredVerbosity)
            ::printTour(stream, tour);
    }
    
    //  TSPSolver :: Builder:
    //      Builds the TSPSolver object using the Builder OOP design pattern
    
    class Builder{
    protected:
        uint32_t RNGSeed = RANDOM_SEED; // default = 0
        DistanceMatrix distanceMatrix; // required, no default
        uint8_t verbosity = 0;
            //  |-> [0 - 4]
        uint8_t timingMode = 0;
            //  |-> [0 - 2]
        std::ostream* out = &(std::cout);
        
        FN_InitialiseTour fn_initialiseTour = &default_nearestNeighbour;
        FN_SetStartCity fn_setStartCity = &default_setStartCity;
    
    public:
        virtual ~Builder() = default;
        
        virtual Builder& setHyperparameters(const Config& config) = 0;
        virtual Builder& setSeed(uint32_t seed);
        Builder& setDistanceMatrix(const DistanceMatrix& matrix);
        Builder& setVerbosity(uint8_t level);
        Builder& setTimingMode(uint8_t timingMode);
        Builder& setLogOutput(std::ostream& stream);
        
        Builder& setFN_initialiseTour(FN_InitialiseTour fn);
        Builder& setFN_setStartCity(FN_SetStartCity fn);
        virtual std::unique_ptr<TSPSolver> build() = 0;
    };
};