#pragma once

#include <QObject>
#include <Ultralight/CAPI.h>
#include "view.h"

class Renderer : public QObject
{
    Q_OBJECT

public:
    Renderer(QObject *parent, const char *_basePath);
    ~Renderer();
    View *createView(QWidget *parent);

private:
    QList<View *> m_views;
    ULRenderer m_renderer;

    void tick();
};
