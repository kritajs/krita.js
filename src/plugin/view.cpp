#include "view.h"
#include "binding.h"
#include <QBitmap>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

View::View(QWidget *parent, ULView view) : QWidget(parent), m_view(view) {
    // Load content into the view
    ULString entryUrl = ulCreateString("file:///index.html");
    ulViewLoadURL(m_view, entryUrl);
    ulDestroyString(entryUrl);

    // Register callbacks
    auto onViewDOMReady = [](void *user_data, ULView caller,
                             unsigned long long frame_id, bool is_main_frame,
                             ULString url) {
        View *view = static_cast<View *>(user_data);
        view->handleViewDOMReady();
    };
    ulViewSetDOMReadyCallback(m_view, onViewDOMReady, this);

    auto onViewLoaded = [](void *user_data, ULView caller,
                           unsigned long long frame_id, bool is_main_frame,
                           ULString url) {
        if (is_main_frame) {
            View *view = static_cast<View *>(user_data);
            view->handleViewLoaded();
        }
    };
    ulViewSetFinishLoadingCallback(m_view, onViewLoaded, this);

    auto onViewConsoleMessage =
        [](void *user_data, ULView caller, ULMessageSource source,
           ULMessageLevel level, ULString message, unsigned int line_number,
           unsigned int column_number, ULString source_id) {
            // Separate calls to qDebug will result in new lines so we re-use
            // the same qDebug to prevent that
            auto dbg = qDebug();
            dbg << ulStringGetData(message);

            if (source == kMessageSource_JS) {
                dbg << " (" << ulStringGetData(source_id) << " @ line "
                    << line_number << ", col " << column_number << ")";
            }
        };
    ulViewSetAddConsoleMessageCallback(m_view, onViewConsoleMessage, this);

    // Enable hover events
    setAttribute(Qt::WA_Hover);
}

View::~View() {
    ulDestroyView(m_view);
    qDeleteAll(m_bindings.begin(), m_bindings.end());
    m_bindings.clear();
}

bool View::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::MouseButtonPress:
        handleMouseEvent(static_cast<QMouseEvent *>(e)->pos(),
                         kMouseEventType_MouseDown);
        return true;

    case QEvent::MouseButtonRelease:
        handleMouseEvent(static_cast<QMouseEvent *>(e)->pos(),
                         kMouseEventType_MouseUp);
        return true;

    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
        handleMouseEvent(static_cast<QHoverEvent *>(e)->pos(),
                         kMouseEventType_MouseMoved);
        return true;

    default:
        break;
    }
    return QWidget::event(e);
}

void View::paintEvent(QPaintEvent *) {
    if (!m_isReady)
        return;

    qDebug("PAINT EVENT");

    ULSurface surface = ulViewGetSurface(m_view);
    ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
    void *pixelBuffer = ulBitmapLockPixels(bitmap);
    unsigned int bytesPerRow = ulBitmapGetRowBytes(bitmap);
    QImage img = QImage(static_cast<const uchar *>(pixelBuffer),
                        ulBitmapGetWidth(bitmap), ulBitmapGetHeight(bitmap),
                        bytesPerRow, QImage::Format_ARGB32);
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), img);
    setMask(QPixmap::fromImage(img).mask());
    ulBitmapUnlockPixels(bitmap);
    ulSurfaceClearDirtyBounds(surface);
}

void View::handleViewDOMReady() {
    // Once the DOM is ready, we expose two classes on the global object:
    // - Q: allows JS to access any Qt class by using `Q.<className>`.
    //   For example, `const button = new Q.PushButton()`;
    // - K: allows JS to access any libkis class by using `K.<className>`.
    //   For example, `const krita = K.Krita.instance()`;

    JSContextRef ctx = ulViewLockJSContext(m_view);
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSPropertyAttributes attrs =
        kJSPropertyAttributeReadOnly || kJSPropertyAttributeDontDelete;

    // Create and expose the Q class on the global object. Only exposing
    // classes from the following Qt modules:
    // - Core
    // - GUI
    // - Widgets
    QStringList qtLibs = {"Qt5Core", "Qt5Gui", "Qt5Widgets"};
    auto getQtClassName = [](const QString &requestedName) {
        // We want to expose Qt classes to JS without the "Q" prefix so we add
        // the "Q" back in when the Binding class needs to search for the
        // requested Qt class
        return QString("Q") + requestedName;
    };
    Binding *qtBinding = new Binding(ctx, qtLibs, getQtClassName);
    m_bindings.append(qtBinding);
    JSStringRef qClassName = JSStringCreateWithUTF8CString("Q");
    JSObjectSetProperty(ctx, globalObj, qClassName, qtBinding->m_classObj,
                        attrs, NULL);
    JSStringRelease(qClassName);

    // Create and expose the K class on the global object.
    QStringList kritaLibs = {"libkritalibkis"};
    auto getKritaClassName = [](const QString &requestedName) {
        return requestedName;
    };
    Binding *kritaBinding = new Binding(ctx, kritaLibs, getKritaClassName);
    m_bindings.append(kritaBinding);
    JSStringRef kritaClassName = JSStringCreateWithUTF8CString("K");
    JSObjectSetProperty(ctx, globalObj, kritaClassName,
                        kritaBinding->m_classObj, attrs, NULL);
    JSStringRelease(kritaClassName);

    ulViewUnlockJSContext(m_view);
}

void View::handleViewLoaded() {
    qDebug("VIEW LOADED");

    m_isReady = true;
    update(); // Queue paint event
}

void View::handleMouseEvent(const QPoint &pos, ULMouseEventType type) {
    ULMouseEvent e =
        ulCreateMouseEvent(type, pos.x(), pos.y(), kMouseButton_None);
    ulViewFireMouseEvent(m_view, e);
    ulDestroyMouseEvent(e);
}

#include "moc_view.cpp"
