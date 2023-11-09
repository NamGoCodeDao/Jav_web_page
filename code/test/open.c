#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main()
{
    // int fileID;
    // char buffer[maxlen];
    // int i, size;
    /*Create a file*/
    int a = Create("a.txt");
    int fileID = Open("text.txt", 1);
    int fileID2 = Open("text2.txt", 0);
    // int fileID3 = Open("text3.txt", 1);
    // int a = Close(fileID2);
    // int rs_rm2=Remove("text2.txt");
    Halt();
    return 0;
}