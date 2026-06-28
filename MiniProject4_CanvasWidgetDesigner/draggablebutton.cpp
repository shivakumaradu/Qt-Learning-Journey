#include "draggablebutton.h"

DraggableButton::DraggableButton(const QString &widgetType,
                                 QWidget *parent)
    : QPushButton(widgetType, parent)
    , m_widgetType(widgetType)
{
    setFixedSize(136, 34);
    setCursor(Qt::OpenHandCursor);
}

void DraggableButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QPushButton::mousePressEvent(event);
        return;
    }

    // ── Package the widget type into QMimeData ────────────────────────
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(m_widgetType);

    // ── Ghost image: render this button into a pixmap ─────────────────
    QPixmap ghost(size());
    ghost.fill(Qt::transparent);
    render(&ghost);

    // ── Execute the drag ──────────────────────────────────────────────
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(ghost);
    drag->setHotSpot(event->pos());

    // Cursor changes to ClosedHand while dragging
    setCursor(Qt::ClosedHandCursor);
    drag->exec(Qt::CopyAction);
    setCursor(Qt::OpenHandCursor);   // restore after drag completes
}