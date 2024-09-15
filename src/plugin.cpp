#include <kpluginfactory.h>

#include "plugin.h"

K_PLUGIN_FACTORY_WITH_JSON(KritaJsPluginFactory, "kritajs.json", registerPlugin<KritaJsPlugin>();)

KritaJsPlugin::KritaJsPlugin(QObject *parent, const QVariantList&) : QObject(parent) {
    qWarning() << "HELLO FROM KRITA.JS QT C++ PLUGIN";
}

#include "plugin.moc"
