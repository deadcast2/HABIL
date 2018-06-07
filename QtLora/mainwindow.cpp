#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new QSerialPort(this))
{
    ui->setupUi(this);

    serialPort->setPortName("COM3");
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(!serialPort->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not open COM3 port"));
    }

    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readData()
{
    const QByteArray data = serialPort->readAll();

    QByteArray jp2Header = QByteArray("\x6a\x50", 2);
    if(data.indexOf(jp2Header) > -1)
    {
        transmissionState = TransmissionState::Receiving;
    }

    QByteArray jp2Footer = QByteArray("\xff\xd9", 2);
    if(data.indexOf(jp2Footer) > -1)
    {
        UpdateProgress(tr("Finished!"), data.size());
        transmissionState = TransmissionState::Ready;
        totalBytesReceived = 0;
    }

    if(transmissionState == TransmissionState::Receiving)
    {
        UpdateProgress(tr("Downloading new photo..."), data.size());
    }
}

void MainWindow::UpdateProgress(QString message, quint16 bytesReceived)
{
    QString formattedSize = locale().formattedDataSize(totalBytesReceived += bytesReceived);
    statusBar()->showMessage(QString("%1 %2").arg(message, formattedSize));
}
