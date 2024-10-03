#include "binding.h"
#include "q_object_proxy.h"
#include <QDebug>
#include <QLibrary>
#include <memory>
#include <unordered_map>
#include <utility>

using namespace std;

// Returns a constructor object for the class specified by propertyName. Returns
// a JS undefined value if class is not found in any of the searched libraries.
JSValueRef getProperty(JSContextRef ctx, JSObjectRef object,
                       JSStringRef propertyName, JSValueRef *exception) {
    auto *binding = static_cast<Binding *>(JSObjectGetPrivate(object));
    QString className = binding->transformPropertyName(
        QString::fromWCharArray(JSStringGetCharactersPtr(propertyName),
                                // NOLINTNEXTLINE
                                JSStringGetLength(propertyName)));

    shared_ptr<QObjectProxy> cachedProxy = binding->getCachedProxy(className);
    if (cachedProxy) {
        return cachedProxy->m_classObj;
    }

    // Find staticMetaObject by explicitly linking
    QMetaObject *mo = nullptr;
    for (const auto &libName : binding->m_libsToSearch) {
        QLibrary lib(libName);
        // Black magic - this relies on the Itanium ABI name mangling scheme
        QString staticMetaObjectSymbol =
            QString("_ZN%1%2").arg(className.size()).arg(className) +
            "16staticMetaObjectE";
        // NOLINTNEXTLINE
        mo = (QMetaObject *)lib.resolve(
            staticMetaObjectSymbol.toLocal8Bit().constData());
        if (mo) {
            break;
        }
    }

    // Requested property was not found in any of the searched libraries. Just
    // return undefined;
    if (!mo) {
        return JSValueMakeUndefined(ctx);
    }

    auto proxy = make_shared<QObjectProxy>(ctx, mo);
    binding->addProxyToCache(className, proxy);
    return proxy->m_classObj;
}

Binding::Binding(JSContextRef ctx, QStringList libsToSearch,
                 TransformPropertyNameCallback transformPropertyNameCallback)
    : m_libsToSearch(move(libsToSearch)),
      transformPropertyName(transformPropertyNameCallback) {
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.getProperty = &getProperty;
    JSClassRef classRef = JSClassCreate(&definition);
    m_classObj = JSObjectMake(ctx, classRef, this);
    JSClassRelease(classRef);
}

Binding::~Binding() { m_classCache.clear(); }

shared_ptr<QObjectProxy> Binding::getCachedProxy(QString key) {
    try {
        return m_classCache.at(key);
    } catch (const exception &e) {
        return nullptr;
    }
}

void Binding::addProxyToCache(QString key, shared_ptr<QObjectProxy> proxy) {
    m_classCache[key] = proxy;
}
