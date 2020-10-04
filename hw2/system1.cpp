/**
 * g++ -o system2 system2.cpp
 * ./system2
 */

using namespace std;

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {

    string arguments[] = {"ls","pwd","whoami"};

    cout << "next system command:" << endl;
    system("ls");
    cout << "\n";

    cout << "next system command:" << endl;
    system("pwd");
    cout << "\n";

    cout << "next system command:" << endl;
    system("whoami");
    cout << "\n";



    // cout << "\n\n\nOther way:\n";

    // int arraySize = sizeof(arguments) / sizeof(arguments[0]);

    // for(int i = 0; i < arraySize; i++) {
    //     string temp = arguments[i];
    //     cout << system(temp.c_str()) << endl;
    // }
    return 0;
}