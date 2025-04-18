#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    w.raise();   // bring to front
    w.activateWindow(); // focus
    w.resize(800, 600); // enforce size again

    return app.exec();
}
