#include <QDebug>
#include <QMetaMethod>
#include "qt_meta_object_proxy.h"

JSValueRef getProperty(JSContextRef ctx, JSClassRef jsClass, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    QMetaObject *mo = static_cast<QMetaObject *>(JSObjectGetPrivate(object));
    return JSValueMakeNumber(ctx, 5.0);
}

JSObjectRef callAsConstructor(JSContextRef ctx, JSClassRef jsClass, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    QMetaObject *mo = static_cast<QMetaObject *>(JSObjectGetPrivate(constructor));
    void *instance = mo->newInstance();
    return JSObjectMake(ctx, jsClass, instance);
};

QtMetaObjectProxy::QtMetaObjectProxy(JSContextRef ctx, QMetaObject *mo) : m_mo(mo)
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
