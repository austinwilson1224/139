/**
 * g++ -o system2 system2.cpp
 * ./system2
 */
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    std::cout << "argc = " << argc << std::endl;

    for(int i = 1; i < argc; i++) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
        std::cout << "Executing command '" << argv[i] << "'" << std::endl;
        system(argv[i]);
        std::cout << std::endl;
    }
    return 0;
}