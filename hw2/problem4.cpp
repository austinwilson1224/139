/**
 * problem 4
 * 
 * commands to run program 
 * g++ -o problem4 problem4.cpp
 * ./a
 */


using namespace std;
#include <iostream>
#include <ctime> /* clock_t, clock, CLOCKS_PER_SEC */



int main() {

    clock_t t = clock();
    if (t > 3000) {
        return 0;
    }
    cout << t << "\n" ;
    main();
    return 0;
}