#include "draggablepanel.h"

DraggablePanel::DraggablePanel(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(160);
    setStyleSheet("background-color: #2b2b2b;");
    setupUI();
}

void DraggablePanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(6);
    layout->setContentsMargins(10, 14, 10, 14);

    // ── Panel title ───────────────────────────────────────────────────
    QLabel *title = new QLabel("Drag & Drop Panel", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: white;"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding: 4px;"
        "background-color: #1e1e1e;"
        "border-radius: 4px;"
        );
    layout->addWidget(title);

    QLabel *subtitle = new QLabel("Drag these to canvas:", this);
    subtitle->setStyleSheet("color: #aaaaaa; font-size: 10px; padding: 2px 0;");
    layout->addWidget(subtitle);

    layout->addSpacing(4);

    // ── One draggable button per widget type ──────────────────────────
    const QStringList types = {
        "Button", "Label", "TextField", "Checkbox", "RadioButton"
    };

    const QString btnStyle =
        "QPushButton {"
        "  background-color: #3c3f41;"
        "  color: #e0e0e0;"
        "  border: 1px solid #555555;"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "  font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #4e5254;"
        "  border: 1px solid #888888;"
        "}";

    for (const QString &type : types) {
        DraggableButton *btn = new DraggableButton(type, this);
        btn->setStyleSheet(btnStyle);
        layout->addWidget(btn);
    }

    // ── "Drag & Drop" label at bottom ─────────────────────────────────
    layout->addStretch();
    QLabel *footer = new QLabel("Drag && Drop", this);
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("color: #5588cc; font-size: 10px;");
    layout->addWidget(footer);
}