#include "rectcanvas.h"
#include <QRandomGenerator>

// ─────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────
RectCanvas::RectCanvas(QWidget *parent)
    : QWidget(parent)
    , m_drawing(false)
    , m_dragStart(0, 0)
    , m_liveRect()
{
    setMinimumSize(700, 500);
    setWindowTitle("Mini Project 2: Rectangle Drawing Canvas");
    setMouseTracking(false);   // default — only track while button held

    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));
}

// ─────────────────────────────────────────────
// Paint Event — redraws EVERYTHING from scratch
// ─────────────────────────────────────────────
void RectCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false); // crisp pixel-perfect rects

    // 1. Draw all completed rectangles
    for (const DrawnRect &dr : m_rects) {
        painter.setBrush(QBrush(dr.color));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(dr.rect);
    }

    // 2. Draw the live rubber-band rectangle (only while dragging)
    if (m_drawing && m_liveRect.isValid()) {
        QPen livePen(Qt::darkGray);
        livePen.setStyle(Qt::DashLine);
        livePen.setWidth(1);

        painter.setPen(livePen);
        painter.setBrush(QBrush(QColor(100, 100, 255, 60))); // semi-transparent blue
        painter.drawRect(m_liveRect);
    }
}

// ─────────────────────────────────────────────
// Mouse Press — begin drawing
// ─────────────────────────────────────────────
void RectCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_drawing   = true;
        m_dragStart = event->pos();           // record start point
        m_liveRect  = QRect(m_dragStart, m_dragStart); // zero-size rect at click
        update();
    }
}

// ─────────────────────────────────────────────
// Mouse Move — update live preview
// ─────────────────────────────────────────────
void RectCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_drawing) return;

    // Build rect from start → current pos, normalized so width/height always positive
    m_liveRect = QRect(m_dragStart, event->pos()).normalized();
    update();
}

// ─────────────────────────────────────────────
// Mouse Release — finalize the rectangle
// ─────────────────────────────────────────────
void RectCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_drawing) return;

    m_drawing = false;

    QRect finalRect = QRect(m_dragStart, event->pos()).normalized();

    // Ignore accidental tiny clicks (less than 5x5 pixels)
    if (finalRect.width() >= 5 && finalRect.height() >= 5) {
        DrawnRect dr;
        dr.rect  = finalRect;
        dr.color = randomColor();
        m_rects.append(dr);
    }

    m_liveRect = QRect(); // reset — isValid() will now return false
    update();
}

// ─────────────────────────────────────────────
// Helper — pick a random pleasant color
// ─────────────────────────────────────────────
QColor RectCanvas::randomColor() const
{
    static const QList<QColor> palette = {
        QColor(255, 99,  99,  180),   // red,    semi-transparent
        QColor(99,  180, 255, 180),   // blue
        QColor(99,  220, 99,  180),   // green
        QColor(255, 200, 80,  180),   // yellow
        QColor(200, 99,  255, 180),   // purple
        QColor(255, 160, 80,  180),   // orange
    };

    int index = QRandomGenerator::global()->bounded(palette.size());
    return palette[index];
}