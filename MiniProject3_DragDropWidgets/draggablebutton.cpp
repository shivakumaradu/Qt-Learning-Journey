#include "draggablebutton.h"

DraggableButton::DraggableButton(const QString &widgetType,
                                 QWidget *parent)
    : QPushButton(widgetType, parent)  // button label = widget type string
    , m_widgetType(widgetType)
{
    setFixedSize(120, 36);             // consistent size for all panel buttons
    setCursor(Qt::OpenHandCursor);     // hint to user: this is draggable
}

void DraggableButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QPushButton::mousePressEvent(event);   // pass non-left clicks to base
        return;
    }

    // ── Step 1: Build the data envelope ──────────────────────────────
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(m_widgetType);           // e.g. "Button", "Label"

    // ── Step 2: Create a visual ghost image ───────────────────────────
    // Render THIS button's appearance into a pixmap so the user sees
    // a "copy" of the button following their cursor during the drag
    QPixmap ghost(this->size());
    ghost.fill(Qt::transparent);               // start fully transparent
    this->render(&ghost);                      // paint button onto pixmap

    // ── Step 3: Create and configure the drag object ──────────────────
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);               // attach data (QDrag takes ownership)
    drag->setPixmap(ghost);                    // attach ghost image
    drag->setHotSpot(event->pos());            // cursor "holds" the ghost at click point

    // ── Step 4: Execute — blocks until drop or cancel ─────────────────
    drag->exec(Qt::CopyAction);

    // NOTE: we do NOT call QPushButton::mousePressEvent(event) here
    // because that would trigger the button's normal "click" animation
    // while dragging — undesirable visual behavior
}