#ifndef DRAGGABLEBUTTON_H
#define DRAGGABLEBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>

class DraggableButton : public QPushButton
{
    Q_OBJECT

public:
    explicit DraggableButton(const QString &widgetType,
                             QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_widgetType;   // "Button", "Label", "TextField", etc.
};

#endif // DRAGGABLEBUTTON_H