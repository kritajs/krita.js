#pragma once

#include "q_object_proxy.h"
#include <QString>
#include <QStringList>
#include <Ultralight/CAPI.h>
#include <memory>

using namespace std;
using TransformPropertyNameCallback = QString (*)(const QString &);

class Binding {
  public:
    // The class object to be exposed on the global object
    JSObjectRef m_classObj;
    // A list of shared libraries to search when JS accesses any property on the
    // class
    QStringList m_libsToSearch;
    // Callback to map the requested property name to a symbol (used when
    // searching for a C++ class at runtime). For example, if you wanted to
    // expose QObject as Object, this callback should do: `return QString("Q") +
    // requestedName;`
    TransformPropertyNameCallback transformPropertyName;

    Binding(JSContextRef ctx, QStringList libsToSearch,
            TransformPropertyNameCallback transformPropertyNameCallback);
    ~Binding();

    shared_ptr<QObjectProxy> getCachedProxy(QString key);
    void addProxyToCache(QString key, shared_ptr<QObjectProxy> proxy);

  private:
    unordered_map<QString, shared_ptr<QObjectProxy>> m_classCache;
};
