#include "MainWindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>
#include "DarkStyle.h"
#include "framelesswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    a.setStyle(new DarkStyle);
    // create frameless window (and set windowState or title)
    FramelessWindow framelessWindow;
    framelessWindow.setWindowTitle(TITLE);
    framelessWindow.setFixedSize(WINDOW_DIMENSIONS);

    MainWindow *mainWindow = new MainWindow;
    // add the mainwindow to our custom frameless window
    framelessWindow.setContent(mainWindow);
    framelessWindow.show();

    return a.exec();
}
