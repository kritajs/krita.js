#include "krita.h"
#include "renderer.h"
#include <QDebug>
#include <QPointer>

// Must use `extern "C"` to prevent compiler from name mangling
#define KRITAJS_EXPORT extern "C" __declspec(dllexport)

const QPointer<Renderer> renderer = new Renderer();

// Initialize krita.js. This should be called during Krita's startup so that
// krita.js plugins are able to register themselves as extensions and/or add
// dock widget factories.
KRITAJS_EXPORT void initialize(const char *basePath) {
    if (renderer->m_initialized) {
        qWarning() << "krita.js has already been initialized!";
        return;
    }

    qDebug() << "Initializing krita.js...";
    renderer->initialize(basePath);
}

// Start rendering. This should only be called when Krita's main window has been
// created.
KRITAJS_EXPORT void start() {
    if (!renderer->m_initialized) {
        qWarning() << "Tried to start krita.js rendering but renderer has not "
                      "been initialized.";
        return;
    }

    Krita *krita = Krita::instance();
    renderer->setParent(krita);
    QMainWindow *qwin = krita->activeWindow()->qwindow();
    // Let renderer intercept events on qwin so that it can detect resize events
    qwin->installEventFilter(renderer);
    View *view = renderer->createView(qwin);
    view->show();
}
