#pragma once

#include <QMetaObject>
#include <QString>
#include <Ultralight/CAPI.h>

class QtMetaObjectProxy
{
public:
    QMetaObject *m_mo;
    JSClassRef m_classRef;

    QtMetaObjectProxy(QString className, QMetaObject *mo) : m_mo(mo)
    {
        JSClassDefinition definition = kJSClassDefinitionEmpty;
        definition.version = 1000;
        definition.className = className.toLocal8Bit().constData();

        // Get property
        definition.getPropertyEx = [](JSContextRef ctx, JSClassRef jsClass, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
        {
            QtMetaObjectProxy *proxy = (QtMetaObjectProxy *)JSClassGetPrivate(jsClass);
            return JSValueMakeNumber(ctx, 5.0);
        };

        // Constructor
        definition.callAsConstructorEx = [](JSContextRef ctx, JSClassRef jsClass, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
        {
            QtMetaObjectProxy *proxy = (QtMetaObjectProxy *)JSClassGetPrivate(jsClass);
            void *instance = proxy->m_mo->newInstance();
            return JSObjectMake(ctx, jsClass, instance);
        };

        // Create the class and pass a pointer to this QtMetaObjectProxy.
        // We do this so that the callbacks above have a way to access this instance's data
        // such as the QMetaObject.
        m_classRef = JSClassCreate(&definition);
        JSClassSetPrivate(m_classRef, this);
    }

    ~QtMetaObjectProxy()
    {
        JSClassRelease(m_classRef);
    }
};
