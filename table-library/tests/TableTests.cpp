// Small unit-style tests for table::Table
// Exercises operator[] proxy, at(), and exception behavior
#include "../Table.hpp"
#include <iostream>

using namespace table;

static int failures = 0;

void expect(bool cond, const char* msg){
    if(!cond){
        std::cerr << "FAIL: " << msg << "\n";
        ++failures;
    } else {
        std::cout << "PASS: " << msg << "\n";
    }
}

int main(){
    // Build a small table
    Table t;
    t.setTable({
        {"id","name","score"},
        {"1","Alice","87"},
        {"2","Bob","91"}
    });

    // Basic read via operator[][]
    try{
        std::string &refname = t[2][1];
        refname = "Michael";
        std::cout << "\tChanged name in row 2 to: " << t[2][1] << std::endl;
        
        std::string name = t[1][1];
        std::string name2 = name;
        
        std::cout << "\tname 1 : " << name << std::endl;
        std::cout << "\talso name 1: " << name2 << std::endl;
        
        expect(name == "Alice", "t[1][1] == Alice");
    }catch(...){ expect(false, "t[1][1] should not throw"); }
    
    std::cout << std::endl;
    
    // Mutation via operator[][]
    try{
        t[1][2] = "88"; // change Alice's score
        
        std::string str = t[1][2];
        std::cout << "\tmutated score: " << str << std::endl;
        
        expect(t.at(1,2) == "88", "t[1][2] mutation reflected by at()");
    }catch(...){ expect(false, "mutation via t[1][2] should not throw"); }

    std::cout << std::endl;
    
    // Basic read via operator[]
    try{
        // Demonstrate that we can obtain a reference to the underlying vector
        try{
            std::vector<std::string> &row = t[2];
            row = {"2", "Zoe", "91"};
            
            std::cout << "\tChanged row 2 to:";
            for (size_t i = 0; i < t[2].size(); ++i) {
                std::cout <<  " " << t[2][i];
            }
            std::cout << std::endl;
            
        }catch(const std::runtime_error& e){ 
            expect(false, "reference assignment via t[2] threw:"); 
            e.what();
        }
        
        t[0] = std::vector<std::string>{"9", "Jaiden", "97", "19"}; // change Alice's score
        auto row = t[2];

        std::cout << "\trow 2:";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout <<  " " << row[i];
        }
        std::cout << std::endl;

    expect(!row.empty() && row[1] == "Zoe", "t[2][1] == Zoe after mutation");
    }catch(...){ expect(false, "t[1][1] should not throw"); }
    
    std::cout << std::endl;
    
    // Mutation via operator[]
    try{
        std::string str = t[1][2];
        std::cout << "\tmutated score: " << str << std::endl;
        
        expect(t.at(1,2) == "88", "t[1][2] mutation reflected by at()");
    }catch(...){ expect(false, "mutation via t[1][2] should not throw"); }

    std::cout << std::endl;
    
    // Checked at() usage
    try{
        std::cout << "\tt.at(0,2) returns " << t.at(0,2) << std::endl; // header, valid
        expect(true, "at(0,2) valid");
    }catch(...){ expect(false, "at(0,2) threw unexpectedly"); }

    std::cout << std::endl;
    
    // Row out of bounds
    try{
        auto r = t[105];
        (void)r;
        expect(false, "t[100] should have thrown RowOutOfBoundsException");
    }catch(const RowOutOfBoundsException& e){
        e.what(); 
        expect(true, "t[100] throws RowOutOfBoundsException"); 
    }
    catch(...){ expect(false, "t[100] threw wrong exception"); }

    std::cout << std::endl;
    
    // Column out of bounds via proxy
    try{
        auto r = t[1][100];
        expect(false, "t[1][100] should throw ColumnOutOfBoundsException");
    }catch(const ColumnOutOfBoundsException& e){ 
        e.what();
        expect(true, "t[1][100] throws ColumnOutOfBoundsException"); 
    }
    catch(...){ expect(false, "t[1][100] threw wrong exception"); }

    std::cout << std::endl;
    
    try{
        std::cout << "\tview mutated score: ";
        if(t.isEmpty()){
            std::cout << "[EMPTY TABLE]" << std::endl;
        } else {
            // bind the view to a local variable so we don't index into a temporary
            auto view = t.view();
            std::cout << view[1][2] << std::endl;
        }

        t[1][1] = "Sam"; // mutate original
    }catch(...){ expect(false, "view() tests threw"); }

    std::cout << std::endl;

    // copy() usage - should produce an independent deep copy
    try{
        auto c = t.copy();
        c[1][2] = "55"; // mutate the copy

        // const copy access
        const auto cc = t.copy();
        try{
            auto val = cc[1][2];
            expect(val == t.at(1,2), "const copy preserves values");
        }catch(...){ expect(false, "const copy operator[][] threw"); }
    }catch(...){ expect(false, "copy() tests threw"); }

    std::cout << std::endl;
    
    
    
    if(failures == 0){
        std::cout << "All tests passed\n";
        return 0;
    }
    std::cerr << failures << " test(s) failed\n";
    return 2;
}
