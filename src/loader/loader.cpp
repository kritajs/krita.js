#include <stdio.h>
#include <QDebug>
#include <Canvas.h>

// Entry point to krita.js
// Must use `extern "C"` to prevent compiler from name mangling
extern "C" __declspec(dllexport) int KRITAJS(int i)
{
    qDebug() << "HELLO FROM KRITA.JS C++";
    // qDebug() << Krita::instance()->version();
    return i+1;
}
