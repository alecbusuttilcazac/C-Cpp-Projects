// "interface" which all ML algorithms (and methods) must adhere to

#include <iostream>
#include <limits>
#include <sstream>

#include "TSPSolver.hpp"


TSPSolver::TSPSolver(
    uint32_t seed, const DistanceMatrix& matrix, uint8_t verbosity, 
    std::ostream& stream, FN_InitialiseTour fn_it
)
:   matrix(matrix),
    verbosity(verbosity),
    out(&stream),
    fn_initialiseTour(fn_it)
{
    // Set seed
    if(seed == RANDOM_SEED)
        rng.seed(std::random_device{}());
    else
        rng.seed(seed);
}

TSPSolver::~TSPSolver() = default;

TSPSolver::Builder& TSPSolver::Builder::setSeed(uint32_t seed){
    this->RNGSeed = seed;
    return *this;
}

TSPSolver::Builder& TSPSolver::Builder::setDistanceMatrix(const DistanceMatrix& matrix){
    distanceMatrix = matrix;
    return *this;
}

TSPSolver::Builder& TSPSolver::Builder::setVerbosity(uint8_t level){
    verbosity = level;
    return *this;
}

TSPSolver::Builder& TSPSolver::Builder::setLogOutput(std::ostream& stream){
    out = &stream;
    return *this;
}

TSPSolver::Builder& TSPSolver::Builder::setFN_initialiseTour(FN_InitialiseTour fn){
    fn_initialiseTour = fn;
    return *this;
}

double TSPSolver::calculateTourDistance(
    const std::vector<uint32_t>& tour, const DistanceMatrix& distanceMatrix
){
    double totalDistance = 0.0;
    size_t n = tour.size();
    if(n == 0)
        return -1;
    
    // Sum distances between consecutive cities
    for(size_t i = 0; i + 1 < n; ++i){
        uint32_t fromCity = tour[i];
        uint32_t toCity = tour[i + 1];
        totalDistance += distanceMatrix[fromCity][toCity];
    }
    
    // Add distance from last city back to first (complete the cycle)
    uint32_t lastCity = tour[n - 1];
    uint32_t firstCity = tour[0];
    totalDistance += distanceMatrix[lastCity][firstCity];
    
    return totalDistance;
}

Tour TSPSolver::default_nearestNeighbour(
    const DistanceMatrix& distanceMatrix, std::mt19937& rng, uint32_t startingCity
){
    /*  Default NN implementation:
        ->  choose starting city AT RANDOM
        ->  repeat until all cities are visited
        ->  return tour struct 
    */
    
    Tour tour;
    size_t numCities = distanceMatrix.size();
    std::vector<bool> visited(numCities, false);
    tour.tour.resize(numCities);
    
    // Randomly generate starting city
    startingCity = std::uniform_int_distribution<int32_t>(
        0, distanceMatrix.size() - 1
    )(rng);
    
    tour.tour[0] = startingCity;
    visited[startingCity] = true;
    
    for(size_t i = 1; i < numCities; i++){
        const int lastCity = tour.tour[i-1];
        int nearestCity = -1;
        double minDistance = std::numeric_limits<double>::max();
        
        // Find nearest unvisited city
        for(size_t j = 0; j < numCities; j++){
            if(!visited[j] && distanceMatrix[lastCity][j] < minDistance){
                minDistance = distanceMatrix[lastCity][j];
                nearestCity = j;
            }
        }
        
        tour.tour[i] = nearestCity;
        visited[nearestCity] = true;
    }
    
    tour.cost = calculateTourDistance(tour.tour, distanceMatrix);
    return tour;
}