#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QTableWidget>
#include <QMessageBox>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QThread>
#include <QAction>
#include <QSettings>
#include <QDir>
#include <QAction>
#include "sleeper.h"

int dat=0,vin=1,defect_code=2,work_time=3,str; //номера столбцов

QString set_vin(""), set_defect_code(""), set_work_time("");

QDate date = QDate::currentDate();

QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
QSettings *settings1 = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    quitAction = new QAction("&Выход", this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trIcon = new QSystemTrayIcon();
    trIcon->setIcon(QIcon(":/img/icon.png"));
    trIcon->show();
    trIconMenu = new QMenu(this);
    trIconMenu->addAction(quitAction);
    trIcon->setContextMenu(trIconMenu);
    connect(trIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(showHide(QSystemTrayIcon::ActivationReason)));

    aboutMenu = new QMenu(this);
    aboutMenu->addAction("&О программе", this, &MainWindow::about);
    aboutMenu->addAction("О &Qt", qApp, &QApplication::aboutQt);
    ui->pushButton_about->setMenu(aboutMenu);

    mSerialPort = new QSerialPort;

    connect(mSerialPort,SIGNAL(readyRead()),this,SLOT(ReadMyCom()));
	connect(ui->textBrowser,SIGNAL(textChanged()),this,SLOT(on_textBrowserRefesh()));

    ui->pushButton_close->setDisabled(true);

    ui->autohide->setChecked(settings->value("settings/autohide", 0).toBool());
    ui->autorun->setChecked(settings->value("settings/autorun", 0).toBool());
    ui->setport->setChecked(settings->value("settings/setport", 0).toBool());
    ui->autoconn->setChecked(settings->value("settings/autoconnect", 0).toBool());

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        if(ui->setport->isChecked())
        {
            ui->comboBox_portName->addItem(settings->value("settings/port").toString());
            ui->comboBox_portName->setDisabled(true);
        }
        else
        {
            ui->comboBox_portName->addItem(info.portName());
        }


    QStringList baudRateList = QStringList() << "1200" << "2400" << "4800"
                                             << "9600" << "14400" << "19200" << "38400" << "43000" << "57600"
                                             << "76800" << "115200" << "128000" << "230400" << "256000" <<"460800"
                                             << "921600" << "1382400";
    QStringList parityList = QStringList() << "Нет" << "Чет" << "Нечет" << "Точка" << "Пробел";
    QStringList stopBitsList = QStringList() << "1" << "1.5" << "2";
    QStringList dataBitsList = QStringList() << "8" << "7" << "6" << "5" << "4";

    ui->comboBox_baudRate->addItems(baudRateList);
    ui->comboBox_baudRate->setCurrentIndex(10);
    ui->comboBox_parity->addItems(parityList);
    ui->comboBox_stopBit->addItems(stopBitsList);
    ui->comboBox_dataBit->addItems(dataBitsList);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    on_pushButton_extset_clicked();

    if((settings->value("settings/autoconnect")) == 1)
    {
      on_pushButton_open_clicked();
    }
}

MainWindow::~MainWindow()
{
	mSerialPort->close();
    trIcon->hide();
    delete ui;
}


void MainWindow::ReadMyCom()
{
   // mSerialPort->clear();
    data_received = mSerialPort->readAll();
    Sleeper::msleep(100);
    QString strk = tc->toUnicode(data_received);
    ui->textBrowser->insertPlainText(strk+"\n");
    set_vin=strk.left(4);
    set_defect_code=strk.mid(4,2 );
    set_work_time=strk.right(2);
    qDebug()<< set_vin;
    qDebug()<< set_defect_code;
    qDebug()<< set_work_time;
    on_pushButton_test_clicked();
}

void MainWindow::on_textBrowserRefesh()
{
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::saveAsCSV(QString filename)
{
   QFile f(filename);

    if( f.open( QIODevice::Append ) )
    {
        QTextStream ts( &f );
        QStringList strList;

        strList << "\" \"";
        for( int c = 0; c < ui->tableWidget->horizontalHeader()->count(); ++c )
            strList << "\""+ui->tableWidget->model()->headerData(c, Qt::Horizontal).toString()+"\"";
        ts << strList.join( ";" )+"\n";

        for( int r = 0; r < ui->tableWidget->verticalHeader()->count(); ++r )
        {
            strList.clear();
            strList << "\""+ui->tableWidget->model()->headerData(r, Qt::Vertical).toString()+"\"";
            for( int c = 0; c < ui->tableWidget->horizontalHeader()->count(); ++c )
            {
                strList << "\""+ui->tableWidget->model()->data(ui->tableWidget->model()->index(r, c), Qt::DisplayRole).toString()+"\"";
            }
            ts << strList.join( ";" )+"\n";
        }
        f.close();
        QMessageBox::information(this,"Сообщение","Экспорт успешно завершён");
    }

 else
   {

        QMessageBox::warning(this,"Сообщение","Экспорт не удался! Файл открыт в другой программе.\nЗакройте приложение использующее в настоящее время файл экспорта.");
   }

}

void MainWindow::on_pushButton_test_clicked()
{

    QTableWidgetItem *item_dat=new QTableWidgetItem(date.toString("dd.MM.yyyy"));
    QTableWidgetItem *item_vin=new QTableWidgetItem(set_vin);
    QTableWidgetItem *item_defect_code=new QTableWidgetItem(set_defect_code);
    QTableWidgetItem *item_work_time=new QTableWidgetItem(set_work_time);

    item_dat->setTextAlignment(Qt::AlignCenter);
    item_vin->setTextAlignment(Qt::AlignCenter);
    item_defect_code->setTextAlignment(Qt::AlignCenter);
    item_work_time->setTextAlignment(Qt::AlignCenter);

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(str, dat, item_dat);
    ui->tableWidget->setItem(str, vin, item_vin);
    ui->tableWidget->setItem(str, defect_code, item_defect_code);
    ui->tableWidget->setItem(str, work_time, item_work_time);
    str++;
    return;
}

void MainWindow::on_pushButton_export_clicked()
{
    saveAsCSV("test.csv");
}

void MainWindow::on_pushButton_open_clicked()
{
    try
    {
        mSerialPort->setPortName(ui->comboBox_portName->currentText());

        if (!mSerialPort->open(QIODevice::ReadWrite)) {
            QMessageBox::warning(this,"Внимание","Порт: " +(ui->comboBox_portName->currentText())+ " недоступен или используется другим приложением."
                                       "\nЗакройте все программы использующие этот порт и повторите попытку.");
            return;
        }
        mSerialPort->open(QIODevice::ReadWrite);
        mSerialPort->setBaudRate(ui->comboBox_baudRate->currentText().toInt());

        QString currentData = " ";
        currentData = ui->comboBox_dataBit->currentText();
        switch(currentData.toInt())
        {
            case 8:mSerialPort->setDataBits(QSerialPort::Data8);break;
            case 7:mSerialPort->setDataBits(QSerialPort::Data7);break;
            case 6:mSerialPort->setDataBits(QSerialPort::Data6);break;
            case 5:mSerialPort->setDataBits(QSerialPort::Data5);break;
        }

        mSerialPort->setFlowControl(QSerialPort::NoFlowControl);
        currentData = ui->comboBox_stopBit->currentText();
        int n = ui->comboBox_stopBit->currentIndex();
        switch(n)
        {
            case 0:mSerialPort->setStopBits(QSerialPort::OneStop);break;
            case 1:mSerialPort->setStopBits(QSerialPort::OneAndHalfStop);break;
            case 2:mSerialPort->setStopBits(QSerialPort::TwoStop);break;

        }
        int m = ui->comboBox_parity->currentIndex();
        switch(m)
        {
            case 0:mSerialPort->setParity(QSerialPort::NoParity);break;
            case 1:mSerialPort->setParity(QSerialPort::EvenParity);break;
            case 2:mSerialPort->setParity(QSerialPort::OddParity);break;

        }
        ui->pushButton_close->setDisabled(false);
        ui->pushButton_open->setDisabled(true);
    } 
	catch(...)
    {
        QMessageBox::warning(this,"ERROR!","Cannot open the serialport!");
    }
}

void MainWindow::on_pushButton_close_clicked()
{
    mSerialPort->close();
    ui->pushButton_close->setDisabled(true);
    ui->pushButton_open->setDisabled(false);

}

void MainWindow::on_pushButton_clean_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_pushButton_reset_table_clicked()
{
    for( str=ui->tableWidget->rowCount(); str > 0; str-- ) ui->tableWidget->removeRow(str -1); str--; str=0;
}

void MainWindow::on_pushButton_reset_row_clicked()
{
        if(str > 0){
          str=ui->tableWidget->rowCount();
          ui->tableWidget->removeRow(str-1);
          str--;
        }
}

void MainWindow::showHide(QSystemTrayIcon::ActivationReason r)
{
    if (r==QSystemTrayIcon::Trigger){
     if (!this->isVisible())
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}
}

void MainWindow::on_autohide_clicked()
{
    if(ui->autohide->isChecked())
    {
     settings->setValue("settings/autohide", 1);
    }
    else
    {
     settings->setValue("settings/autohide", 0);
    }
     settings->sync();
}

void MainWindow::on_autorun_clicked()
{
    if(ui->autorun->isChecked())
    {
     settings->setValue("settings/autorun", 1);
     settings1->setValue(APPLICATION_NAME, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    }
    else
    {
     settings->setValue("settings/autorun", 0);
     settings1->remove(APPLICATION_NAME);
    }
     settings->sync();
}

void MainWindow::on_setport_clicked()
{
    if(ui->setport->isChecked())
    {
     settings->setValue("settings/setport", 1);
     settings->setValue("settings/port", ui->comboBox_portName->currentText());
     ui->comboBox_portName->clear();
     ui->comboBox_portName->addItem(settings->value("settings/port").toString());
     ui->comboBox_portName->setDisabled(true);
    }
    else
    {
     settings->setValue("settings/setport", 0);
     settings->setValue("settings/port", "");
     ui->comboBox_portName->clear();
     ui->comboBox_portName->setDisabled(false);
     foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
     ui->comboBox_portName->addItem(info.portName());
    }
     settings->sync();
}

void MainWindow::on_autoconn_clicked()
{
    if(ui->autoconn->isChecked())
    {
     settings->setValue("settings/autoconnect", 1);
    }
    else
    {
     settings->setValue("settings/autoconnect", 0);
    }
     settings->sync();
}

void MainWindow::on_pushButton_extset_clicked()
{
      if(ui->setWidget->isHidden())
      {
       ui->setWidget->show();
       ui->pushButton_extset->setText("-");
       ui->widget->move(0,190);
       ui->textBrowser->move(10,330);
       ui->textBrowser->resize(220,300);
      }
      else
      {
      ui->setWidget->hide();
      ui->pushButton_extset->setText("+");
      ui->widget->move(0,90);
      ui->textBrowser->move(10,230);
      ui->textBrowser->resize(220,400);
      }
}

void MainWindow::about()
{
   QMessageBox::about(this, "О программе",
           "Техзадание: Стригин В.Ф. (VF.Strigin@vaz.ru) \n"
           "Разработка программы: Культяпов А.В. (camellan@yandex.ru) \n"
           "Исходный код программы: https://github.com/camellan/defstate");
}

void MainWindow::on_pushButton_about_clicked()
{
    aboutMenu->exec();
}
