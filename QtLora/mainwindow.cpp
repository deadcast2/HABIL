#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtJP2OpenJPEGImageHandler.h"

#include <QMessageBox>
#include <QFile>
#include <QBuffer>

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
        QMessageBox::critical(this, tr("Error"), tr("Could not open COM3 port. Try again."));
        exit(EXIT_FAILURE);
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
    totalReceivedData.append(data);

    if(totalReceivedData.contains("begin") && transmissionState == TransmissionState::Ready)
    {
        UpdateProgress(tr("Incoming transmission..."), 0);
        transmissionState = TransmissionState::Receiving;
        return;
    }

    if(totalReceivedData.contains("end") && transmissionState == TransmissionState::Receiving)
    {
        transmissionState = TransmissionState::Ready;
        UpdateProgress(tr("Finished!"), 0);

        QBuffer buffer(&receivedPhotoData);
        if(buffer.open(QIODevice::ReadOnly))
        {
            QtJP2OpenJPEGImageHandler handler(&buffer);
            QImage *image = new QImage();
            if(handler.read(image))
            {
                QPixmap jp2 = QPixmap::fromImage(*image);
                ui->photoLabel->setPixmap(jp2);
            }
        }

        receivedPhotoData.clear();
        totalReceivedData.clear();
        totalBytesReceived = 0;
        return;
    }

    if(transmissionState == TransmissionState::Receiving)
    {
        receivedPhotoData.append(data);
        UpdateProgress(tr("Downloading new photo..."), data.size());
    }
}

void MainWindow::UpdateProgress(QString message, quint16 bytesReceived)
{
    QString formattedSize = locale().formattedDataSize(totalBytesReceived += bytesReceived);
    statusBar()->showMessage(QString("%1 %2").arg(message, formattedSize));
}
