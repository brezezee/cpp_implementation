#include "shared_ptr.hpp"
#include <cstdio>
#include <iostream>

using namespace std;



int main() {
    auto p = shared_ptr<int>(new int(2));
    *p.get() = 3;
    cout << *p << endl;
    p.reset();
    shared_ptr<int> a(p);
    cout << *a << endl;

    
    getchar();
    
    getchar();
    getchar();
    return 0;
}
