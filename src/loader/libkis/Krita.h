#pragma once

#include <QObject>
#include <QString>

class Krita : public QObject
{
public:
    QString version() const;
    static Krita* instance();
};
