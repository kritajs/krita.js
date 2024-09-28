#pragma once

#include <QString>
#include <QStringList>
#include <Ultralight/CAPI.h>
#include "qt_meta_object_proxy.h"

typedef QString (*TransformPropertyNameCallback)(const QString &requestedName);

class Binding
{
public:
    // The class object to be exposed on the global object
    JSObjectRef m_classObj;
    // A list of shared libraries to search when JS accesses any property on the class
    QStringList m_libsToSearch;
    // Callback to map the requested property name to a symbol (used when searching for a C++ class at runtime).
    // For example, if you wanted to expose QObject as Object, this callback should do:
    // `return QString("Q") + requestedName;`
    TransformPropertyNameCallback transformPropertyName;

    Binding(JSContextRef ctx, QStringList libsToSearch, TransformPropertyNameCallback transformPropertyNameCallback);
    ~Binding();

    QtMetaObjectProxy *getCachedProxy(QString key);
    void addProxyToCache(QString key, QtMetaObjectProxy *proxy);

private:
    std::unordered_map<QString, QtMetaObjectProxy *> m_classCache;
};
