#include "dragcanvas.h"

// ── Constructor ──────────────────────────────────────────────────────
DragCanvas::DragCanvas(QWidget *parent)
    : QWidget(parent)
    , m_dragOver(false)
{
    setAcceptDrops(true);              // REQUIRED: opt in as drop target
    setMinimumSize(500, 450);

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(245, 245, 245));  // light gray canvas
    setPalette(pal);
}

// ── paintEvent ────────────────────────────────────────────────────────
void DragCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    drawGrid(p);                           // layer 1: background grid

    for (const DroppedWidget &w : m_widgets)
        drawWidget(p, w);                  // layer 2: all dropped widgets

    if (m_dragOver)
        drawDropPreview(p);                // layer 3: hover preview (topmost)
}

// ── dragEnterEvent ────────────────────────────────────────────────────
void DragCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        m_dragOver = true;
        m_dragType = event->mimeData()->text();
        m_dragPos  = event->position().toPoint();
        event->acceptProposedAction();     // "yes, I accept this drop"
        update();
    } else {
        event->ignore();                   // unknown data type → reject
    }
}

// ── dragMoveEvent ─────────────────────────────────────────────────────
void DragCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        m_dragPos = event->position().toPoint();   // update preview position
        event->acceptProposedAction();
        update();                          // repaint to move the preview rect
    } else {
        event->ignore();
    }
}

// ── dragLeaveEvent ────────────────────────────────────────────────────
void DragCanvas::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    m_dragOver = false;                    // hide preview
    m_dragType.clear();
    update();
}

// ── dropEvent ─────────────────────────────────────────────────────────
void DragCanvas::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasText()) {
        event->ignore();
        return;
    }

    QString type = event->mimeData()->text();
    QPoint  pos  = event->position().toPoint();

    // Create new dropped widget centered on drop position
    DroppedWidget w;
    w.type  = type;
    w.color = colorForType(type);
    w.rect  = QRect(pos.x() - 50, pos.y() - 20, 100, 40); // 100×40, centered on cursor

    m_widgets.append(w);

    // Clean up hover state
    m_dragOver = false;
    m_dragType.clear();

    event->acceptProposedAction();
    update();
}

// ── drawGrid ──────────────────────────────────────────────────────────
void DragCanvas::drawGrid(QPainter &p) const
{
    QPen gridPen(QColor(220, 220, 220));
    p.setPen(gridPen);

    for (int x = 0; x < width(); x += 20)
        p.drawLine(x, 0, x, height());

    for (int y = 0; y < height(); y += 20)
        p.drawLine(0, y, width(), y);
}

// ── drawWidget ────────────────────────────────────────────────────────
void DragCanvas::drawWidget(QPainter &p, const DroppedWidget &w) const
{
    // Fill
    p.setBrush(QBrush(w.color));
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(w.rect);

    // Label — centered, black text
    p.setPen(Qt::black);
    p.drawText(w.rect, Qt::AlignCenter, w.type);
}

// ── drawDropPreview ───────────────────────────────────────────────────
void DragCanvas::drawDropPreview(QPainter &p) const
{
    QRect previewRect(m_dragPos.x() - 50, m_dragPos.y() - 20, 100, 40);

    QPen pen(QColor(80, 80, 255));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    p.setPen(pen);
    p.setBrush(QBrush(QColor(80, 80, 255, 40)));  // semi-transparent blue fill
    p.drawRect(previewRect);

    // Label inside preview
    p.setPen(QColor(60, 60, 200));
    p.drawText(previewRect, Qt::AlignCenter, m_dragType);
}

// ── colorForType ──────────────────────────────────────────────────────
QColor DragCanvas::colorForType(const QString &type) const
{
    if (type == "Button")      return QColor(100, 149, 237, 200);  // cornflower blue
    if (type == "Label")       return QColor(240, 128, 128, 200);  // light coral
    if (type == "TextField")   return QColor(144, 238, 144, 200);  // light green
    if (type == "Checkbox")    return QColor(255, 215,   0, 200);  // gold
    if (type == "RadioButton") return QColor(221, 160, 221, 200);  // plum
    return QColor(200, 200, 200, 200);                              // default gray
}