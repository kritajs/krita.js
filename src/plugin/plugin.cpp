#include <stdio.h>
#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include "Krita.h"
#include "Window.h"
#include "renderer.h"

// Must use `extern "C"` to prevent compiler from name mangling
#define KRITAJS_EXPORT extern "C" __declspec(dllexport)

bool initialized = false;
Renderer *renderer = nullptr;

// Initialize krita.js. This should be called during Krita's startup so that
// krita.js plugins are able to register themselves as extensions and/or add
// dock widget factories.
KRITAJS_EXPORT void initialize(const char *basePath)
{
    if (initialized)
    {
        qWarning("krita.js has already been initialized!");
        return;
    }
    initialized = true;

    qDebug() << "Initializing krita.js...";
    Krita *krita = Krita::instance();
    renderer = new Renderer(krita, basePath);

    // QDockWidget *pluginManager = new QDockWidget(qwindow);
    // pluginManager->setObjectName("krita.js Plugin Manager");
    // View *pluginManagerView = renderer->createView(0);
    // pluginManager->setWidget(pluginManagerView);
    // pluginManager->move(200, 200);
    // pluginManager->resize(640, 480);
    // pluginManager->show();
}

// Start rendering. This should only be called when Krita's main window has been created.
KRITAJS_EXPORT void start()
{
    if (!renderer)
    {
        qWarning("Tried to start krita.js rendering but renderer has not been initialized.");
    }

    qDebug("STARTING KRITA.JS RENDERING");
}
