#pragma once

#include <QMainWindow>
#include <QObject>
#include <qobject.h>

class Window : public QObject {
  public:
    [[nodiscard]] QMainWindow *qwindow() const;
};

class Krita : public QObject {
  public:
    [[nodiscard]] Window *activeWindow() const;
    static Krita *instance();
};
