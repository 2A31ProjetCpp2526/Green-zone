#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include "chartdialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Connection *c = Connection::getInstance();
    if (!c->openConnection()) return -1;

    MainWindow w;
    w.show();
    ChartDialog dlg;   // ★ SHOW CHART
    dlg.exec();

    SmartMetalDialog Dlg;
    Dlg.exec();
    


    return a.exec();
}
