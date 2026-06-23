#include "paintwidget.h"

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget(parent), m_drawing(false)
{
    setMinimumSize(600, 400);
    setWindowTitle("Mini Project 1: Simple Paint");

    // White background instead of default gray
    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));
}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);   // rounded line ends — smoother strokes
    pen.setJoinStyle(Qt::RoundJoin); // rounded corners between segments
    painter.setPen(pen);

    // Draw all completed strokes
    for (const QList<QPoint> &stroke : m_allStrokes) {
        for (int i = 1; i < stroke.size(); ++i) {
            painter.drawLine(stroke[i - 1], stroke[i]);
        }
    }

    // Draw the stroke currently being drawn
    for (int i = 1; i < m_currentStroke.size(); ++i) {
        painter.drawLine(m_currentStroke[i - 1], m_currentStroke[i]);
    }
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_currentStroke.clear();
        m_currentStroke.append(event->pos());
        update();
    }
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drawing) {
        m_currentStroke.append(event->pos());
        update();
    }
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_drawing) {
        m_currentStroke.append(event->pos());
        m_allStrokes.append(m_currentStroke);
        m_currentStroke.clear();
        m_drawing = false;
        update();
    }
}