#pragma once

#include <QString>
#include <QStringList>
#include <Ultralight/CAPI.h>
#include "qt_meta_object_proxy.h"

class Binding
{
public:
    JSObjectRef m_classObj;
    QStringList m_libsToSearch;

    Binding(JSContextRef ctx, QStringList libsToSearch);
    ~Binding();

    JSObjectRef getCachedCtor(QString className);

private:
    std::unordered_map<QString, QtMetaObjectProxy *> m_cache;
};
