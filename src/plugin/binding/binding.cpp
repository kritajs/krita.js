#include <unordered_map>
#include <QDebug>
#include <QLibrary>
#include <QMetaType>
#include <QString>
#include "binding.h"
#include "qt_meta_object_proxy.h"

// Returns a JSObjectRef for the libkis/Qt class named by `propertyName`. Returns a JS undefined value if class is not found.
JSValueRef getProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    Binding *binding = static_cast<Binding *>(JSObjectGetPrivate(object));
    QString className = QString::fromWCharArray(JSStringGetCharactersPtr(propertyName), JSStringGetLength(propertyName));

    JSObjectRef ctor = binding->getCachedCtor(className);
    if (ctor)
    {
        return ctor;
    }

    // Find staticMetaObject by explicitly linking
    QMetaObject *mo = nullptr;
    for (const auto &libName : binding->m_libsToSearch)
    {
        QLibrary lib(libName);
        // Black magic - this relies on the Itanium ABI name mangling scheme
        QString staticMetaObjectSymbol = QString("_ZN%1%2").arg(className.size()).arg(className) + "16staticMetaObjectE";
        mo = (QMetaObject *)lib.resolve(staticMetaObjectSymbol.toLocal8Bit().constData());
        if (mo)
        {
            break;
        }
    }

    // Requested property was not found in any of the shared libraries. Just return undefined;
    if (!mo)
    {
        return JSValueMakeUndefined(ctx);
    }

    return JSValueMakeUndefined(ctx);

    // // Create and cache an instance of our class.
    // // Subsequent reads to get this class will re-use the same class instance.

    // // Note that we're exposing a class instance (JSObjectMake) instead of a
    // // constructor (JSObjectMakeConstructor). The class definition defines a
    // // callAsConstructor callback which is why you can do `new SomeQtClass()`
    // // in JS.

    // // I tried using JSObjectMakeConstructor but couldn't get private data
    // // working with it which is why we use a class instance instead.
    // QtMetaObjectProxy *proxy = new QtMetaObjectProxy(className, mo);
    // JSObjectRef classObj = JSObjectMake(ctx, proxy->m_classRef, NULL);
    // cache[className] = classObj;

    // return classObj;
}

Binding::Binding(JSContextRef ctx, QStringList libsToSearch) : m_libsToSearch(libsToSearch)
{
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    definition.getProperty = &getProperty;
    JSClassRef classRef = JSClassCreate(&definition);
    m_classObj = JSObjectMake(ctx, classRef, this);
    JSClassRelease(classRef);
}

Binding::~Binding()
{
    for (auto &[className, proxy] : m_cache)
    {
        delete proxy;
    }
}

JSObjectRef Binding::getCachedCtor(QString className)
{
    try
    {
        return m_cache.at(className)->m_ctor;
    }
    catch (const std::exception &e)
    {
        return nullptr;
    }
}
