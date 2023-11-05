#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main()
{
    // int fileID;
    // char buffer[maxlen];
    // int i, size;
    /*Create a file*/
    if (Create("text.txt") == -1)
    {
        // xuất thông báo lỗi tạo tập tin
    }
    else
    {
        // xuất thông báo tạo tập tin thành công
    }
    if (Create("text1.txt") == -1)
    {
        // xuất thông báo lỗi tạo tập tin
    }
    else
    {
        // xuất thông báo tạo tập tin thành công
    }
    if (Create("text2.txt") == -1)
    {
        // xuất thông báo lỗi tạo tập tin
    }
    else
    {
        // xuất thông báo tạo tập tin thành công
    }
    Halt();
    return 0;
}