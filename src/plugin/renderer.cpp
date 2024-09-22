#include <QTimer>
#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>
#include <Ultralight/CAPI.h>
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

    // Hook into Krita/Qt's event loop so that we can continuously update Ultralight
    QTimer *timer = new QTimer(this);
    timer->setObjectName("krita.js timer");
    QObject::connect(timer, &QTimer::timeout, this, &Renderer::tick);
    timer->start(50);
}

Renderer::~Renderer()
{
    ulDestroyRenderer(m_renderer);
}

View *Renderer::createView(QWidget *parent)
{
    ULViewConfig viewConfig = ulCreateViewConfig();
    // ulViewConfigSetInitialDeviceScale(viewConfig, 2.0);
    // Use CPU acceleration so that we can request a bitmap. We use QPainter to render the bitmap.
    ulViewConfigSetIsAccelerated(viewConfig, false);
    ULView ulView = ulCreateView(m_renderer, 640, 480, viewConfig, 0);
    ulDestroyViewConfig(viewConfig);

    View *view = new View(parent, ulView);
    m_views.append(view);
    return view;
}

void Renderer::tick()
{
    ulUpdate(m_renderer);
    ulRender(m_renderer);

    // Queue a repaint if any views need repainting
    for (int i = 0; i < m_views.size(); ++i)
    {
        View *view = m_views.at(i);
        if (!view->m_isReady)
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
