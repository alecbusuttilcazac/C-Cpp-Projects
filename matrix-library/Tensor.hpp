#pragma once

#include <vector>
#include <cstddef>
#include <stdexcept>

namespace la{

class Tensor {
private:
    std::vector<size_t> shape;
    std::vector<size_t> strides;
    std::vector<double> data;

public:
    Tensor(const std::vector<size_t>& shape) 
    :   shape(shape)
    {
        // compute total size
        size_t total = 1;
        for (size_t d : shape)
            total *= d;
        
        data.resize(total);

        // compute row-major strides
        strides.resize(shape.size());
        strides.back() = 1;
        for (int i = (int)shape.size() - 2; i >= 0; --i)
            strides[i] = strides[i + 1] * shape[i + 1];
    }

    double& operator[](const std::vector<size_t>& idx) {
        size_t offset = 0;
        for (size_t i = 0; i < idx.size(); ++i)
            offset += idx[i] * strides[i];
        return data[offset];
    }
};

}