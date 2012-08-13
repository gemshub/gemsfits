#include <QtGui/QApplication>
#include "mainwindow.h"
#include "sqlscript.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile File("stylesheet.qss");
//    QFile File("stylesheet_empty.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());
    w.setStyleSheet(StyleSheet);
    w.show();

    return a.exec();
}
