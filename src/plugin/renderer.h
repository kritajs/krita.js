#pragma once

#include "view.h"
#include <QObject>
#include <Ultralight/CAPI.h>

class Renderer : public QObject {
    Q_OBJECT

  public:
    Renderer(QObject *parent, const char *_basePath);
    ~Renderer();
    bool eventFilter(QObject *object, QEvent *event);
    View *createView(QWidget *parent);

  private:
    QList<View *> m_views;
    ULRenderer m_renderer;

    void tick();
};
