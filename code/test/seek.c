#include "syscall.h"
#include "copyright.h"
#define maxlen 100

int main()
{
    int fileID;
    char buffer[maxlen];
    int i, size;
    int result;
    int temp;
    /*Create a file*/
     fileID = Open("text.txt", 0);
     result = Seek(5, fileID);
     buffer[100];
     size = Read(buffer,100 , fileID);
     temp =Write(buffer, 100, consoleOutputID);
    Halt();
    return 0;
}