#include "mainwindow.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

// ─────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_canvas(nullptr)
    , m_panel(nullptr)
    , m_statusLabel(nullptr)
    , m_zoomLabel(nullptr)
    , m_posLabel(nullptr)
{
    setWindowTitle("Canvas Widget Designer - Qt Interview Assignment");
    setMinimumSize(900, 620);

    setupLayout();       // 1. create and arrange child widgets
    setupToolbar();      // 2. create toolbar with action buttons
    setupStatusBar();    // 3. create status bar labels
    setupConnections();  // 4. wire signals to slots
}

MainWindow::~MainWindow()
{
    // Qt parent-child system handles all deletions.
    // Nothing to manually delete here.
}

// ─────────────────────────────────────────────────────────────────────
// setupLayout
// Creates the two-panel layout: canvas (left) + side panel (right)
// ─────────────────────────────────────────────────────────────────────
void MainWindow::setupLayout()
{
    // Central container widget — QMainWindow requires a single central widget
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Horizontal split: canvas takes 4/5, panel takes 1/5
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_canvas = new CanvasWidget(this);
    m_panel  = new DraggablePanel(this);

    mainLayout->addWidget(m_canvas, 4);   // stretch factor 4
    mainLayout->addWidget(m_panel,  1);   // stretch factor 1
}

// ─────────────────────────────────────────────────────────────────────
// setupToolbar
// ─────────────────────────────────────────────────────────────────────
void MainWindow::setupToolbar()
{
    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(false);   // prevent user from dragging toolbar away

    // "Add Button" — creates a Button widget at canvas center
    QPushButton *addBtn = new QPushButton("Add Button", this);
    addBtn->setFixedHeight(28);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddButton);
    toolbar->addWidget(addBtn);

    toolbar->addSeparator();

    // "Clear All" — removes all canvas items
    QPushButton *clearBtn = new QPushButton("Clear All", this);
    clearBtn->setFixedHeight(28);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearAll);
    toolbar->addWidget(clearBtn);

    toolbar->addSeparator();

    // "Help" — shows usage instructions
    QPushButton *helpBtn = new QPushButton("Help", this);
    helpBtn->setFixedHeight(28);
    connect(helpBtn, &QPushButton::clicked, this, &MainWindow::onHelp);
    toolbar->addWidget(helpBtn);
}

// ─────────────────────────────────────────────────────────────────────
// setupStatusBar
// Three sections: left message | center zoom | right position
// ─────────────────────────────────────────────────────────────────────
void MainWindow::setupStatusBar()
{
    // Left — transient operation messages
    m_statusLabel = new QLabel("Ready - Click and drag to create widgets", this);
    m_statusLabel->setMinimumWidth(300);
    statusBar()->addWidget(m_statusLabel);    // addWidget = left side, can be overwritten by showMessage()

    // Center — zoom level (permanent = right side, never overwritten)
    m_zoomLabel = new QLabel("Zoom: 100%", this);
    m_zoomLabel->setMinimumWidth(90);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_zoomLabel);

    // Right — canvas mouse position
    m_posLabel = new QLabel("Position: (0, 0)", this);
    m_posLabel->setMinimumWidth(130);
    m_posLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_posLabel);
}

// ─────────────────────────────────────────────────────────────────────
// setupConnections
// All signal→slot wiring in one place — easy to audit
// ─────────────────────────────────────────────────────────────────────
void MainWindow::setupConnections()
{
    // Canvas notifies us when zoom changes
    connect(m_canvas, &CanvasWidget::zoomChanged,
            this,     &MainWindow::onZoomChanged);

    // Canvas notifies us when an operation completes
    connect(m_canvas, &CanvasWidget::statusChanged,
            this,     &MainWindow::onStatusChanged);

    // Canvas notifies us when selection state changes
    // (could be used to enable/disable a Delete toolbar button)
    connect(m_canvas, &CanvasWidget::selectionChanged,
            this,     &MainWindow::onSelectionChanged);

    // Canvas reports mouse position for status bar
    connect(m_canvas, &CanvasWidget::mousePosChanged,
            this,     [this](QPoint pos) {
                m_posLabel->setText(
                    QString("Position: (%1, %2)").arg(pos.x()).arg(pos.y())
                    );
            });
}

// ─────────────────────────────────────────────────────────────────────
// Slots
// ─────────────────────────────────────────────────────────────────────
void MainWindow::onAddButton()
{
    // Add a Button widget at a sensible default position on the canvas
    m_canvas->addWidget(WidgetType::Button);
}

void MainWindow::onClearAll()
{
    // Confirm before destructive action
    auto reply = QMessageBox::question(
        this,
        "Clear All",
        "Remove all widgets from the canvas?",
        QMessageBox::Yes | QMessageBox::No
        );
    if (reply == QMessageBox::Yes)
        m_canvas->clearAll();
}

void MainWindow::onHelp()
{
    QMessageBox::information(this, "Help",
                             "Canvas Widget Designer\n\n"
                             "MOUSE:\n"
                             "  Left click          → Select widget\n"
                             "  Drag empty area     → Draw new widget\n"
                             "  Drag selected       → Move widget\n"
                             "  Double-click        → Change color\n"
                             "  Right-click         → Context menu\n"
                             "  Ctrl + Wheel        → Zoom\n\n"
                             "KEYBOARD:\n"
                             "  Delete              → Remove selected\n"
                             "  Arrow keys          → Move (1px)\n"
                             "  Shift + Arrow       → Move (10px)\n"
                             "  Ctrl + A            → Select all\n"
                             "  Escape              → Deselect\n"
                             "  Ctrl + Plus/Minus   → Zoom\n"
                             "  Ctrl + 0            → Reset zoom\n\n"
                             "DRAG & DROP:\n"
                             "  Drag items from the right panel onto the canvas"
                             );
}

void MainWindow::onZoomChanged(int zoomPercent)
{
    m_zoomLabel->setText(QString("Zoom: %1%").arg(zoomPercent));
}

void MainWindow::onStatusChanged(const QString &message)
{
    m_statusLabel->setText(message);
}

void MainWindow::onSelectionChanged(bool hasSelection)
{
    // Could enable/disable a Delete button here if one existed in toolbar
    Q_UNUSED(hasSelection);
}