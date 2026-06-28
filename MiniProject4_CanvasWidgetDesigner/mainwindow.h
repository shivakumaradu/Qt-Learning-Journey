#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QToolBar>
#include <QPushButton>
#include <QHBoxLayout>

#include "canvaswidget.h"
#include "draggablepanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // ── Toolbar button handlers ───────────────────────────────────────
    void onAddButton();
    void onClearAll();
    void onHelp();

    // ── Canvas signal receivers ───────────────────────────────────────
    void onZoomChanged(int zoomPercent);
    void onStatusChanged(const QString &message);
    void onSelectionChanged(bool hasSelection);

private:
    // ── Child widgets ─────────────────────────────────────────────────
    CanvasWidget    *m_canvas;
    DraggablePanel  *m_panel;

    // ── Status bar labels ─────────────────────────────────────────────
    QLabel *m_statusLabel;
    QLabel *m_zoomLabel;
    QLabel *m_posLabel;

    // ── Setup helpers — called once from constructor ───────────────────
    void setupToolbar();
    void setupStatusBar();
    void setupConnections();
    void setupLayout();
};

#endif // MAINWINDOW_H