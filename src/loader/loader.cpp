#include <stdio.h>
#include <QDebug>

extern "C" __declspec(dllexport) int add_one(int i);

int add_one(int i)
{
    qDebug() << "HELLO FROM KRITA.JS C++";
    return i+1;
}
