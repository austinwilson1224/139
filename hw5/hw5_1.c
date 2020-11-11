/*
Dat Hua
CSC 139
Herbert Mayer
Code is provided on Zoom.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX     1000    //arbitrary length of 1 buffer for a line
#define LENGTH  15
#define USAGE( str ) printf( "%s", str );    \
        for (i = 0; cmds[i]; i++) {             \
                printf( "%s", cmds[i]);         \
        };                                      \
        printf( "\n" );
char buff[MAX];    //string buffer for 1 line
const char* cmds[LENGTH] = {"cd","exec","fork","gcc","ls","man","more","mv","rm","pwd","sh","touch","which","$PATH"};

char cmd[MAX];          // temp to hold currently scanned command

int main(int argc, char* argv[])
{

    int i;
    int count=0;    //count a.out ... strings on command line
    buff[0] = 0;            //null-terminated string
    printf("INTIAL cli 2020\n");
    USAGE("Legal commands: ");
    printf(" %d strings passed to argv[]\n", argc);
    while (argc--) {
        printf("%d\n",count);
        printf("%i\n",argc);
        count++;
        printf("next string is '%s'\n", argv[count]);
        strcat(buff, argv[count]);
        strcat(buff, "; ");
        printf("new string is '%s'\n", buff);

    }//end while
    if (count) {
        //there was a Unix command, beyond a.out or cli
        i = 0;  //search for all cmds[][], start with cmds[0][]
        while (cmds[i]) {
            if (strcmp(cmds[i], argv[1])) {
                //find the next one, or index null string
                i++;

            }
            else {
                //found, so break the loop
                break;
            }//end if
        }//end while
        if (cmds[i]) {
            printf("cmd '%s' is one of predefined.\n", cmds[i]);
        }
        else {
            USAGE("Not one of legal commands:");
        } //end if
        system(buff);
    } //end if
    return 0;
} //end main
