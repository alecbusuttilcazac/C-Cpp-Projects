#include <iostream>
#include "Stack.hpp"

using namespace std;
using namespace adt;

int main(){
    cout << "=== Stack Demo ===" << endl << endl;
    
    // Create a stack of integers
    Stack<int> s;
    cout << "Created empty stack" << endl;
    cout << "Is empty? " << (s.isEmpty() ? "Yes" : "No") << endl;
    cout << "Size: " << s.size() << endl << endl;
    
    // Push some values
    cout << "Pushing: 10, 20, 30, 40, 50" << endl;
    s.push(10);
    s.push(20);
    s.push(30);
    s.push(40);
    s.push(50);
    cout << "Size: " << s.size() << endl << endl;
    
    // Peek at top
    cout << "Top of stack (peek): " << s.peek() << endl << endl;
    
    // Pop all elements
    cout << "Popping all elements:" << endl;
    while(!s.isEmpty()) {
        cout << s.pop() << " ";
    }
    cout << endl << endl;
    
    // Test copy constructor
    cout << "Testing copy constructor:" << endl;
    Stack<int> s1;
    s1.push(100);
    s1.push(200);
    s1.push(300);
    
    Stack<int> s2 = s1;  // Deep copy
    cout << "Original stack popped: " << s1.pop() << endl;
    cout << "Copied stack popped: " << s2.pop() << endl;
    cout << "Original still has: " << s1.peek() << endl;
    cout << "Copy still has: " << s2.peek() << endl << endl;
    
    // Test exception handling
    cout << "Testing empty stack exception:" << endl;
    Stack<int> s3;
    try {
        s3.pop();
    } catch(const std::underflow_error& e) {
        cout << "Caught exception: " << e.what() << endl;
    }
    
    return 0;
}
