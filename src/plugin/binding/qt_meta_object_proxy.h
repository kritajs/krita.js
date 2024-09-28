#pragma once

#include <QMetaObject>
#include <QString>
#include <Ultralight/CAPI.h>

class QtMetaObjectProxy
{
public:
    JSObjectRef m_classObj;

    QtMetaObjectProxy(JSContextRef ctx, QMetaObject *mo);

private:
    QMetaObject *m_mo;
};
