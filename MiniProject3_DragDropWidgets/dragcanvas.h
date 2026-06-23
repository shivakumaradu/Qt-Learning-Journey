#ifndef DRAGCANVAS_H
#define DRAGCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QList>
#include <QRect>
#include <QPoint>
#include <QString>

// Data model for one dropped widget on the canvas
struct DroppedWidget {
    QRect   rect;
    QString type;    // "Button", "Label", "TextField", etc.
    QColor  color;
};

class DragCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit DragCanvas(QWidget *parent = nullptr);

protected:
    // Paint
    void paintEvent(QPaintEvent *event) override;

    // Drag-and-drop receivers
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event)   override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event)           override;

private:
    QList<DroppedWidget> m_widgets;     // all dropped widgets
    bool    m_dragOver;                 // true while a drag hovers over us
    QPoint  m_dragPos;                  // current hover position (for preview)
    QString m_dragType;                 // type string being dragged

    QColor colorForType(const QString &type) const;
    void   drawWidget(QPainter &p, const DroppedWidget &w) const;
    void   drawDropPreview(QPainter &p) const;
    void   drawGrid(QPainter &p) const;
};

#endif // DRAGCANVAS_H