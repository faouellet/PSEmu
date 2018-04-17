#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QWidget>

#include <memory>

class QPaintEvent;

class RenderWidget final : public QWidget
{
public:
    RenderWidget(QWidget* parent = nullptr);

public: // Qt interface
    virtual void paintEvent(QPaintEvent* event);

private:
    QImage m_logo;
};

#endif // RENDER_WIDGET_H