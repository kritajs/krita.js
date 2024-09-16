#include <stdio.h>
#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include <QObject>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>
#include <Ultralight/CAPI.h>

#include "Krita.h"
#include "Window.h"

class KJS : public QDockWidget
{
public:
    KJS(QWidget *parent, const char *_basePath) : QDockWidget(parent) {
        qDebug() << "PRE-INIT KJS";

        // Enable required platform handlers
        // Use the OS's native font loader
        ulEnablePlatformFontLoader();
        // Use AppCore's file system singleton to load file:/// URLs from the OS.
        ULString basePath = ulCreateString(_basePath);
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

        // Create view
        ULViewConfig viewConfig = ulCreateViewConfig();
        // Using a device scale of 2.0 for now. Should get this value from Qt instead.
        ulViewConfigSetInitialDeviceScale(viewConfig, 2.0);
        // Disable GPU acceleration so that we can request a bitmap. We use QPainter to render the bitmap.
        ulViewConfigSetIsAccelerated(viewConfig, false);
        m_view = ulCreateView(m_renderer, 1000, 500, viewConfig, 0);
        ulDestroyViewConfig(viewConfig);

        // Load content into the view
        ULString entryUrl = ulCreateString("file:///assets/page.html");
        ulViewLoadURL(m_view, entryUrl);
        ulDestroyString(entryUrl);

        // Register callback for when the view has finished loading
        ulViewSetFinishLoadingCallback(m_view, &onViewLoaded, this);

        // Hook into Krita/Qt's event loop so that we can continuously update Ultralight
        QTimer *timer = new QTimer(this);
        timer->setObjectName("krita.js timer");
        QObject::connect(timer, &QTimer::timeout, this, &KJS::tick);
        timer->start(50);

        qDebug() << "POST-INIT KJS";
    }

    ~KJS() {
        qDebug() << "DESTROY KJS";
        ulDestroyView(m_view);
        ulDestroyRenderer(m_renderer);
        delete m_img;
    }

    void tick()
    {
        ulUpdate(m_renderer);
        
        // If Ultralight surface pixels are dirty then queue a repaint
        ULSurface surface = ulViewGetSurface(m_view);
        if (!ulIntRectIsEmpty(ulSurfaceGetDirtyBounds(surface))) {
            qDebug("QUEUE PAINT");
            update();
        }
    }

    void paintEvent(QPaintEvent *)
    {
        // m_img will be set once the view is loaded. Skip painting before that has happened.
        if (m_img == nullptr)
        {
            return;
        }

        qDebug("PAINT EVENT");
        ulRender(m_renderer);
        ULSurface surface = ulViewGetSurface(m_view);
        ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
        void *pixelBuffer = ulBitmapLockPixels(bitmap);
        QPainter painter(this);
        painter.drawImage(QPoint(0,0), *m_img);
        ulBitmapUnlockPixels(bitmap);
        ulSurfaceClearDirtyBounds(surface);
    }

    static void onViewLoaded(void *user_data, ULView caller, unsigned long long frame_id, bool is_main_frame, ULString url)
    {
        if (is_main_frame)
        {
            qDebug("VIEW LOADED");
            KJS *kjs = static_cast<KJS*>(user_data);
            kjs->_onViewLoaded();
        }
    }

private:
    ULView m_view;
    ULRenderer m_renderer;
    QImage *m_img = nullptr;

    void _onViewLoaded()
    {
        ULSurface surface = ulViewGetSurface(m_view);
        ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
        void *pixelBuffer = ulBitmapLockPixels(bitmap);
        m_img = new QImage(
            static_cast<const uchar*>(pixelBuffer),
            ulBitmapGetWidth(bitmap),
            ulBitmapGetHeight(bitmap),
            QImage::Format_ARGB32
        );
        ulBitmapUnlockPixels(bitmap);
        update();
    }
};

bool initialized = false;

// Entry point to krita.js
// Must use `extern "C"` to prevent compiler from name mangling
extern "C" __declspec(dllexport) void KRITAJS(const char *basePath)
{
    if (initialized)
    {
        qDebug("krita.js has already been initialized!");
        return;
    }
    initialized = true;

    qDebug() << "Initializing krita.js...";
    Krita *krita = Krita::instance();
    QMainWindow *qwindow = krita->activeWindow()->qwindow();
    KJS *kjs = new KJS(qwindow, basePath);
    kjs->move(200, 200);
    kjs->resize(1000, 500);
    kjs->show();

    return;
}
