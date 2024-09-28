#include <QDebug>
#include <QPainter>
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
    qDeleteAll(m_bindings.begin(), m_bindings.end());
    m_bindings.clear();
}

void View::paintEvent(QPaintEvent *)
{
    if (!m_isReady)
    {
        return;
    }

    qDebug("PAINT EVENT");

    ULSurface surface = ulViewGetSurface(m_view);
    ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
    void *pixelBuffer = ulBitmapLockPixels(bitmap);
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), m_img);
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
    // Once the DOM is ready, we expose two classes on the global object:
    // - Q: allows JS to access any Qt class by using `Q.<className>`.
    //   For example, `const button = new Q.PushButton()`;
    // - K: allows JS to access any libkis class by using `K.<className>`.
    //   For example, `const krita = K.Krita.instance()`;

    JSContextRef ctx = ulViewLockJSContext(m_view);
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSPropertyAttributes attrs = kJSPropertyAttributeReadOnly || kJSPropertyAttributeDontDelete;

    // Create and expose the Q class on the global object. Only exposing
    // classes from the following Qt modules:
    // - Core
    // - GUI
    // - Widgets
    QStringList qtLibs = {"Qt5Core", "Qt5Gui", "Qt5Widgets"};
    Binding *qtBinding = new Binding(ctx, qtLibs);
    m_bindings.append(qtBinding);
    JSStringRef qClassName = JSStringCreateWithUTF8CString("Q");
    JSObjectSetProperty(ctx, globalObj, qClassName, qtBinding->m_classObj, attrs, NULL);
    JSStringRelease(qClassName);

    // Create and expose the K class on the global object.
    QStringList kritaLibs = {"libkritalibkis"};
    Binding *kritaBinding = new Binding(ctx, kritaLibs);
    m_bindings.append(kritaBinding);
    JSStringRef kritaClassName = JSStringCreateWithUTF8CString("K");
    JSObjectSetProperty(ctx, globalObj, kritaClassName, kritaBinding->m_classObj, attrs, NULL);
    JSStringRelease(kritaClassName);

    ulViewUnlockJSContext(m_view);
}

void View::_onViewLoaded()
{
    qDebug("VIEW LOADED");

    ULSurface surface = ulViewGetSurface(m_view);
    ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
    void *pixelBuffer = ulBitmapLockPixels(bitmap);
    m_img = QImage(
        static_cast<const uchar *>(pixelBuffer),
        ulBitmapGetWidth(bitmap),
        ulBitmapGetHeight(bitmap),
        QImage::Format_ARGB32);
    m_isReady = true;
    ulBitmapUnlockPixels(bitmap);
    update(); // Queue paint event
}

#include "moc_view.cpp"
