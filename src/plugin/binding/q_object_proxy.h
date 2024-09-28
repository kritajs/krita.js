#pragma once

#include <QMetaObject>
#include <QString>
#include <Ultralight/CAPI.h>

class QObjectProxy
{
public:
    JSObjectRef m_classObj;

    QObjectProxy(JSContextRef ctx, QMetaObject *mo);

private:
    QMetaObject *m_mo;
};
