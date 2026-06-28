#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Application-wide metadata (used by QSettings, about dialogs, etc.)
    QApplication::setApplicationName("Canvas Widget Designer");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("ZopplerSystems");

    MainWindow window;
    window.show();

    return app.exec();
}