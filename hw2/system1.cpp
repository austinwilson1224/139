/**
 * g++ -o system1 system1.cpp
 * ./system1
 */
#include <iostream>
#include <stdlib.h>
//using namespace std;
int main(int argc, char *argv[]) {
    //cout << "next system command:" << endl;
    system("ls");
    //cout << "\n";
    //cout << "next system command:" << endl;
    system("pwd");
    //cout << "\n";
    //cout << "next system command:" << endl;
    system("whoami");
    //cout << "\n";
    return 0;
}