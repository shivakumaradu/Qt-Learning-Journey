#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "dragcanvas.h"
#include "draggablebutton.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    DragCanvas      *m_canvas;
    QWidget         *m_panel;
};

#endif // MAINWINDOW_H