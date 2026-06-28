#include "canvaswidget.h"
#include <QApplication>
#include <QRandomGenerator>
#include <QMimeData>
#include <QDrag>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QtMath>

// ─────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────
CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
    , m_selectedIndex(-1)
    , m_mode(CanvasMode::Idle)
    , m_zoom(100)
    , m_dragOver(false)
{
    setFocusPolicy(Qt::StrongFocus);   // receive keyboard events on click
    setAcceptDrops(true);              // receive drag-and-drop events
    setMouseTracking(true);            // mouseMoveEvent fires without button held

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(245, 245, 248));
    setPalette(pal);

    setMinimumSize(500, 400);
}

CanvasWidget::~CanvasWidget() { }

// ─────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::clearAll()
{
    m_items.clear();
    m_selectedIndex = -1;
    m_mode = CanvasMode::Idle;
    emit selectionChanged(false);
    emit statusChanged("Canvas cleared");
    update();
}

void CanvasWidget::addWidget(WidgetType type)
{
    // Place new widget at a readable default position
    CanvasItem item;
    item.type     = type;
    item.label    = labelForType(type);
    item.color    = defaultColorForType(type);
    item.selected = true;

    // Stagger position so multiple "Add Button" clicks don't pile up
    int offset = m_items.size() * 20;
    item.rect = QRect(60 + offset, 60 + offset, 120, 48);

    deselectAll();
    m_items.append(item);
    m_selectedIndex = m_items.size() - 1;

    emit selectionChanged(true);
    emit statusChanged(QString("Added: %1").arg(item.label));
    update();
}

void CanvasWidget::setZoom(int percent)
{
    int clamped = qBound(20, percent, 500);
    if (clamped == m_zoom) return;
    m_zoom = clamped;
    update();
    emit zoomChanged(m_zoom);
    emit statusChanged(QString("Zoom: %1%").arg(m_zoom));
}

// ─────────────────────────────────────────────────────────────────────
// paintEvent
// Redraws the ENTIRE canvas on every call.
// Order: grid → items → rubber-band → drop preview → hints
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // ── Apply zoom transform ──────────────────────────────────────────
    p.save();
    double scale = m_zoom / 100.0;
    p.scale(scale, scale);

    // All drawing below is in CANVAS coordinates
    drawGrid(p);

    for (const CanvasItem &item : m_items)
        drawItem(p, item);

    // Rubber-band while drawing a new widget
    if (m_mode == CanvasMode::DrawingNew && m_drawRect.isValid())
        drawRubberBand(p);

    // Drop preview while dragging from panel
    if (m_dragOver)
        drawDropPreview(p);

    p.restore();
    // ── Below this line: unscaled screen-space drawing ────────────────

    drawHints(p);   // instruction text overlay, always same screen size
}

// ─────────────────────────────────────────────────────────────────────
// drawGrid
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::drawGrid(QPainter &p) const
{
    const int gridSize = 20;
    double scale = m_zoom / 100.0;

    // How many canvas units are visible in the widget area
    int visW = static_cast<int>(width()  / scale) + gridSize;
    int visH = static_cast<int>(height() / scale) + gridSize;

    QPen pen(QColor(225, 225, 230));
    pen.setWidth(0);   // cosmetic pen — always 1px on screen
    p.setPen(pen);

    for (int x = 0; x <= visW; x += gridSize)
        p.drawLine(x, 0, x, visH);
    for (int y = 0; y <= visH; y += gridSize)
        p.drawLine(0, y, visW, y);
}

// ─────────────────────────────────────────────────────────────────────
// drawItem
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::drawItem(QPainter &p, const CanvasItem &item) const
{
    // ── Fill ──────────────────────────────────────────────────────────
    p.setBrush(QBrush(item.color));

    // ── Border — thicker blue dashes when selected ────────────────────
    if (item.selected) {
        QPen selPen(QColor(30, 100, 255));
        selPen.setWidth(2);
        selPen.setStyle(Qt::DashLine);
        p.setPen(selPen);
    } else {
        p.setPen(QPen(QColor(80, 80, 80), 1));
    }

    p.drawRect(item.rect);

    // ── Label ─────────────────────────────────────────────────────────
    QFont f;
    f.setPointSize(9);
    f.setBold(item.selected);
    p.setFont(f);
    p.setPen(Qt::black);
    p.drawText(item.rect, Qt::AlignCenter, item.label);

    // ── Selection handles ─────────────────────────────────────────────
    if (item.selected)
        drawSelectionHandles(p, item.rect);
}

// ─────────────────────────────────────────────────────────────────────
// drawSelectionHandles
// 4 small white squares at the corners of the selection
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::drawSelectionHandles(QPainter &p, const QRect &rect) const
{
    const int H = 7;
    p.setBrush(Qt::white);
    p.setPen(QPen(QColor(30, 100, 255), 1));

    const QList<QPoint> corners = {
        rect.topLeft(),    rect.topRight(),
        rect.bottomLeft(), rect.bottomRight()
    };

    for (const QPoint &c : corners)
        p.drawRect(QRect(c.x() - H/2, c.y() - H/2, H, H));
}

// ─────────────────────────────────────────────────────────────────────
// drawRubberBand
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::drawRubberBand(QPainter &p) const
{
    QPen pen(QColor(60, 60, 220));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    p.setPen(pen);
    p.setBrush(QBrush(QColor(80, 80, 255, 30)));
    p.drawRect(m_drawRect);

    // Show dimensions while drawing
    QString sizeStr = QString("%1 × %2")
                          .arg(m_drawRect.width())
                          .arg(m_drawRect.height());
    p.setPen(QColor(40, 40, 180));
    p.drawText(m_drawRect.bottomRight() + QPoint(4, 0), sizeStr);
}

// ─────────────────────────────────────────────────────────────────────
// drawDropPreview
// Ghost rectangle shown while dragging from the panel
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::drawDropPreview(QPainter &p) const
{
    QRect preview(m_dragPos.x() - 60,
                  m_dragPos.y() - 24,
                  120, 48);

    QPen pen(QColor(30, 180, 30));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    p.setPen(pen);
    p.setBrush(QBrush(QColor(30, 200, 30, 40)));
    p.drawRect(preview);

    p.setPen(QColor(20, 140, 20));
    QFont f;
    f.setPointSize(9);
    p.setFont(f);
    p.drawText(preview, Qt::AlignCenter, m_dragType);
}

// ─────────────────────────────────────────────────────────────────────
// drawHints
// Drawn AFTER p.restore() — screen-space, always same size
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::drawHints(QPainter &p) const
{
    if (!m_items.isEmpty()) return;   // hide hints once user has added items

    p.setPen(QColor(180, 180, 200));
    QFont f;
    f.setPointSize(10);
    p.setFont(f);

    QRect hintRect = rect().adjusted(20, 20, -20, -20);
    p.drawText(hintRect,
               Qt::AlignTop | Qt::AlignHCenter,
               "← Mouse Events: Click, Drag, Double-click");

    p.drawText(hintRect,
               Qt::AlignBottom | Qt::AlignHCenter,
               "Keyboard: Delete, Arrows, Ctrl+/-");

    p.drawText(hintRect,
               Qt::AlignVCenter | Qt::AlignLeft,
               "Wheel: Ctrl+Scroll to Zoom →");
}

// ─────────────────────────────────────────────────────────────────────
// mousePressEvent
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;

    QPoint canvasPos = toCanvasCoords(event->pos());
    int hitIdx = hitTest(canvasPos);

    if (hitIdx >= 0) {
        // ── Hit an item ───────────────────────────────────────────────
        deselectAll();
        m_selectedIndex = hitIdx;
        m_items[m_selectedIndex].selected = true;
        m_dragOffset = canvasPos - m_items[m_selectedIndex].rect.topLeft();
        m_mode = CanvasMode::DraggingItem;

        emit selectionChanged(true);
        emit statusChanged(QString("Selected: %1")
                               .arg(m_items[m_selectedIndex].label));
    } else {
        // ── Hit empty space ───────────────────────────────────────────
        deselectAll();
        m_selectedIndex = -1;
        m_drawRect = QRect(canvasPos, canvasPos);
        m_mode = CanvasMode::DrawingNew;

        emit selectionChanged(false);
        emit statusChanged("Drawing...");
    }

    update();
}

// ─────────────────────────────────────────────────────────────────────
// mouseMoveEvent
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint canvasPos = toCanvasCoords(event->pos());

    switch (m_mode) {
    case CanvasMode::DraggingItem:
        if (m_selectedIndex >= 0 && m_selectedIndex < m_items.size()) {
            m_items[m_selectedIndex].rect.moveTopLeft(canvasPos - m_dragOffset);
            update();
        }
        break;

    case CanvasMode::DrawingNew:
        m_drawRect = QRect(m_drawRect.topLeft(), canvasPos).normalized();
        update();
        break;

    case CanvasMode::Idle:
        break;
    }

    // Always report canvas position to status bar
    emit mousePosChanged(canvasPos);
}

// ─────────────────────────────────────────────────────────────────────
// mouseReleaseEvent
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;

    if (m_mode == CanvasMode::DrawingNew) {
        QRect finalRect = m_drawRect.normalized();

        if (finalRect.width() >= 10 && finalRect.height() >= 10) {
            CanvasItem newItem;
            newItem.type     = WidgetType::Button;   // default type for drawn rects
            newItem.label    = "Button";
            newItem.color    = defaultColorForType(WidgetType::Button);
            newItem.rect     = finalRect;
            newItem.selected = true;

            deselectAll();
            m_items.append(newItem);
            m_selectedIndex = m_items.size() - 1;

            emit selectionChanged(true);
            emit statusChanged("Widget created — double-click to change color");
        } else {
            emit statusChanged("Ready - Click and drag to create widgets");
        }

        m_drawRect = QRect();
    }

    m_mode = CanvasMode::Idle;
    update();
}

// ─────────────────────────────────────────────────────────────────────
// mouseDoubleClickEvent
// Cycles the color of the clicked item through a palette
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint canvasPos = toCanvasCoords(event->pos());
    int idx = hitTest(canvasPos);
    if (idx >= 0)
        cycleColor(idx);
}

// ─────────────────────────────────────────────────────────────────────
// contextMenuEvent
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *clearAction     = menu.addAction("Clear All");
    QAction *resetZoomAction = menu.addAction("Reset Zoom (Ctrl+0)");
    menu.addSeparator();
    QAction *deleteAction    = menu.addAction("Delete Selected");
    QAction *selectAllAction = menu.addAction("Select All (Ctrl+A)");

    deleteAction->setEnabled(m_selectedIndex >= 0);

    connect(clearAction,     &QAction::triggered, this, &CanvasWidget::clearAll);
    connect(resetZoomAction, &QAction::triggered, this, [this]{ setZoom(100); });
    connect(deleteAction,    &QAction::triggered, this, &CanvasWidget::deleteSelected);
    connect(selectAllAction, &QAction::triggered, this, &CanvasWidget::selectAll);

    menu.exec(event->globalPos());
}

// ─────────────────────────────────────────────────────────────────────
// keyPressEvent
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::keyPressEvent(QKeyEvent *event)
{
    bool ctrl  = event->modifiers() & Qt::ControlModifier;
    bool shift = event->modifiers() & Qt::ShiftModifier;
    int  step  = shift ? 10 : 1;

    switch (event->key()) {
    case Qt::Key_Delete:
        deleteSelected();
        break;

    case Qt::Key_Left:   moveSelected(-step,     0); break;
    case Qt::Key_Right:  moveSelected( step,     0); break;
    case Qt::Key_Up:     moveSelected(    0, -step); break;
    case Qt::Key_Down:   moveSelected(    0,  step); break;

    case Qt::Key_A:
        if (ctrl) selectAll();
        break;

    case Qt::Key_Escape:
        deselectAll();
        m_selectedIndex = -1;
        emit selectionChanged(false);
        emit statusChanged("Deselected");
        update();
        break;

    case Qt::Key_Plus:
    case Qt::Key_Equal:
        if (ctrl) setZoom(m_zoom + 10);
        break;

    case Qt::Key_Minus:
        if (ctrl) setZoom(m_zoom - 10);
        break;

    case Qt::Key_0:
        if (ctrl) setZoom(100);
        break;

    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

// ─────────────────────────────────────────────────────────────────────
// wheelEvent
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();
        setZoom(m_zoom + (delta > 0 ? 10 : -10));
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

// ─────────────────────────────────────────────────────────────────────
// Drag & Drop
// ─────────────────────────────────────────────────────────────────────
void CanvasWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        m_dragOver = true;
        m_dragType = event->mimeData()->text();
        m_dragPos  = toCanvasCoords(event->position().toPoint());
        event->acceptProposedAction();
        update();
    } else {
        event->ignore();
    }
}

void CanvasWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText()) {
        m_dragPos = toCanvasCoords(event->position().toPoint());
        event->acceptProposedAction();
        update();
    } else {
        event->ignore();
    }
}

void CanvasWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    m_dragOver = false;
    m_dragType.clear();
    update();
}

void CanvasWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasText()) { event->ignore(); return; }

    QString    typeStr = event->mimeData()->text();
    WidgetType type    = widgetTypeFromString(typeStr);
    QPoint     pos     = toCanvasCoords(event->position().toPoint());

    CanvasItem newItem;
    newItem.type     = type;
    newItem.label    = labelForType(type);
    newItem.color    = defaultColorForType(type);
    newItem.rect     = QRect(pos.x() - 60, pos.y() - 24, 120, 48);
    newItem.selected = true;

    deselectAll();
    m_items.append(newItem);
    m_selectedIndex = m_items.size() - 1;

    m_dragOver = false;
    m_dragType.clear();

    event->acceptProposedAction();
    emit selectionChanged(true);
    emit statusChanged(QString("Dropped: %1").arg(newItem.label));
    update();
}

// ─────────────────────────────────────────────────────────────────────
// Private helpers
// ─────────────────────────────────────────────────────────────────────
int CanvasWidget::hitTest(const QPoint &canvasPos) const
{
    for (int i = m_items.size() - 1; i >= 0; --i)
        if (m_items[i].rect.contains(canvasPos))
            return i;
    return -1;
}

QPoint CanvasWidget::toCanvasCoords(const QPoint &screenPos) const
{
    double scale = m_zoom / 100.0;
    return QPoint(static_cast<int>(screenPos.x() / scale),
                  static_cast<int>(screenPos.y() / scale));
}

void CanvasWidget::deselectAll()
{
    for (CanvasItem &item : m_items)
        item.selected = false;
}

void CanvasWidget::selectAll()
{
    for (CanvasItem &item : m_items)
        item.selected = true;
    if (!m_items.isEmpty())
        m_selectedIndex = 0;
    emit selectionChanged(!m_items.isEmpty());
    emit statusChanged(QString("Selected all (%1 items)").arg(m_items.size()));
    update();
}

void CanvasWidget::deleteSelected()
{
    if (m_selectedIndex < 0 || m_selectedIndex >= m_items.size()) return;

    QString label = m_items[m_selectedIndex].label;
    m_items.removeAt(m_selectedIndex);
    m_selectedIndex = -1;
    m_mode = CanvasMode::Idle;

    emit selectionChanged(false);
    emit statusChanged(QString("Deleted: %1").arg(label));
    update();
}

void CanvasWidget::cycleColor(int index)
{
    static const QList<QColor> palette = {
        QColor(100, 149, 237, 210),
        QColor(240, 128, 128, 210),
        QColor(144, 238, 144, 210),
        QColor(255, 215,   0, 210),
        QColor(221, 160, 221, 210),
        QColor(255, 160,  80, 210)
    };

    QColor current = m_items[index].color;
    int found = -1;
    for (int i = 0; i < palette.size(); ++i) {
        if (palette[i].rgb() == current.rgb()) { found = i; break; }
    }
    int next = (found + 1) % palette.size();
    m_items[index].color = palette[next];

    emit statusChanged("Color changed — double-click again to cycle");
    update();
}

void CanvasWidget::moveSelected(int dx, int dy)
{
    if (m_selectedIndex < 0 || m_selectedIndex >= m_items.size()) return;
    m_items[m_selectedIndex].rect.translate(dx, dy);
    update();
}