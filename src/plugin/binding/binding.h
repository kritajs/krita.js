#pragma once

#include <QString>
#include <QStringList>
#include <Ultralight/CAPI.h>
#include "qt_meta_object_proxy.h"

typedef QString (*GetClassNameCallback)(const QString &requestedName);

class Binding
{
public:
    JSObjectRef m_classObj;
    QStringList m_libsToSearch;
    GetClassNameCallback m_getClassNameCallback;

    Binding(JSContextRef ctx, QStringList libsToSearch, GetClassNameCallback getClassNameCallback);
    ~Binding();

    JSObjectRef getCachedCtor(QString className);

private:
    std::unordered_map<QString, QtMetaObjectProxy *> m_cache;
};
