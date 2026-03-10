#include <iostream>
#include <chrono>
#include "BinarySearchTree.hpp"

int main(){
    auto startTime = std::chrono::high_resolution_clock::now();
    using namespace std;
    adt::tree::BST<int> bt;
    
    cout << "=== INSERTION & DUPLICATES ===" << endl;
    bt.insert(50);
    bt.insert(30);
    bt.insert(70);
    bt.insert(20);
    bt.insert(40);
    bt.insert(60);
    bt.insert(80);
    bt.insert(30);  // duplicate
    bt.insert(50);  // duplicate
    cout << "Inserted: 50, 30, 70, 20, 40, 60, 80, 30(dup), 50(dup)" << endl;
    
    cout << "\n=== SIZE & UNIQUE SIZE ===" << endl;
    cout << "Total size: " << bt.size() << " (expected 9)" << endl;
    cout << "Unique size: " << bt.uniqueSize() << " (expected 7)" << endl;
    cout << "Is empty: " << bt.isEmpty() << " (expected 0)" << endl;
    
    cout << "\n=== HEIGHT ===" << endl;
    cout << "Height: " << bt.height() << endl;
    
    cout << "\n=== IS BALANCED ===" << endl;
    cout << "Is balanced: " << bt.isBalanced() << " (expected 1)" << endl;
    
    cout << "\n=== MIN & MAX ===" << endl;
    cout << "Min: " << bt.min() << " (expected 20)" << endl;
    cout << "Max: " << bt.max() << " (expected 80)" << endl;
    
    cout << "\n=== SEARCH & CONTAINS ===" << endl;
    cout << "Search for 30: " << bt.search(30) << " (expected 2)" << endl;
    cout << "Contains 40: " << bt.contains(40) << " (expected 1)" << endl;
    cout << "Contains 999: " << bt.contains(999) << " (expected 0)" << endl;
    
    cout << "\n=== INORDER TRAVERSAL (sorted) ===" << endl;
    auto inorder_result = bt.inorder();
    for(int v : inorder_result) cout << v << " ";
    cout << endl;
    
    cout << "\n=== PREORDER TRAVERSAL ===" << endl;
    auto preorder_result = bt.preorder();
    for(int v : preorder_result) cout << v << " ";
    cout << endl;
    
    cout << "\n=== POSTORDER TRAVERSAL ===" << endl;
    auto postorder_result = bt.postorder();
    for(int v : postorder_result) cout << v << " ";
    cout << endl;
    
    cout << "\n=== REMOVE OPERATIONS ===" << endl;
    cout << "Before remove(30): size=" << bt.size() << ", count(30)=" << bt.search(30) << endl;
    bt.remove(30);  // should decrement count, not delete node
    cout << "After remove(30): size=" << bt.size() << ", count(30)=" << bt.search(30) << endl;
    
    bt.remove(30);  // should delete node now
    cout << "After remove(30) again: size=" << bt.size() << ", count(30)=" << bt.search(30) << endl;
    cout << "Is balanced: " << bt.isBalanced() << endl;
    
    cout << "\n=== REMOVE LEAF NODE ===" << endl;
    bt.remove(20);
    cout << "After remove(20-leaf): size=" << bt.size() << endl;
    
    cout << "\n=== REMOVE NODE WITH TWO CHILDREN ===" << endl;
    cout << "Before remove(30-internal): size=" << bt.size() << endl;
    bt.remove(30);
    cout << "After remove(30-internal): size=" << bt.size() << endl;
    cout << "Inorder after: ";
    auto inorder2 = bt.inorder();
    for(int v : inorder2) cout << v << " ";
    cout << endl;
    
    cout << "\n=== CLEAR ===" << endl;
    cout << "Before clear: size=" << bt.size() << ", isEmpty=" << bt.isEmpty() << endl;
    bt.clear();
    cout << "After clear: size=" << bt.size() << ", isEmpty=" << bt.isEmpty() << endl;
    
    cout << "\n=== UNBALANCED TREE TEST ===" << endl;
    adt::tree::BST<int> bt2;
    bt2.insert(1);
    bt2.insert(2);
    bt2.insert(3);
    bt2.insert(4);
    bt2.insert(5);
    cout << "Inserted 1,2,3,4,5 (linear): isBalanced=" << bt2.isBalanced() << " (expected 0)" << endl;
    cout << "Height: " << bt2.height() << " (expected 4)" << endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();

    // Convert to milliseconds since epoch
    auto start = std::chrono::duration_cast<std::chrono::microseconds>(startTime.time_since_epoch()).count();
    auto end = std::chrono::duration_cast<std::chrono::microseconds>(endTime.time_since_epoch()).count();

    // Calculate elapsed
    double elapsed = end - start;
    double elapsedMicro = elapsed / 1000;
    std::cout << "Start: " << start << " micros" << endl;
    std::cout << "End: " << end << " micros" << endl;
    std::cout << "Elapsed: " << elapsed << " micros = " << elapsedMicro << "millis" << endl;

    return 0;
}