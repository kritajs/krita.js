#pragma once

#include <QMouseEvent>
#include <QWidget>
#include <Ultralight/CAPI.h>
#include "binding.h"

class View : public QWidget
{
    Q_OBJECT

public:
    ULView m_view;
    // Whether this view is ready to be rendered
    bool m_isReady = false;

    View(QWidget *parent, ULView view);
    ~View();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    static void onViewDOMReady(void *user_data,
                               ULView caller,
                               unsigned long long frame_id,
                               bool is_main_frame,
                               ULString url);

    static void onViewLoaded(void *user_data,
                             ULView caller,
                             unsigned long long frame_id,
                             bool is_main_frame,
                             ULString url);

    static void onViewConsoleMessage(void *user_data,
                                     ULView caller,
                                     ULMessageSource source,
                                     ULMessageLevel level,
                                     ULString message,
                                     unsigned int line_number,
                                     unsigned int column_number,
                                     ULString source_id);

private:
    QImage m_img;
    QList<Binding *> m_bindings;

    void _onViewDOMReady();
    void _onViewLoaded();
};
