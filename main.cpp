#include "mainwindow.h"
#include <QApplication>
#include <QSettings>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("BERLOGA");
    QCoreApplication::setApplicationName("DefState");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QApplication::setQuitOnLastWindowClosed(false);
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    MainWindow w;
    if((settings->value("settings/autohide")) == 0)
    {
      w.show();
    }
    return a.exec();
}
