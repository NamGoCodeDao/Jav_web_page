#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main()
{
    // int fileID;
    // char buffer[maxlen];
    // int i, size;
    /*Create a file*/
    int fileID = Open("text.txt", 1);
    int fileID2 = Open("text2.txt", 0);
    int fileID3 = Open("text3.txt", 1);
//    int rs_rm=Remove("text3.txt");
//    int rs_rm2=Remove("text2.txt");
    Halt();
    return 0;
}