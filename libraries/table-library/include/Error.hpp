#pragma once

// Standard library includes
#include <stdexcept>
#include <string>

namespace error {
    
class FatalException : public std::runtime_error{
public:
    explicit FatalException(const std::string& errorMessage)
    :   std::runtime_error("FATAL CSV ERROR - " + errorMessage) {}
};

class NonFatalException : public std::runtime_error{
public:
    explicit NonFatalException(const std::string& errorMessage)
    :   std::runtime_error("NON-FATAL CSV ERROR - " + errorMessage) {}
};

} // namespace error