#include <stdio.h>

extern "C" __declspec(dllexport) int add_one(int i);

int add_one(int i)
{
    return i+1;
}
