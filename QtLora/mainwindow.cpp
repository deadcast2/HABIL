#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtJP2OpenJPEGImageHandler.h"

#include <QMessageBox>
#include <QFile>
#include <QBuffer>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>

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
        QDateTime finishedAt = QDateTime::currentDateTime();
        UpdateProgress(QString("Finished! %1").arg(finishedAt.toString()), 0);
        SavePhoto(finishedAt);
        ShowPhoto();
        PrepareForNextTransmission();
        return;
    }

    if(transmissionState == TransmissionState::Receiving)
    {
        receivedPhotoData.append(data);
        UpdateProgress(tr("Receiving new photo..."), data.size());
    }
}

bool MainWindow::ShowPhoto()
{
    QBuffer buffer(&receivedPhotoData);
    if(buffer.open(QIODevice::ReadOnly))
    {
        QtJP2OpenJPEGImageHandler handler(&buffer);
        QImage image;
        if(handler.read(&image))
        {
            QPixmap pixmap = QPixmap::fromImage(image);
            ui->photoLabel->setPixmap(pixmap);
            return true;
        }
    }
    return false;
}

bool MainWindow::SavePhoto(QDateTime timestamp)
{
    QString photoPath = QString("%1/HABIL").arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    QDir photoDirectory;
    if(photoDirectory.mkpath(photoPath))
    {
        QString photoName = QString("%1/%2.jp2").arg(photoPath, timestamp.toString("MM-dd-yy-h-ma"));
        QFile savedCopy(photoName);
        if(savedCopy.open(QFile::WriteOnly))
        {
            savedCopy.write(receivedPhotoData);
            savedCopy.close();
            return true;
        }
    }
    return false;
}

void MainWindow::PrepareForNextTransmission()
{
    receivedPhotoData.clear();
    totalReceivedData.clear();
    totalBytesReceived = 0;
}

void MainWindow::UpdateProgress(QString message, quint16 bytesReceived)
{
    QString formattedSize = locale().formattedDataSize(totalBytesReceived += bytesReceived);
    statusBar()->showMessage(QString("%1 (%2)").arg(message, formattedSize));
}
