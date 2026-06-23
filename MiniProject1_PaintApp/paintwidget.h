#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QList>
#include <QPoint>

class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QList<QPoint> m_currentStroke;       // points in the stroke being drawn right now
    QList<QList<QPoint>> m_allStrokes;   // all completed strokes
    bool m_drawing;                       // true while mouse button is held
};

#endif // PAINTWIDGET_H