#pragma once

#include "view.h"
#include <QObject>
#include <Ultralight/CAPI.h>

class Renderer : public QObject {
    Q_OBJECT

  public:
    bool m_initialized;

    ~Renderer() override;
    void initialize(const char *_basePath);
    bool eventFilter(QObject *object, QEvent *event) override;
    View *createView(QWidget *parent);

  private:
    QList<View *> m_views;
    ULRenderer m_renderer;

    void tick();
};
