#include "q_object_proxy.h"
#include <QDebug>

JSValueRef getProperty(JSContextRef ctx, JSClassRef jsClass, JSObjectRef object,
                       JSStringRef propertyName, JSValueRef *exception) {
    auto *mo = static_cast<QMetaObject *>(JSObjectGetPrivate(object));
    return JSValueMakeNumber(ctx, 0);
}

JSObjectRef callAsConstructor(JSContextRef ctx, JSClassRef jsClass,
                              JSObjectRef constructor, size_t argumentCount,
                              // NOLINTNEXTLINE
                              const JSValueRef arguments[],
                              JSValueRef *exception) {
    auto *mo = static_cast<QMetaObject *>(JSObjectGetPrivate(constructor));
    // This only works for QObjects that mark their constructor as Q_INVOKABLE.
    // The majority of them do not so we need to come up with another way of
    // creating instances.
    QObject *instance = mo->newInstance();
    return JSObjectMake(ctx, jsClass, instance);
};

QObjectProxy::QObjectProxy(JSContextRef ctx, QMetaObject *mo) : m_mo(mo) {
    JSClassDefinition definition = kJSClassDefinitionEmpty;
    // Use version 1000 so that the callbacks also receive `JSClassRef jsClass`
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    definition.version = 1000;
    definition.getPropertyEx = &getProperty;
    definition.callAsConstructorEx = &callAsConstructor;
    JSClassRef classRef = JSClassCreate(&definition);
    m_classObj = JSObjectMake(ctx, classRef, m_mo);
    JSClassRelease(classRef);
}
