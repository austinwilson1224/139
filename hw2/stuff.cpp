#include <iostream>


// argc 
int main(int argc, char *argv[]) {
    std::cout << argv[0] << "\n";
    std::cout << argv[1] << "\n";
    std::cout << argv[2] << "\n";


    system(argv[1]);



    //std::cout << 
    return 0;
}