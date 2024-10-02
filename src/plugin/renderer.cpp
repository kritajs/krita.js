#include <QDebug>
#include <QEvent>
#include <QResizeEvent>
#include <QTimer>
#include <AppCore/CAPI.h>
#include "renderer.h"

Renderer::Renderer(QObject *parent, const char *_basePath) : QObject(parent)
{
    // Enable required platform handlers
    // Use the OS's native font loader
    ulEnablePlatformFontLoader();
    // Use AppCore's file system singleton to load file:/// URLs from the OS.
    std::string assetsPath = "/assets";
    ULString basePath = ulCreateString((_basePath + assetsPath).c_str());
    ulEnablePlatformFileSystem(basePath);
    ulDestroyString(basePath);

    // Use AppCore's default logger to write the log file to disk.
    std::string logFilePath = "/ultralight.log";
    ULString logPath = ulCreateString((_basePath + logFilePath).c_str());
    ulEnableDefaultLogger(logPath);
    ulDestroyString(logPath);

    // Create renderer
    ULConfig config = ulCreateConfig();
    m_renderer = ulCreateRenderer(config);
    ulDestroyConfig(config);

    // Allow remote inspection
    ulStartRemoteInspectorServer(m_renderer, "127.0.0.1", 19998);

    // Hook into Krita/Qt's event loop so that we can continuously update Ultralight
    QTimer *timer = new QTimer(this);
    timer->setObjectName("krita.js timer");
    QObject::connect(timer, &QTimer::timeout, this, &Renderer::tick);
    timer->start(0);

    setObjectName("krita.js Renderer");
}

Renderer::~Renderer()
{
    ulDestroyRenderer(m_renderer);
}

bool Renderer::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize)
    {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        for (int i = 0; i < m_views.size(); ++i)
        {
            View *view = m_views.at(i);
            QSize newSize = resizeEvent->size();
            if (!view->m_isReady)
                continue;

            view->resize(newSize.width(), newSize.height());
            ulViewResize(view->m_view, newSize.width(), newSize.height());
            // If ulViewResize is called, the bitmap will be cleared so we need to render
            // again to repopulate the bitmap.
            ulRender(m_renderer);
        }
    }

    return false;
}

View *Renderer::createView(QWidget *parent)
{
    ULViewConfig viewConfig = ulCreateViewConfig();
    ulViewConfigSetIsTransparent(viewConfig, true);
    // Use CPU acceleration so that we can request a bitmap. We use QPainter to render the bitmap.
    ulViewConfigSetIsAccelerated(viewConfig, false);
    ULView ulView = ulCreateView(m_renderer, parent->width(), parent->height(), viewConfig, NULL);
    ulDestroyViewConfig(viewConfig);

    View *view = new View(parent, ulView);
    view->setObjectName(QString("krita.js View #%1").arg(m_views.count()));
    view->resize(parent->width(), parent->height());
    ulViewResize(view->m_view, parent->width(), parent->height());
    m_views.append(view);
    return view;
}

void Renderer::tick()
{
    // Any resizing should be done before these calls. If ulViewResize is called,
    // the bitmap will be cleared so we need to render again to repopulate the
    // bitmap.
    ulUpdate(m_renderer);
    ulRender(m_renderer);

    // Queue a repaint if any views need repainting
    for (int i = 0; i < m_views.size(); ++i)
    {
        View *view = m_views.at(i);
        if (!view->m_isReady || !view->isVisible())
            continue;

        ULSurface surface = ulViewGetSurface(view->m_view);
        if (!ulIntRectIsEmpty(ulSurfaceGetDirtyBounds(surface)))
        {
            qDebug("QUEUE PAINT");
            view->update();
        }
    }
}

#include "moc_renderer.cpp"
