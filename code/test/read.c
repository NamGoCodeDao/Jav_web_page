#include "syscall.h"
#include "copyright.h"


int main()
{
    // int fileID;
    // char buffer[maxlen];
    // int i, size;
    /*Create a file*/
    char* buffer[100];
    // Test write
    int x = Read(buffer, 100, consoleInputID);
    int fileID = Open("text.txt", 0);
    Write(buffer, x, fileID);

    //Test read
    // int fileID = Open("text.txt", 0);
    // int x = Read(buffer, 100, fileID);
    // Write(buffer, x, consoleOutputID);
    Halt();
    return 0;
}