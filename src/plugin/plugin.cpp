#include <stdio.h>
#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include <QMouseEvent>
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
    KJS(QWidget *parent, const char *_basePath) : QDockWidget(parent)
    {
        qDebug() << "PRE-INIT KJS";

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

        // Create view
        ULViewConfig viewConfig = ulCreateViewConfig();
        // Using a device scale of 2.0 for now. Should get this value from Qt instead.
        // ulViewConfigSetInitialDeviceScale(viewConfig, 2.0);
        // Disable GPU acceleration so that we can request a bitmap. We use QPainter to render the bitmap.
        ulViewConfigSetIsAccelerated(viewConfig, false);
        m_view = ulCreateView(m_renderer, 640, 480, viewConfig, 0);
        ulDestroyViewConfig(viewConfig);

        // Load content into the view
        ULString entryUrl = ulCreateString("file:///index.html");
        ulViewLoadURL(m_view, entryUrl);
        ulDestroyString(entryUrl);

        // Register callbacks
        ulViewSetDOMReadyCallback(m_view, &onViewDOMReady, this);
        ulViewSetFinishLoadingCallback(m_view, &onViewLoaded, this);
        ulViewSetAddConsoleMessageCallback(m_view, &onViewConsoleMessage, this);

        // Hook into Krita/Qt's event loop so that we can continuously update Ultralight
        QTimer *timer = new QTimer(this);
        timer->setObjectName("krita.js timer");
        QObject::connect(timer, &QTimer::timeout, this, &KJS::tick);
        timer->start(50);

        // Enable mouse move events
        setMouseTracking(true);

        qDebug() << "POST-INIT KJS";
    }

    ~KJS()
    {
        qDebug() << "DESTROY KJS";
        ulDestroyView(m_view);
        ulDestroyRenderer(m_renderer);
        delete m_img;
    }

protected:
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
        painter.drawImage(QPoint(0, 0), *m_img);
        ulBitmapUnlockPixels(bitmap);
        ulSurfaceClearDirtyBounds(surface);
    }

    void mousePressEvent(QMouseEvent *event)
    {
        ULMouseEvent e = ulCreateMouseEvent(kMouseEventType_MouseDown, event->x(), event->y(), kMouseButton_None);
        ulViewFireMouseEvent(m_view, e);
        ulDestroyMouseEvent(e);
    }

    void mouseReleaseEvent(QMouseEvent *event)
    {
        ULMouseEvent e = ulCreateMouseEvent(kMouseEventType_MouseUp, event->x(), event->y(), kMouseButton_None);
        ulViewFireMouseEvent(m_view, e);
        ulDestroyMouseEvent(e);
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        ULMouseEvent e = ulCreateMouseEvent(kMouseEventType_MouseMoved, event->x(), event->y(), kMouseButton_None);
        ulViewFireMouseEvent(m_view, e);
        ulDestroyMouseEvent(e);
    }

private:
    ULView m_view;
    ULRenderer m_renderer;
    QImage *m_img = nullptr;

    // Called when the view's DOM is ready
    static void onViewDOMReady(void *user_data,
                               ULView caller,
                               unsigned long long frame_id,
                               bool is_main_frame,
                               ULString url)
    {
        qDebug("DOM READY");
        KJS *kjs = static_cast<KJS *>(user_data);
        kjs->_onViewDOMReady();
    }

    // Called when the view has finished loading
    static void onViewLoaded(void *user_data,
                             ULView caller,
                             unsigned long long frame_id,
                             bool is_main_frame,
                             ULString url)
    {
        if (is_main_frame)
        {
            qDebug("VIEW LOADED");
            KJS *kjs = static_cast<KJS *>(user_data);
            kjs->_onViewLoaded();
        }
    }

    // Called when a message is added to the view's console
    static void onViewConsoleMessage(void *user_data,
                                     ULView caller,
                                     ULMessageSource source,
                                     ULMessageLevel level,
                                     ULString message,
                                     unsigned int line_number,
                                     unsigned int column_number,
                                     ULString source_id)
    {
        // Separate calls to qDebug will result in new lines so we re-use the same qDebug to prevent that
        auto dbg = qDebug();
        dbg << ulStringGetData(message);

        if (source == kMessageSource_JS)
        {
            dbg << " (" << ulStringGetData(source_id) << " @ line " << line_number << ", col " << column_number << ")";
        }
    }

    void _onViewDOMReady()
    {
        JSContextRef ctx = ulViewLockJSContext(m_view);
        std::string className = "MyClass";
        JSStringRef name = JSStringCreateWithUTF8CString(className.c_str());
        JSClassDefinition definition = kJSClassDefinitionEmpty;
        definition.className = className.c_str();
        JSClassRef classRef = JSClassCreate(&definition);
        JSObjectRef ctor = JSObjectMakeConstructor(ctx, classRef, 0);

        // Attach class constructor to global object
        JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
        JSObjectSetProperty(ctx, globalObj, name, ctor, 0, 0);

        // Cleanup
        JSClassRelease(classRef);
        JSStringRelease(name);
        ulViewUnlockJSContext(m_view);
    }

    void _onViewLoaded()
    {
        ULSurface surface = ulViewGetSurface(m_view);
        ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
        void *pixelBuffer = ulBitmapLockPixels(bitmap);
        m_img = new QImage(
            static_cast<const uchar *>(pixelBuffer),
            ulBitmapGetWidth(bitmap),
            ulBitmapGetHeight(bitmap),
            QImage::Format_ARGB32);
        ulBitmapUnlockPixels(bitmap);
        update(); // Queue paint event
    }

    void tick()
    {
        ulUpdate(m_renderer);
        ulRender(m_renderer);

        // Queue a paint event if Ultralight view has updated
        ULSurface surface = ulViewGetSurface(m_view);
        if (!ulIntRectIsEmpty(ulSurfaceGetDirtyBounds(surface)))
        {
            qDebug("QUEUE PAINT");
            update();
        }
    }
};

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
    KJS *kjs = new KJS(qwindow, basePath);
    kjs->move(200, 200);
    kjs->resize(640, 480);
    kjs->show();

    return;
}
