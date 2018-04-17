#include "renderwidget.h"

#include <QApplication>
#include <QPainter>

RenderWidget::RenderWidget(QWidget* parent) :
    QWidget(parent)
    , m_logo{":/images/PSXLogo.png", "png"}
{
    Q_INIT_RESOURCE(resources);
}

void RenderWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter{this};
    painter.fillRect(rect(), Qt::black);

    const int rectHeight = rect().height();
    const int rectWidth = rect().width();

    const QImage scaledLogo = m_logo.scaled(rectWidth, rectHeight, Qt::KeepAspectRatio);

    const int imgHeight = scaledLogo.height();
    const int imgWidth = scaledLogo.width();

    painter.drawPixmap(
        QPoint{(rectWidth - imgWidth) / 2, (rectHeight - imgHeight) / 2}, 
        QPixmap::fromImage(scaledLogo));
}
