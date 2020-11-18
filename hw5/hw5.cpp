#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>

using namespace std;

char buff[1000];

int printArray(string arr[]) {
    string result = "";
    int length = sizeof(&arr) / sizeof(&arr[0]);



    for (int i = 0; i < length; i++) {
        std::cout << arr[i] << " ";
        result = result + arr[i] + " ";
    }
    return length;
}

bool contains(string value, string arr[], int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return true;
        } 
    }
    return false;
}

string allCommands(int argc, char* argv[]) {
    string result = "";
    for(int i = 1; i < argc; i++) {
        result += argv[i];
    }
    return result;
}


int main(int argc, char* argv[]) {
    string commands[] = {
        "cd",
        "exec",
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
        "$path"
    };
    int size = sizeof(commands) / sizeof(commands[0]);


    // intro section 
    std::cout << "\nMy CLI 2020" << endl;
    std::cout << "Legal commands: "; //<< printArray(commands) << endl;
    for(int i = 0; i < size; i++){
        std::cout << commands[i] << " ";
    }
    std::cout << endl << argc << " commands passed to argv[]" << endl;



    // command section 
    for (int i = 1;i < argc; i++) {
        // traverse argv
        
        std::cout << "next string is " << argv[i] << endl;
    }

    string s = allCommands(argc,argv);
    // std::cout << test << endl;




    // std::string s = "pwd,ls,mv temp ../temp";
    std::string delimiter = ",";


    // make an empty vector
    string cmds1[20];
    vector<string> test;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        //std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
        test.push_back(token);
    }
    // std::cout << s << std::endl;

    int size2 = test.size();
    // cout << size2;

    for(int i = 0; i <= size2 + 1; i++) {
        cout << test[i];
        system(test[i].c_str());
    }

    // bool test = contains("cd",commands, size);
    // std::cout << test;







    ////// from c code
    // std::cout << "\n\n\n";
    // int i;
    // int count=0;    //count a.out ... strings on command line
    // buff[0] = 0;            //null-terminated string
    // printf("INTIAL cli 2020\n");
    // //USAGE("Legal commands: ");
    // printf(" %d strings passed to argv[]\n", argc);
    // while (argc--) {
    //     printf("%d\n",count);
    //     printf("%i\n",argc);
    //     count++;
    //     printf("next string is '%s'\n", argv[count]);
    //     strcat(buff, argv[count]);
    //     strcat(buff, "; ");
    //     printf("new string is '%s'\n", buff);

    // }//end while


    return 0;
}





