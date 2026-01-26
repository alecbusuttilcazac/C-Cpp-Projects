#include <iostream>
#include <vector>
#include "ReducibleArray.hpp"

int main() {
    using namespace std;
    cout << "=== Default Construction ===" << endl;
    
    ReducibleArray<string>* arrayPtr = new ReducibleArray<string>();
    ReducibleArray<string> array = *arrayPtr;
    
    // OR 
    
    // ReducibleArray<string> array;
    
    cout << "SIZE = " << array.size() << ", USED = " << array.usedSize() << endl;

    cout << "\n=== Out-of-bounds Access ===" << endl;
    try {
        cout << array[0];
    } catch (runtime_error& e) {
        cout << e.what() << endl;
    }

    cout << "\n=== Initializer List Construction ===" << endl;
    array = ReducibleArray<string>({"str1", "str2", "str3"});
    cout << "array[1] = " << array[1] << endl;
    cout << "SIZE = " << array.size() << ", USED = " << array.usedSize() << endl;

    cout << "\n=== Freeing and Access ===" << endl;
    array.destroy(1);
    try {
        cout << array[1];
    } catch (runtime_error& e) {
        cout << e.what() << endl;
    }
    cout << "SIZE = " << array.size() << ", USED = " << array.usedSize() << endl;

    cout << "\n=== Assignment and Modification ===" << endl;
    cout << "array[2] = " << array[2] << endl;
    array[2] = "NEW!";
    cout << "array[2] = " << array[2] << endl;

    cout << "\n=== Memory Size ===" << endl;
    cout << "SIZE = " << array.size() << ", USED = " << array.usedSize() << endl;
    cout << "MEMORY SIZE = " << array.memorySize() << " bytes" << endl;

    cout << "\n=== Double Free Exception ===" << endl;
    try {
        array.destroy(1);
    } catch (runtime_error& e) {
        cout << e.what() << endl;
    }
    cout << "Testing ifFree (should not throw):" << endl;
    array.ifDestroy(1);

    cout << "\n=== Freeing All ===" << endl;
    array.destroy(2);
    cout << "SIZE = " << array.size() << ", USED = " << array.usedSize() << endl;

    cout << "\n=== Reassignment with Initializer List ===" << endl;
    array = ReducibleArray<string>({"str4", "str5", "str6", "str7"});
    cout << "array[3] = " << array[3] << endl;
    cout << "MEMORY SIZE = " << array.memorySize() << " bytes" << endl;
    cout << "SIZE = " << array.size() << ", USED = " << array.usedSize() << endl;

    cout << "\n=== Free and Access ===" << endl;
    array.destroy(3);
    try {
        cout << array[3];
    } catch (runtime_error& e) {
        cout << e.what() << endl;
    }

    cout << "\n=== Emplace (copy and move) ===" << endl;
    size_t idx = array.emplace("emplaced");
    cout << "Emplaced at: " << idx << ", value: " << array[idx] << endl;
    for(auto it = array.activeBegin(); it != array.activeEnd(); ++it){
        std::cout << *it;
    }
    string moved = "moved";
    if(array.size() != array.usedSize()){
        idx = array.emplace(std::move(moved));
        cout << "Emplaced (move) at: " << idx << ", value: " << array[idx] << endl;
    }

    cout << "\n=== ptrSwap and fullSwap ===" << endl;
    array.emplace(1, "swap1");
    array.emplace(2, "swap2");
    cout << "Before ptrSwap: " << array[1] << ", " << array[2] << endl;
    array.ptrSwap(1, 2);
    cout << "After ptrSwap: " << array[1] << ", " << array[2] << endl;
    array.fullSwap(1, 2);
    cout << "After fullSwap: " << array[1] << ", " << array[2] << endl;

    cout << "\n=== reducedCopy and reduce ===" << endl;
    std::vector<size_t> map;
    auto reduced = array.reducedCopy(map);
    cout << "Reduced copy size: " << reduced.size() << ", used: " << reduced.usedSize() << endl;
    cout << "Mapping: ";
    for (auto m : map) cout << m << " ";
    cout << endl;

    array.reduce(map);
    cout << "After reduce, size: " << array.size() << ", used: " << array.usedSize() << endl;
    cout << "Mapping: ";
    for (auto m : map) cout << m << " ";
    cout << endl;

    cout << "\n=== Iterators ===" << endl;
    cout << "All elements: ";
    for (auto it = array.begin(); it != array.end(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "Active elements: ";
    for (auto it = array.activeBegin(); it != array.activeEnd(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "\n=== clear, reset, and freeAll ===" << endl;
    array.clear();
    cout << "After clear: used = " << array.usedSize() << endl;
    array.reset("reset!");
    cout << "After reset: ";
    for (size_t i = 0; i < array.size(); ++i)
        cout << array[i] << " ";
    cout << endl;
    
    // === Reserve and Add Element Test ===
    cout << "\n=== Reserve and Add Element ===" << endl;
    array.clear();
    cout << "Before reserve: size = " << array.size() << ", used = " << array.usedSize() << endl;
    array.reserve(10);
    cout << "After reserve(10): size = " << array.size() << ", used = " << array.usedSize() << endl;
    size_t newIdx = array.emplace("reserved-added");
    cout << "Added at index: " << newIdx << ", value: " << array[newIdx] << endl;
    cout << "After emplace: size = " << array.size() << ", used = " << array.usedSize() << endl;
    cout << "Active elements: ";
    for (auto it = array.activeBegin(); it != array.activeEnd(); ++it)
        cout << *it << " ";
    cout << endl;
    // Test [] operator: assignment and access
    cout << "\n=== [] Operator Usage ===" << endl;
    cout << "Before assignment: array[" << newIdx << "] = " << array[newIdx] << endl;
    array[newIdx] = "changed-value";
    cout << "After assignment: array[" << newIdx << "] = " << array[newIdx] << endl;
    // Access another index (should throw if not active)
    try {
        cout << "Accessing array[5]: " << array[5] << endl;
    } catch (runtime_error& e) {
        cout << "Exception on array[5]: " << e.what() << endl;
    }
    
    // array.destroyArray(); (for realtime deallocation) or nothing
    delete arrayPtr;
    cout << "After freeAll: used = " << array.usedSize() << endl;

    cout << "\n=== find() Method Test ===" << endl;
    array = ReducibleArray<string>({"find1", "find2", "find3", "find2"});
    auto it = array.find("find2");
    if (it != array.end()) {
        cout << "Found 'find2' at index: " << (it.operator*() == "find2" ? "yes" : "no") << ", value: " << *it << endl;
    } else {
        cout << "'find2' not found" << endl;
    }

    const auto& constArray = static_cast<const ReducibleArray<string>&>(array);
    auto cit = constArray.find("find3");
    if (cit != constArray.end()) {
        cout << "Found 'find3' (const) at index: " << *cit << endl;
    } else {
        cout << "'find3' not found (const)" << endl;
    }

    auto notfound = array.find("notfound");
    cout << "Searching for 'notfound': ";
    if (notfound == array.end()) {
        cout << "not found (as expected)" << endl;
    } else {
        cout << "unexpectedly found: " << *notfound << endl;
    }

    auto notfound_const = constArray.find("notfound");
    cout << "Searching for 'notfound' (const): ";
    if (notfound_const == constArray.end()) {
        cout << "not found (as expected)" << endl;
    } else {
        cout << "unexpectedly found: " << *notfound_const << endl;
    }

    cout << "\n=== Comparison Operators ===" << endl;
    ReducibleArray<string> arr1({"a", "b"});
    ReducibleArray<string> arr2({"a", "b"});
    cout << "arr1 == arr2: " << (arr1 == arr2) << endl;
    arr2[1] = "c";
    cout << "arr1 != arr2: " << (arr1 != arr2) << endl;

    return 0;
}