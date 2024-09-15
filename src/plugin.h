#pragma once

#include <QObject>

class KritaJsPlugin : public QObject {
    Q_OBJECT
public:
    KritaJsPlugin(QObject *parent, const QVariantList&);
    ~KritaJsPlugin() {}
};
