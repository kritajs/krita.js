#include <unordered_map>
#include <windows.h>
#include <QDebug>
#include <QMetaType>
#include <QString>
#include "binding.h"
#include "qt_meta_object_proxy.h"

std::unordered_map<QString, JSObjectRef> cache;

JSValueRef getClassConstructor(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    QString className = QString::fromWCharArray(JSStringGetCharactersPtr(propertyName), JSStringGetLength(propertyName));

    try
    {
        return cache.at(className);
    }
    catch (const std::exception &e)
    {
        qDebug() << className << "not found in cache.";
    }

    // Find staticMetaObject in libkis/Qt DLLs
    HINSTANCE lib = GetModuleHandle(TEXT("Qt5Core.dll"));
    QMetaObject *mo = nullptr;
    if (lib)
    {
        // Black magic - this relies on the Itanium ABI name mangling scheme
        QString fnName = QString("_ZN%1%2").arg(className.size()).arg(className) + "16staticMetaObjectE";
        mo = (QMetaObject *)GetProcAddress(lib, fnName.toLocal8Bit().constData());
    }

    // Requested property is not a libkis/Qt class. Ignore it.
    if (!mo)
    {
        return JSValueMakeUndefined(ctx);
    }

    // Create and cache an instance of our class.
    // Subsequent reads to get this class will re-use the same class instance.
    QtMetaObjectProxy *proxy = new QtMetaObjectProxy(className, mo);
    JSObjectRef classObj = JSObjectMake(ctx, proxy->m_classRef, NULL);
    cache[className] = classObj;

    return classObj;
}
