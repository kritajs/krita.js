#include <stdio.h>
#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include "Krita.h"
#include "Window.h"
#include "renderer.h"

bool initialized = false;

// Entry point to krita.js
// Must use `extern "C"` to prevent compiler from name mangling
extern "C" __declspec(dllexport) void KRITAJS(const char *basePath)
{
    if (initialized)
    {
        qWarning("krita.js has already been initialized!");
        return;
    }
    initialized = true;

    qDebug() << "Initializing krita.js...";
    Krita *krita = Krita::instance();
    QMainWindow *qwindow = krita->activeWindow()->qwindow();
    Renderer *renderer = new Renderer(qwindow, basePath);

    QDockWidget *pluginManager = new QDockWidget(qwindow);
    pluginManager->setObjectName("krita.js Plugin Manager");
    View *pluginManagerView = renderer->createView(0);
    pluginManager->setWidget(pluginManagerView);
    pluginManager->move(200, 200);
    pluginManager->resize(640, 480);
    pluginManager->show();
}
