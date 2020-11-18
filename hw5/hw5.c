#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define MAX 1000
#define LENGTH 15

//list of commands supported by cli
char * cmds[LENGTH] =
{       
        "cd",
        "exec",
        "exit",
        "fork",
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
        "$PATH"
};

char cmd[MAX];
char buff[MAX];

int main(int argc, char * argv[])
{
        //main
        int i, count=0;
        buff[0] = '0';
        printf("My cli 2020\n");
        printf("Commands supported: ");
        printf("%d strings passed to argv[]\n", argc);
        while (--argc)
        {       
                count++;
                printf("Next string is '%s'\n" , argv[count]);
                strcat(buff, argv[count]);
                strcat(buff, ";");
                printf("New string is '%s'\n", buff);
        }
        if (count)
        {
                i=0;
                while(cmds[i])
                {
                        if(strcmp(cmds[i],argv[1]))
                        {
                                i++;
                        }else
                        {
                                break;
                        }
                }
                if(cmds[i])
                {
                        printf("Cmd '%s' is predefined. \n", cmds[i]);
                }else
                {
                        printf("Command not supported: ");
                }
                system(buff);
        }
        return 0;
}