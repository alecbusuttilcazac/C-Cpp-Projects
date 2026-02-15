#pragma once

#include <cmath>
#include <vector>
#include <cstdint>
#include <memory>
#include <iostream>
#include <sstream>

using DistanceMatrix = std::vector<std::vector<double>>;

struct City{
    uint32_t id;
    double x, y;
};

struct Tour{
    std::vector<uint32_t> tour;
    double cost = INFINITY;
};

struct Result{
    Tour solution;
    double executionTime;
    uint32_t iterations;
    bool success;
};

inline void printTour(std::ostream& stream, const std::vector<uint32_t>& tour){
    stream << "[";
    if(!tour.empty())
        stream << tour[0];

    for(size_t i = 1; i < tour.size(); ++i)
        stream << ", " << tour[i];

    stream << "]";
}

inline std::ostream& operator<<(std::ostream& out, const std::vector<uint32_t>& tour){
    printTour(out, tour);
    return out;
}

template<typename... Args>
inline void log(std::ostream& out, uint8_t verbosity, uint8_t requiredVerbosity, Args&&... args){
    if(verbosity >= requiredVerbosity)
        (out << ... << args) << std::endl;
    out.clear();
}
inline void log(std::ostream& out, uint8_t verbosity, uint8_t requiredVerbosity, const std::string& message){
    if(verbosity >= requiredVerbosity)
        out << message << std::endl;
    out.clear();
}
inline void log(std::ostream& out, uint8_t verbosity, uint8_t requiredVerbosity, std::ostringstream& oss){
    ::log(out, verbosity, requiredVerbosity, oss.str());
    oss.str("");
    oss.clear();
}

inline DistanceMatrix buildDistanceMatrix(const std::vector<City>& cities){
    size_t n = cities.size();
    DistanceMatrix distMatrix(n, std::vector<double>(n, 0.0));
    
    // Euclidean Distance
    for(size_t i = 0; i < n; i++){
        for(size_t j = 0; j < n; j++){
            // distance from a city to itself is 0
            if(i == j){
                distMatrix[i][j] = 0.0;
            }else{
                double dx = cities[i].x - cities[j].x;
                double dy = cities[i].y - cities[j].y;
                distMatrix[i][j] = std::sqrt(dx*dx + dy*dy);
            }
        }
    }
    
    return distMatrix;
}