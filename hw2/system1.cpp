/**
 * g++ -o system1 system1.cpp
 * ./system1
 */
#include <iostream>
#include <stdlib.h>
using namespace std;
int main(int argc, char *argv[]) {
    cout << "next system command:" << endl;
    system("ls");
    cout << endl;
    cout << "next system command:" << endl;
    system("pwd");
    cout << endl;
    cout << "next system command:" << endl;
    system("whoami");
    cout << endl;
    return 0;
}