#pragma once

#include <vector>
#include <stdexcept>

template<typename T>
class ReducibleArray{
private:
    T** array;
    std::vector<bool> active; // track which array cells are allocated
    size_t usedCount = 0; // track how many array cells are allocated (active)

public:
    ReducibleArray();
    ReducibleArray(std::initializer_list<T> list);
    ReducibleArray(size_t newSize);
    // initialises size newSize with default value
    ReducibleArray(size_t newSize, const T& defaultValue); 
    
    ReducibleArray(const ReducibleArray& other); // copy constructor
    ReducibleArray& operator=(const ReducibleArray& other); // copy assignment
    
    ReducibleArray(ReducibleArray&& other) noexcept; // move constructor
    ReducibleArray& operator=(ReducibleArray&& other) noexcept; // move assignment
    
    ~ReducibleArray(); // destructor
    
    class Iterator{
    private:
        ReducibleArray* array_ptr; // ptr to parent
        size_t index = 0; // current index
    
    public:
        Iterator(ReducibleArray* ptr, size_t idx);
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        T& operator*();
    };
    
    class ConstIterator {
    private:
        const ReducibleArray* array_ptr; // ptr to parent
        size_t index = 0; // current index
    public:
        ConstIterator(const ReducibleArray* ptr, size_t idx);
        ConstIterator& operator++();
        bool operator==(const ConstIterator& other) const;
        bool operator!=(const ConstIterator& other) const;
        const T& operator*() const;
    };
    
    class ActiveIterator{
    private:
        ReducibleArray* array_ptr; // ptr to parent
        size_t index = 0; // current index
    
    public:
        ActiveIterator(ReducibleArray* ptr, size_t idx);
        ActiveIterator& operator++();
        bool operator==(const ActiveIterator& other) const;
        bool operator!=(const ActiveIterator& other) const;
        T& operator*();
    };
    
    class ConstActiveIterator {
    private:
        const ReducibleArray* array_ptr; // ptr to parent
        size_t index = 0; // current index
    public:
        ConstActiveIterator(const ReducibleArray* ptr, size_t idx);
        ConstActiveIterator& operator++();
        bool operator==(const ConstActiveIterator& other) const;
        bool operator!=(const ConstActiveIterator& other) const;
        const T& operator*() const;
    };

    // iterate over whole array
    Iterator begin(); // first element
    Iterator last(); // last element
    Iterator end(); // past last element
    ConstIterator begin() const;
    ConstIterator last() const;
    ConstIterator end() const;

    // only iterate over active (used) cells
    ActiveIterator activeBegin(); // first active element
    ActiveIterator activeLast(); // last active element
    ActiveIterator activeEnd(); // past last element
    ConstActiveIterator activeBegin() const;
    ConstActiveIterator activeLast() const;
    ConstActiveIterator activeEnd() const;
    
    // shrink array to fit active (used) size
    ReducibleArray<T> reducedCopy(std::vector<size_t>& map) const;
    ReducibleArray<T> reducedCopy() const;
    void reduce(std::vector<size_t>& map);
    void reduce();
    
    // swap two ReducibleArrays
    void swap(ReducibleArray& other);
    
    bool isFull() const;
    size_t size() const; // original capacity
    size_t usedSize() const; // acual active (used) size
    size_t memorySize() const; // in bytes
    
    void emplace(size_t index, const T& item); // copy item to index
    size_t emplace(const T& item); // copy item to first free index
    void emplace(size_t index, T&& item); // move item to index
    size_t emplace(T&& item); // move item to first free index
    
    Iterator find(const T& value); // find first matching item
    ConstIterator find(const T& value) const;
    
    void reserve(size_t size);
    void resize(size_t size);
    void resize(size_t size, const T& fill = nullptr);
    void shrinkToFit();
    
    bool exists(size_t index);
    void destroy(size_t index);
    void ifDestroy(size_t index); // if exists, destroy. else, do nothing.
    void destroyArray(); // destroy all elements in the array and array itself
    void clear(); // destroy all elements in the array (but not array itself)
    void reset(); // recreate array with original capacity
    void reset(const T& defaultValue); // recreate array with original capacity and default item
    
    void ptrSwap(size_t index1, size_t index2);
    void fullSwap(size_t index1, size_t index2);
    
    // Compare values at same indexes
    bool operator==(const ReducibleArray& other);
    bool operator!=(const ReducibleArray& other);
    
    // compares used counts
    bool operator<(const ReducibleArray& other);
    bool operator>(const ReducibleArray& other);
    bool operator<=(const ReducibleArray& other);
    bool operator>=(const ReducibleArray& other);
    
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
};



//========================================================================//
//============================<ReducibleArray>============================//
//========================================================================//

template<typename T>
ReducibleArray<T>::ReducibleArray()
:   array(nullptr)
{}

template<typename T>
ReducibleArray<T>::ReducibleArray(std::initializer_list<T> list)              
:   usedCount(list.size()),
    active(list.size(), true)
{    
    array = new T*[active.size()];
    size_t i = 0;
    for(const auto& val : list)
        array[i++] = new T(val);
}

template<typename T> 
ReducibleArray<T>::ReducibleArray(size_t newSize)
:   usedCount(0),
    active(newSize, false)
{    
    array = new T*[newSize];
}

template<typename T> 
ReducibleArray<T>::ReducibleArray(size_t newSize, const T& defaultValue)
:   usedCount(newSize),
    active(newSize, true)
{    
    array = new T*[newSize];
    for(size_t i=0; i<newSize; i++)
        array[i] = new T(defaultValue);
}

template<typename T> 
ReducibleArray<T>::ReducibleArray(const ReducibleArray& other)
:   usedCount(other.usedCount),
    active(other.active)
{    
    array = new T*[active.size()];
    for(size_t i=0; i<active.size(); i++)
        if(active[i] && other.array[i])
            array[i] = new T(*(other.array[i]));
        else{
            array[i] = nullptr;
            active[i] = false;
        }
}

// Copy assignment
template<typename T>
ReducibleArray<T>& ReducibleArray<T>::operator=(const ReducibleArray& other){
    if(this != &other){
        if(array){
            for (size_t i = 0; i < active.size(); ++i)
                delete array[i];
            delete[] array;
        }
        
        usedCount = other.usedCount;
        active = other.active;
        
        array = new T*[active.size()];
        for(size_t i=0; i<active.size(); i++)
            if(active[i] && other.array[i])
                array[i] = new T(*(other.array[i]));
            else{
                array[i] = nullptr;
                active[i] = false;
            }
    }
    return *this;
}

template<typename T> 
ReducibleArray<T>::ReducibleArray(ReducibleArray&& other) noexcept{
    if(array){
        for(size_t i = 0; i < active.size(); i++)
            delete array[i];
        delete[] array;
    }
    
    usedCount = other.usedCount;
    active = std::move(other.active);
    array = other.array;
    other.array = nullptr;
}

// Move assignment
template<typename T>
ReducibleArray<T>& ReducibleArray<T>::operator=(ReducibleArray&& other) noexcept {
    if(this != &other){
        if(array){
            for(size_t i=0; i<active.size(); i++)
                delete array[i];
            delete[] array;
        }
        
        usedCount = other.usedCount;
        active = std::move(other.active);
        array = other.array;
        other.array = nullptr;
    }
    return *this;
}

template<typename T>
ReducibleArray<T>::~ReducibleArray(){
    if(array){
        for(size_t i=0; i<active.size(); i++){
            delete array[i];
            array[i] = nullptr;
        }
        delete[] array;
        array = nullptr;
    }
}

template<typename T>
ReducibleArray<T> ReducibleArray<T>::reducedCopy() const{
    ReducibleArray<T> newArray(usedCount);
        
    size_t newIdx = 0;
    for(size_t oldIdx = 0; oldIdx < active.size(); ++oldIdx){
        if(active[oldIdx]){
            newArray.array[newIdx] = new T(*(array[oldIdx]));
            newArray.active[newIdx] = true;
            newArray.usedCount++;
            
            newIdx++;
        }
    }
    
    if(newIdx == usedCount )
        return newArray;
    else
        throw std::runtime_error("Internal error during reducedCopy: usedCount mismatch");
}

template<typename T> // map will be overwritten
ReducibleArray<T> ReducibleArray<T>::reducedCopy(std::vector<size_t>& map) const{
    ReducibleArray<T> newArray(usedCount);
    map.resize(usedCount);
    
    size_t newIdx = 0;
    for(size_t oldIdx = 0; oldIdx < active.size(); ++oldIdx){
        if(active[oldIdx]){
            newArray.array[newIdx] = new T(*(array[oldIdx]));
            newArray.active[newIdx] = true;
            newArray.usedCount++;
            
            map[newIdx++] = oldIdx;
        }
    }
    
    if(newIdx == usedCount)
        return newArray;
    else
        throw std::runtime_error("Internal error during reducedCopy: usedCount mismatch");
}

template<typename T>
void ReducibleArray<T>::reduce(){
    T** oldArray = std::move(array);
    array = new T*[usedCount];
        
    size_t newIdx = 0;
    for(size_t oldIdx = 0; oldIdx < active.size(); ++oldIdx){
        if(active[oldIdx]){
            array[newIdx] = new T(*(oldArray[oldIdx]));
            newIdx++;
        }
        delete oldArray[oldIdx];
    }
    delete[] oldArray;
    
    if(newIdx != usedCount )
        throw std::runtime_error("Internal error during reducedCopy: usedCount mismatch");

    active.resize(usedCount, true);
}

template<typename T>
void ReducibleArray<T>::reduce(std::vector<size_t>& map){
    T** oldArray = std::move(array);
    array = new T*[usedCount];
    map.resize(usedCount);
        
    size_t newIdx = 0;
    for(size_t oldIdx = 0; oldIdx < active.size(); ++oldIdx){
        if(active[oldIdx]){
            array[newIdx] = new T(*(oldArray[oldIdx]));
            map[newIdx++] = oldIdx;
        }
        delete oldArray[oldIdx];
    }
    delete[] oldArray;
    
    if(newIdx != usedCount )
        throw std::runtime_error("Internal error during reducedCopy: usedCount mismatch");

    active.resize(usedCount, true);
}

template<typename T>
void ReducibleArray<T>::swap(ReducibleArray& other) {
    std::swap(usedCount, other.usedCount);
    std::swap(active, other.active);
    std::swap(array, other.array);
}

template<typename T> 
bool ReducibleArray<T>::isFull() const{
    return usedCount == active.size();
}

template<typename T> 
size_t ReducibleArray<T>::size() const{
    return active.size();
}

template<typename T> 
size_t ReducibleArray<T>::usedSize() const{
    return usedCount;
}

template<typename T> 
size_t ReducibleArray<T>::memorySize() const{ // in bytes
    size_t total = sizeof(*this);
    total += sizeof(T*) * active.size();
    total += sizeof(bool) * active.capacity();
    
    for(unsigned int i = 0; i < active.size(); ++i)
        if(active[i] && array[i])
            total += sizeof(T);
    
    return total;
}

template<typename T> 
void ReducibleArray<T>::emplace(size_t index, const T& item){
    if(!active[index]){
        array[index] = new T(item);
        active[index] = true;
        usedCount++;
    }else
        *(array[index]) = item;
}

template<typename T> 
size_t ReducibleArray<T>::emplace(const T& item){
    if(active.size() == usedCount)
        throw std::runtime_error("ReducibleArray is at full static capacity: cannot emplace new item");
    
    for(size_t i=0; i<active.size(); i++){
        if(!active[i]){
            array[i] = new T(item);
            active[i] = true;
            usedCount++;
            return i;
        }
    }
    
    throw std::runtime_error("Internal mismatch error between usedCount and actual used count");
}

template<typename T> 
void ReducibleArray<T>::emplace(size_t index, T&& item){
    if(!active[index]){
        array[index] = new T(std::move(item));
        active[index] = true;
        usedCount++;
    }else
        *(array[index]) = std::move(item);
}

template<typename T> 
size_t ReducibleArray<T>::emplace(T&& item){
    if(active.size() == usedCount)
        throw std::runtime_error("ReducibleArray is at full static capacity: cannot emplace new item");
    
    for(size_t i=0; i<active.size(); i++){
        if(!active[i]){
            array[i] = new T(std::move(item));
            active[i] = true;
            usedCount++;
            return i;
        }
    }
    
    throw std::runtime_error("Internal mismatch error between usedCount and actual used count");
}

template<typename T> 
void ReducibleArray<T>::resize(size_t size){
    resize(size, T());
}

template<typename T> 
void ReducibleArray<T>::resize(size_t size, const T& fill){
    if(size == active.size())
        return;

    T** oldArray = array;
    size_t oldSize = active.size();
    array = new T*[size];

    if(size > oldSize){
        // Copy old pointers to new array
        for(size_t i=0; i<oldSize; i++){
            if(active[i])
                array[i] = oldArray[i];
            else
                array[i] = nullptr;
        }
        // New elements set to fill value and marked active
        for(size_t i=oldSize; i<size; i++){
            array[i] = new T(fill);
        }
        // Update active vector: preserve old activeness, mark new as active
        active.resize(size, false);
        for(size_t i=oldSize; i<size; i++)
            active[i] = true;
        usedCount += (size - oldSize);
        delete[] oldArray;
    }
    else{ // size < oldSize
        // Copy old pointers to new array, preserve activeness
        size_t newUsedCount = 0;
        for(size_t i=0; i<size; i++){
            if(active[i]){
                array[i] = oldArray[i];
                newUsedCount++;
            }else
                array[i] = nullptr;
        }
        // Delete objects in slots being removed
        for(size_t i=size; i<oldSize; i++){
            if(active[i] && oldArray[i]){
                delete oldArray[i];
            }
        }
        delete[] oldArray;
        active.resize(size);
        usedCount = newUsedCount;
    }
}

template<typename T> 
void ReducibleArray<T>::reserve(size_t size){
    if(size > active.size()){
        T** oldArray = array;
        array = new T*[size];
            
        // copy old pointers to new array
        for(size_t i=0; i<active.size(); i++){
            if(active[i])
                array[i] = oldArray[i];
            else
                array[i] = nullptr;
        }
        delete[] oldArray;
        oldArray = nullptr;
        
        // New elements set to inactive
        for(size_t i=active.size(); i<size; i++)
            array[i] = nullptr;
        active.resize(size, false);
    }
}


template<typename T> 
void ReducibleArray<T>::shrinkToFit(){
    T** oldArray = array;
    size_t oldSize = active.size();
    array = new T*[usedCount];

    size_t newIndex = 0;
    for(size_t oldIndex=0; oldIndex<active.size(); oldIndex++){
        if(active[oldIndex]){
            array[newIndex++] = oldArray[oldIndex];
        }
    }
    
    delete[] oldArray;
    active.resize(usedCount, true);
}

template<typename T> 
bool ReducibleArray<T>::exists(size_t index){
    if(index >= active.size())
        throw std::runtime_error("Index out of bounds");
    return active[index];
}

template<typename T> 
void ReducibleArray<T>::destroy(size_t index){
    if(index >= active.size())
        throw std::runtime_error("Index out of bounds");
    if(!active[index])
        throw std::runtime_error("Index already inactive");
    
    delete array[index];    // delete object pointed to by ptr
    array[index] = nullptr; // clear memory safely
    active[index] = false;    // mark as free to prevent double free
    usedCount--;            // one less array cell is now used
}

template<typename T> 
void ReducibleArray<T>::ifDestroy(size_t index){
    if(index >= active.size())
        throw std::runtime_error("Index out of bounds");
    if(active[index]){
        delete array[index];    // delete object pointed to by ptr
        array[index] = nullptr; // clear memory safely
        active[index] = false;    // mark as free to prevent double free
        usedCount--;            // one less array cell is now used
    }
}

template<typename T> 
void ReducibleArray<T>::destroyArray(){
    if(!array)
        throw std::runtime_error("Array already inactive");
    
    for(size_t i=0; i<active.size(); i++){
        delete array[i];    // delete object pointed to by ptr
        array[i] = nullptr; // clear memory safely
        active[i] = false;    // mark as free to prevent double free
    }
    usedCount = 0;            // one less array cell is now used
    
    delete[] array;
    array = nullptr;
}

template<typename T> 
void ReducibleArray<T>::clear(){
    for(size_t i=0; i<active.size(); i++){
        delete array[i];
        array[i] = nullptr;
        active[i] = false;
    }
    usedCount = 0;
}

template<typename T> 
void ReducibleArray<T>::reset(){
    if(array){
        for(size_t i=0; i<active.size(); i++){
            delete array[i];
            array[i] = nullptr;
        }
        delete[] array;
        array = nullptr;
    }
    
    usedCount = 0;
    active = std::vector<bool>(usedCount, false);
    array = new T*[usedCount];
}

template<typename T> 
void ReducibleArray<T>::reset(const T& defaultValue){
    if(!array)
        array = new T*[active.size()];
    usedCount = active.size();
    
    for(size_t i=0; i<active.size(); i++){
        if(active[i])
            *(array[i]) = defaultValue;
        else{
            delete array[i];
            array[i] = new T(defaultValue);
        }
        
        active[i] = true;
    }
}

template<typename T> 
void ReducibleArray<T>::ptrSwap(size_t index1, size_t index2){
    if(index1 >= active.size() || index2 >= active.size())
        throw std::runtime_error("Index out of bounds");
    if(!active[index1] || !active[index2])
        throw std::runtime_error("Specified index has been deallocated");
    
    if(index1 != index2){
        T* temp = array[index1];
        array[index1] = array[index2];
        array[index2] = temp;
    }
}

template<typename T>
void ReducibleArray<T>::fullSwap(size_t index1, size_t index2) {
    if (index1 >= active.size() || index2 >= active.size())
        throw std::runtime_error("Index out of bounds");
    if(!active[index1] || !active[index2])
        throw std::runtime_error("Specified index has been deallocated");

    if (index1 == index2)
        return;
    
    if (array[index1] == nullptr && array[index2] == nullptr)
        return;
        
    // If one is nullptr, move/copy the other
    if (array[index1] == nullptr && array[index2] != nullptr) {
        array[index1] = new T(std::move(*array[index2]));
        delete array[index2];
        array[index2] = nullptr;
        
    } else if (array[index1] != nullptr && array[index2] == nullptr) {
        array[index2] = new T(std::move(*array[index1]));
        delete array[index1];
        array[index1] = nullptr;
        
    } else {
        std::swap(*array[index1], *array[index2]);
    }
    return;
        
    // If only index1 is active
    if (active[index1] && !active[index2]) {
        if (array[index1] != nullptr) {
            if (array[index2] != nullptr) {
                *array[index2] = std::move(*array[index1]);
            } else {
                array[index2] = new T(std::move(*array[index1]));
            }
            delete array[index1];
            array[index1] = nullptr;
        }
        // Optionally, update activeness if you want to transfer activeness
        // std::swap(active[index1], active[index2]);
        return;
    }

    // If only index2 is active
    if (!active[index1] && active[index2]) {
        if (array[index2] != nullptr) {
            if (array[index1] != nullptr) {
                *array[index1] = std::move(*array[index2]);
            } else {
                array[index1] = new T(std::move(*array[index2]));
            }
            delete array[index2];
            array[index2] = nullptr;
        }
        // Optionally, update activeness if you want to transfer activeness
        // std::swap(active[index1], active[index2]);
        return;
    }
}

template<typename T>
bool ReducibleArray<T>::operator==(const ReducibleArray& other){
    if(active.size() != other.active.size())
        return false;
    if(active != other.active)
        return false;
    
    for(size_t i=0; i<active.size(); i++)
        if((*this)[i] != other[i])
            return false;
    return true;
}

template<typename T>
bool ReducibleArray<T>::operator!=(const ReducibleArray& other){
    return !(*this == other);
}

template<typename T>
bool ReducibleArray<T>::operator<(const ReducibleArray& other){
    return usedCount < other.usedCount;
}
template<typename T>
bool ReducibleArray<T>::operator>(const ReducibleArray& other){
    return usedCount > other.usedCount;
}
template<typename T>
bool ReducibleArray<T>::operator<=(const ReducibleArray& other){
    return usedCount <= other.usedCount;
}
template<typename T>
bool ReducibleArray<T>::operator>=(const ReducibleArray& other){
    return usedCount >= other.usedCount;
}

template<typename T> 
T& ReducibleArray<T>::operator[](size_t index){
    if(index >= active.size())
        throw std::runtime_error("Index out of bounds");
    if(!active[index])
        throw std::runtime_error("Specified index has been deallocated");
    
    return *(array[index]);
}

template<typename T> 
const T& ReducibleArray<T>::operator[](size_t index) const{
    if(index >= active.size())
        throw std::runtime_error("Index out of bounds");
    if(!active[index])
        throw std::runtime_error("Specified index has been deallocated");
    
    return *(array[index]);
}

template<typename T>
typename ReducibleArray<T>::Iterator ReducibleArray<T>::begin() {
    return Iterator(this, 0);
}

template<typename T>
typename ReducibleArray<T>::Iterator ReducibleArray<T>::end() {
    return Iterator(this, size());
}
template<typename T>
typename ReducibleArray<T>::ConstIterator ReducibleArray<T>::begin() const {
    return ConstIterator(this, 0);
}
template<typename T>
typename ReducibleArray<T>::ConstIterator ReducibleArray<T>::end() const {
    return ConstIterator(this, size());
}


template<typename T>
typename ReducibleArray<T>::ActiveIterator ReducibleArray<T>::activeBegin() {
    for(size_t i=0; i<active.size(); i++)
        if(active[i])
            return ActiveIterator(this, i);
    return activeEnd();
}
template<typename T>
typename ReducibleArray<T>::ConstActiveIterator ReducibleArray<T>::activeBegin() const {
    for(size_t i=0; i<active.size(); i++)
        if(active[i])
            return ConstActiveIterator(this, i);
    return activeEnd();
}

template<typename T>
typename ReducibleArray<T>::ActiveIterator ReducibleArray<T>::activeLast() {
    return ActiveIterator(this, size() - 1);
}
template<typename T>
typename ReducibleArray<T>::ConstActiveIterator ReducibleArray<T>::activeLast() const {
    return ConstActiveIterator(this, size() - 1);
}

template<typename T>
typename ReducibleArray<T>::ActiveIterator ReducibleArray<T>::activeEnd() {
    return ActiveIterator(this, size());
}
template<typename T>
typename ReducibleArray<T>::ConstActiveIterator ReducibleArray<T>::activeEnd() const {
    return ConstActiveIterator(this, size());
}

template<typename T>
typename ReducibleArray<T>::Iterator ReducibleArray<T>::last() {
    return Iterator(this, size() - 1);
}
template<typename T>
typename ReducibleArray<T>::ConstIterator ReducibleArray<T>::last() const {
    return ConstIterator(this, size() - 1);
}



//========================================================================//
//=======================<ReducibleArray::Iterator>=======================//
//========================================================================//

template<typename T> 
ReducibleArray<T>::Iterator::Iterator(ReducibleArray* ptr, size_t idx) 
:   array_ptr(ptr), 
    index(idx) 
{}

template<typename T>
typename ReducibleArray<T>::Iterator& ReducibleArray<T>::Iterator::operator++() {
    ++index;
    return *this;
}

template<typename T>
bool ReducibleArray<T>::Iterator::operator==(const typename ReducibleArray<T>::Iterator& other) const {
    return array_ptr == other.array_ptr && index == other.index;
}

template<typename T>
bool ReducibleArray<T>::Iterator::operator!=(const typename ReducibleArray<T>::Iterator& other) const {
    return !(*this == other);
}

template<typename T>
T& ReducibleArray<T>::Iterator::operator*() {
    return (*array_ptr)[index];
}



//========================================================================//
//====================<ReducibleArray::ConstIterator>=====================//
//========================================================================//

template<typename T>
ReducibleArray<T>::ConstIterator::ConstIterator(const ReducibleArray* ptr, size_t idx)
    : array_ptr(ptr), index(idx) {}

template<typename T>
typename ReducibleArray<T>::ConstIterator& ReducibleArray<T>::ConstIterator::operator++() {
    ++index;
    return *this;
}

template<typename T>
bool ReducibleArray<T>::ConstIterator::operator==(const typename ReducibleArray<T>::ConstIterator& other) const {
    return array_ptr == other.array_ptr && index == other.index;
}

template<typename T>
bool ReducibleArray<T>::ConstIterator::operator!=(const typename ReducibleArray<T>::ConstIterator& other) const {
    return !(*this == other);
}

template<typename T>
const T& ReducibleArray<T>::ConstIterator::operator*() const {
    return (*array_ptr)[index];
}



//========================================================================//
//====================<ReducibleArray::ActiveIterator>====================//
//========================================================================//

template<typename T> 
ReducibleArray<T>::ActiveIterator::ActiveIterator(ReducibleArray* ptr, size_t idx) 
:   array_ptr(ptr), 
    index(idx) 
{}

template<typename T>
typename ReducibleArray<T>::ActiveIterator& ReducibleArray<T>::ActiveIterator::operator++() {
    do ++index;
    while (index < array_ptr->size() && !(array_ptr->active[index]));
    return *this;
}

template<typename T>
bool ReducibleArray<T>::ActiveIterator::operator==(const typename ReducibleArray<T>::ActiveIterator& other) const {
    return array_ptr == other.array_ptr && index == other.index;
}

template<typename T>
bool ReducibleArray<T>::ActiveIterator::operator!=(const typename ReducibleArray<T>::ActiveIterator& other) const {
    return !(*this == other);
}

template<typename T>
T& ReducibleArray<T>::ActiveIterator::operator*() {
    return (*array_ptr)[index];
}



//========================================================================//
//=================<ReducibleArray::ConstActiveIterator>==================//
//========================================================================//

template<typename T>
ReducibleArray<T>::ConstActiveIterator::ConstActiveIterator(const ReducibleArray* ptr, size_t idx)
    : array_ptr(ptr), index(idx) {}

template<typename T>
typename ReducibleArray<T>::ConstActiveIterator& ReducibleArray<T>::ConstActiveIterator::operator++() {
    do ++index;
    while (index < array_ptr->size() && !(array_ptr->active[index]));
    return *this;
}

template<typename T>
bool ReducibleArray<T>::ConstActiveIterator::operator==(const typename ReducibleArray<T>::ConstActiveIterator& other) const {
    return array_ptr == other.array_ptr && index == other.index;
}

template<typename T>
bool ReducibleArray<T>::ConstActiveIterator::operator!=(const typename ReducibleArray<T>::ConstActiveIterator& other) const {
    return !(*this == other);
}

template<typename T>
const T& ReducibleArray<T>::ConstActiveIterator::operator*() const {
    return (*array_ptr)[index];
}