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

int i = 0;
int main(int argc, char *argv[]) {
    // clock_t t = clock();
    // if (t > 3000) {
    //     return 0;
    // }
    // cout << t << " ";
    // main();
    // return 0;
    // cout << argc << "\n\n";

    if(argc > 10) {
        return 0;
    }

    std::cout << i;
    main(i++, argv);
}

