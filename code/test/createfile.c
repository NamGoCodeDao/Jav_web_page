#include "syscall.h"
int main()
{
    int a = Write("Nhap vao ten file: ", 100, consoleOutputID);
    char filename[255];
    int b = Read(filename, 255, consoleInputID);
    int c = Create(filename);
    if (c != -1)
    {
        Write("Tao file thanh cong\n", 100, consoleOutputID);
    }
    else
    {
        Write("Tao file khong thanh cong\n", 100, consoleInputID);
    }
    Halt();
    return 0;
}