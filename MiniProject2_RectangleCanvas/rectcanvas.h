#ifndef RECTCANVAS_H
#define RECTCANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QList>
#include <QRect>
#include <QColor>

// Represents one completed rectangle on the canvas
struct DrawnRect {
    QRect  rect;
    QColor color;
};

class RectCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit RectCanvas(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event)        override;
    void mousePressEvent(QMouseEvent *event)   override;
    void mouseMoveEvent(QMouseEvent *event)    override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // --- State Machine ---
    bool   m_drawing;      // true while left button is held
    QPoint m_dragStart;    // where the drag began (screen coords)
    QRect  m_liveRect;     // current rubber-band rect (updated every mouseMoveEvent)

    // --- Completed Rectangles ---
    QList<DrawnRect> m_rects;

    // --- Helpers ---
    QColor randomColor() const;
};

#endif // RECTCANVAS_H