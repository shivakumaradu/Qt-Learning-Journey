#ifndef DRAGGABLEPANEL_H
#define DRAGGABLEPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "draggablebutton.h"

class DraggablePanel : public QWidget
{
    Q_OBJECT

public:
    explicit DraggablePanel(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // DRAGGABLEPANEL_H
