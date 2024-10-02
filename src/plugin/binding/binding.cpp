#include <unordered_map>
#include <QDebug>
#include <QLibrary>
#include "binding.h"
#include "q_object_proxy.h"

// Returns a constructor object for the class specified by propertyName.
// Returns a JS undefined value if class is not found in any of the searched libraries.
JSValueRef getProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    Binding *binding = static_cast<Binding *>(JSObjectGetPrivate(object));
    QString className = QString::fromWCharArray(JSStringGetCharactersPtr(propertyName), JSStringGetLength(propertyName));
    className = binding->transformPropertyName(className);

    QObjectProxy *cachedProxy = binding->getCachedProxy(className);
    if (cachedProxy)
    {
        return cachedProxy->m_classObj;
    }

    // Find staticMetaObject by explicitly linking
    QMetaObject *mo = nullptr;
    for (const auto &libName : binding->m_libsToSearch)
    {
        QLibrary lib(libName);
        // Black magic - this relies on the Itanium ABI name mangling scheme
        QString staticMetaObjectSymbol = QString("_ZN%1%2")
                                             .arg(className.size())
                                             .arg(className) +
                                         "16staticMetaObjectE";
        mo = (QMetaObject *)lib.resolve(staticMetaObjectSymbol.toLocal8Bit().constData());
        if (mo)
        {
            break;
        }
    }

    // Requested property was not found in any of the searched libraries. Just return undefined;
    if (!mo)
    {
        return JSValueMakeUndefined(ctx);
    }

    QObjectProxy *proxy = new QObjectProxy(ctx, mo);
    binding->addProxyToCache(className, proxy);
    return proxy->m_classObj;
}

Binding::Binding(JSContextRef ctx,
                 QStringList libsToSearch,
                 TransformPropertyNameCallback transformPropertyNameCallback)
    : m_libsToSearch(libsToSearch),
      transformPropertyName(transformPropertyNameCallback)
{
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.getProperty = &getProperty;
    JSClassRef classRef = JSClassCreate(&definition);
    m_classObj = JSObjectMake(ctx, classRef, this);
    JSClassRelease(classRef);
}

Binding::~Binding()
{
    for (auto &[className, proxy] : m_classCache)
    {
        delete proxy;
    }
}

QObjectProxy *Binding::getCachedProxy(QString key)
{
    try
    {
        return m_classCache.at(key);
    }
    catch (const std::exception &e)
    {
        return nullptr;
    }
}

void Binding::addProxyToCache(QString key, QObjectProxy *proxy)
{
    m_classCache[key] = proxy;
}
