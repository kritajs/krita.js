#include <QDebug>
#include <QMetaMethod>
#include "qt_meta_object_proxy.h"

JSValueRef getProperty(JSContextRef ctx, JSClassRef jsClass, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    QtMetaObjectProxy *proxy = static_cast<QtMetaObjectProxy *>(JSClassGetPrivate(jsClass));
    // proxy->m_mo->
    // JSObjectMakeFunctionWithCallback
    return JSValueMakeNumber(ctx, 5.0);
}

JSObjectRef callAsConstructor(JSContextRef ctx, JSClassRef jsClass, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    QtMetaObjectProxy *proxy = static_cast<QtMetaObjectProxy *>(JSClassGetPrivate(jsClass));
    void *instance = proxy->m_mo->newInstance();
    return JSObjectMake(ctx, jsClass, instance);
};

QtMetaObjectProxy::QtMetaObjectProxy(QString className, QMetaObject *mo) : m_mo(mo)
{
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    // Use version 1000 so that we can use the Ex versions of the callbacks.
    // These include the JSClassRef as an argument.
    definition.version = 1000;
    definition.className = className.toLocal8Bit().constData();
    definition.getPropertyEx = &getProperty;
    definition.callAsConstructorEx = &callAsConstructor;

    for (int i = m_mo->methodOffset(); i < m_mo->methodCount(); ++i)
    {
        QMetaMethod method = m_mo->method(i);
        if (method.access() == QMetaMethod::Public)
        {
            qDebug() << method.name() << " " << method.methodType();
        }
    }

    // Create the class and pass a pointer to this proxy.
    // We do this so that the callbacks above have a way to access the proxy's data
    // such as the QMetaObject.
    m_classRef = JSClassCreate(&definition);
    JSClassSetPrivate(m_classRef, this);
}

QtMetaObjectProxy::~QtMetaObjectProxy()
{
    JSClassRelease(m_classRef);
}
