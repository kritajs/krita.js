#pragma once

#include <QWidget>
#include <Ultralight/CAPI.h>

class Binding;
class QEvent;

class View : public QWidget {
    Q_OBJECT

  public:
    ULView m_view;
    // Whether this view is ready to be rendered
    bool m_isReady = false;

    View(QWidget *parent, ULView view);
    ~View();

  protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);

  private:
    QList<Binding *> m_bindings;

    void handleViewDOMReady();
    void handleViewLoaded();
    void handleMouseEvent(const QPoint &pos, ULMouseEventType type);
};
