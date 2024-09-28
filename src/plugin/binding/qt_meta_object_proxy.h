#pragma once

#include <QMetaObject>
#include <QString>
#include <Ultralight/CAPI.h>

class QtMetaObjectProxy
{
public:
    QMetaObject *m_mo;
    JSObjectRef m_ctor;

    QtMetaObjectProxy(QString className, QMetaObject *mo);
    ~QtMetaObjectProxy();

private:
    JSClassRef m_classRef;
};
