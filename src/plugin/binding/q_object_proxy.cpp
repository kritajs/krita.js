#include <QDebug>
#include <QMetaMethod>
#include "q_object_proxy.h"

JSValueRef getProperty(JSContextRef ctx, JSClassRef jsClass, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    QMetaObject *mo = static_cast<QMetaObject *>(JSObjectGetPrivate(object));
    return JSValueMakeNumber(ctx, 5.0);
}

JSObjectRef callAsConstructor(JSContextRef ctx, JSClassRef jsClass, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    QMetaObject *mo = static_cast<QMetaObject *>(JSObjectGetPrivate(constructor));
    // This only works for QObjects that mark their constructor as Q_INVOKABLE.
    // The majority of them do not so we need to come up with another way of
    // creating instances.
    QObject *instance = mo->newInstance();
    return JSObjectMake(ctx, jsClass, instance);
};

QObjectProxy::QObjectProxy(JSContextRef ctx, QMetaObject *mo) : m_mo(mo)
{
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    // Use version 1000 so that the callbacks also receive `JSClassRef jsClass`
    definition.version = 1000;
    definition.getPropertyEx = &getProperty;
    definition.callAsConstructorEx = &callAsConstructor;
    JSClassRef classRef = JSClassCreate(&definition);
    m_classObj = JSObjectMake(ctx, classRef, m_mo);
    JSClassRelease(classRef);
}
