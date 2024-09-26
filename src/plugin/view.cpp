#include <QDebug>
#include <QPainter>
#include "binding.h"
#include "view.h"

View::View(QWidget *parent, ULView view) : QWidget(parent), m_view(view)
{
    // Load content into the view
    ULString entryUrl = ulCreateString("file:///index.html");
    ulViewLoadURL(m_view, entryUrl);
    ulDestroyString(entryUrl);

    // Register callbacks
    ulViewSetDOMReadyCallback(m_view, &onViewDOMReady, this);
    ulViewSetFinishLoadingCallback(m_view, &onViewLoaded, this);
    ulViewSetAddConsoleMessageCallback(m_view, &onViewConsoleMessage, this);

    // Enable mouse move events
    setMouseTracking(true);
}

View::~View()
{
    ulDestroyView(m_view);
    delete m_img;
}

void View::paintEvent(QPaintEvent *)
{
    // m_img will be set once the view is loaded. Skip painting before that has happened.
    if (m_img == nullptr)
    {
        return;
    }

    qDebug("PAINT EVENT");

    ULSurface surface = ulViewGetSurface(m_view);
    ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
    void *pixelBuffer = ulBitmapLockPixels(bitmap);
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), *m_img);
    ulBitmapUnlockPixels(bitmap);
    ulSurfaceClearDirtyBounds(surface);
}

void View::mousePressEvent(QMouseEvent *event)
{
    ULMouseEvent e = ulCreateMouseEvent(kMouseEventType_MouseDown, event->x(), event->y(), kMouseButton_None);
    ulViewFireMouseEvent(m_view, e);
    ulDestroyMouseEvent(e);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    ULMouseEvent e = ulCreateMouseEvent(kMouseEventType_MouseUp, event->x(), event->y(), kMouseButton_None);
    ulViewFireMouseEvent(m_view, e);
    ulDestroyMouseEvent(e);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    ULMouseEvent e = ulCreateMouseEvent(kMouseEventType_MouseMoved, event->x(), event->y(), kMouseButton_None);
    ulViewFireMouseEvent(m_view, e);
    ulDestroyMouseEvent(e);
}

void View::onViewDOMReady(void *user_data,
                          ULView caller,
                          unsigned long long frame_id,
                          bool is_main_frame,
                          ULString url)
{
    View *view = static_cast<View *>(user_data);
    view->_onViewDOMReady();
}

void View::onViewLoaded(void *user_data,
                        ULView caller,
                        unsigned long long frame_id,
                        bool is_main_frame,
                        ULString url)
{
    if (is_main_frame)
    {
        View *view = static_cast<View *>(user_data);
        view->_onViewLoaded();
    }
}

void View::onViewConsoleMessage(void *user_data,
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

void View::_onViewDOMReady()
{
    qDebug("DOM READY");

    JSContextRef ctx = ulViewLockJSContext(m_view);
    std::string className = "Qt5Core";
    JSStringRef name = JSStringCreateWithUTF8CString(className.c_str());
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.className = className.c_str();
    definition.getProperty = &getClassConstructor;
    JSClassRef classRef = JSClassCreate(&definition);
    JSObjectRef ctor = JSObjectMakeConstructor(ctx, classRef, NULL);

    // Attach class constructor to global object
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSObjectSetProperty(ctx, globalObj, name, ctor, NULL, NULL);

    // Cleanup
    JSClassRelease(classRef);
    JSStringRelease(name);
    ulViewUnlockJSContext(m_view);
}

void View::_onViewLoaded()
{
    qDebug("VIEW LOADED");

    ULSurface surface = ulViewGetSurface(m_view);
    ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
    void *pixelBuffer = ulBitmapLockPixels(bitmap);
    m_img = new QImage(
        static_cast<const uchar *>(pixelBuffer),
        ulBitmapGetWidth(bitmap),
        ulBitmapGetHeight(bitmap),
        QImage::Format_ARGB32);
    m_isReady = true;
    ulBitmapUnlockPixels(bitmap);
    update(); // Queue paint event
}

#include "moc_view.cpp"
