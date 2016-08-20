#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QTextCodec>
#include <QSystemTrayIcon>
#include <QMenu>

#define APPLICATION_NAME "DefState"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QByteArray data_received;
    QByteArray test_received;
    QTextCodec *tc = QTextCodec::codecForName("UTF-8");
private slots:
    void saveAsCSV(QString filename);
    void on_pushButton_export_clicked();
    void on_pushButton_open_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_clean_clicked();
    void on_pushButton_test_clicked();
    void on_textBrowserRefesh();
    void ReadMyCom();
    void on_pushButton_reset_table_clicked();
    void on_pushButton_reset_row_clicked();
    void showHide(QSystemTrayIcon::ActivationReason);

    void on_autohide_clicked();

    void on_autorun_clicked();

    void on_setport_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *mSerialPort;
    QSystemTrayIcon *trIcon;
    QMenu *trIconMenu;
    QAction *quitAction;
};

#endif // MAINWINDOW_H
