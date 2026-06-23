#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Mini Project 3: Drag and Drop Widgets");
    setMinimumSize(700, 500);

    // ── Central container ─────────────────────────────────────────────
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ── Canvas (left, takes most space) ───────────────────────────────
    m_canvas = new DragCanvas(this);
    mainLayout->addWidget(m_canvas, 4);    // stretch factor 4

    // ── Side panel (right, fixed width) ───────────────────────────────
    m_panel = new QWidget(this);
    m_panel->setFixedWidth(150);
    m_panel->setStyleSheet("background-color: #2d2d2d;");

    QVBoxLayout *panelLayout = new QVBoxLayout(m_panel);
    panelLayout->setAlignment(Qt::AlignTop);
    panelLayout->setSpacing(8);
    panelLayout->setContentsMargins(10, 10, 10, 10);

    // Panel title
    QLabel *title = new QLabel("Drag & Drop", m_panel);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-weight: bold; font-size: 13px;");
    panelLayout->addWidget(title);

    QLabel *subtitle = new QLabel("Drag to canvas:", m_panel);
    subtitle->setStyleSheet("color: #aaaaaa; font-size: 11px;");
    panelLayout->addWidget(subtitle);

    // Draggable buttons
    const QStringList types = {
        "Button", "Label", "TextField", "Checkbox", "RadioButton"
    };

    for (const QString &type : types) {
        DraggableButton *btn = new DraggableButton(type, m_panel);
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: #444444;"
            "  color: white;"
            "  border: 1px solid #666666;"
            "  border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #555555;"
            "}"
            );
        panelLayout->addWidget(btn);
    }

    mainLayout->addWidget(m_panel, 1);    // stretch factor 1
}