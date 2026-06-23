#include <QApplication>
#include "rectcanvas.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RectCanvas window;
    window.show();

    return app.exec();
}