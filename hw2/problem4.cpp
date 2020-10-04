/**
 * problem 4
 * 
 * commands to run program 
 * g++ -o problem4 problem4.cpp
 * ./a
 */
#include <iostream>
#include <ctime>
using namespace std;
int main() {
    clock_t t = clock();
    if (t > 3000) {
        return 0;
    }
    cout << t << " ";
    main();
    return 0;
}