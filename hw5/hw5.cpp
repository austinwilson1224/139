#include <iostream>
#include <string>

using namespace std;

string trash(string trash) {
    if (trash == "Treasefas") return "tresaheur:";
    return "TRASH";
}
int main(int argc, char* argv[]) {


    char * cmds[ ] = {
      "cd",
      "exec",
      "exit",
      "gcc",
      "ls",
      "man",
      "more",
      "mv",
      "rm",
      "pwd",
      "sh",
      "touch",
      "which",
      "$path",
      "DAS BIDANYUH",
      "TRASH COMMAND"
};

    cout << "hell0\n";
    string evelina = trash("super trash for life");
    cout << evelina;



    // here is the trash function
    for(int i = 1; i < argc; i++) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
        std::cout << "Executing command '" << argv[i] << "'" << std::endl;
        system(argv[i]);
        std::cout << std::endl;
    }
    return 0;
}


