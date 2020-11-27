#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QString qss;

    QFile file(":/qss/default.css");
    if (file.open(QIODevice::ReadOnly))
    {
        qss.append(file.readAll());
    }

   app.setStyleSheet(qss);

    MainWindow mw;
    mw.show();

    return app.exec();
}