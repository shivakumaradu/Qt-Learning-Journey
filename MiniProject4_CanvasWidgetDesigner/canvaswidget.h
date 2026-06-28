#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QMenu>

#include "canvasitem.h"

// ─────────────────────────────────────────────────────────────────────
// Canvas interaction modes — explicit state machine
// ─────────────────────────────────────────────────────────────────────
enum class CanvasMode {
    Idle,
    DraggingItem,
    DrawingNew
};

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    ~CanvasWidget() override;

    // ── Public API (called by MainWindow) ─────────────────────────────
    void clearAll();
    void addWidget(WidgetType type);
    void setZoom(int percent);
    int  currentZoom() const { return m_zoom; }

signals:
    void zoomChanged(int zoomPercent);
    void statusChanged(const QString &message);
    void selectionChanged(bool hasSelection);
    void mousePosChanged(QPoint canvasPos);

protected:
    // ── Paint ─────────────────────────────────────────────────────────
    void paintEvent(QPaintEvent *event) override;

    // ── Mouse ─────────────────────────────────────────────────────────
    void mousePressEvent(QMouseEvent *event)       override;
    void mouseMoveEvent(QMouseEvent *event)        override;
    void mouseReleaseEvent(QMouseEvent *event)     override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    // ── Keyboard ──────────────────────────────────────────────────────
    void keyPressEvent(QKeyEvent *event) override;

    // ── Wheel ─────────────────────────────────────────────────────────
    void wheelEvent(QWheelEvent *event) override;

    // ── Drag & Drop ───────────────────────────────────────────────────
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event)   override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event)           override;

private:
    // ── Model ─────────────────────────────────────────────────────────
    QList<CanvasItem> m_items;
    int               m_selectedIndex;

    // ── Interaction state ─────────────────────────────────────────────
    CanvasMode m_mode;
    QPoint     m_dragOffset;
    QRect      m_drawRect;

    // ── Zoom ──────────────────────────────────────────────────────────
    int m_zoom;           // 20 to 500

    // ── Drag & Drop hover state ───────────────────────────────────────
    bool    m_dragOver;
    QPoint  m_dragPos;
    QString m_dragType;

    // ── Paint helpers ─────────────────────────────────────────────────
    void drawGrid(QPainter &p) const;
    void drawItem(QPainter &p, const CanvasItem &item) const;
    void drawSelectionHandles(QPainter &p, const QRect &rect) const;
    void drawRubberBand(QPainter &p) const;
    void drawDropPreview(QPainter &p) const;
    void drawHints(QPainter &p) const;

    // ── Logic helpers ─────────────────────────────────────────────────
    int    hitTest(const QPoint &screenPos) const;
    QPoint toCanvasCoords(const QPoint &screenPos) const;
    void   deselectAll();
    void   selectAll();
    void   deleteSelected();
    void   cycleColor(int index);
    void   moveSelected(int dx, int dy);
};

#endif // CANVASWIDGET_H