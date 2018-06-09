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

    QByteArray startBytes = QByteArray("\x6A\x70\x32", 3);
    QByteArray endBytes = QByteArray("\xd9", 1);
    if(data.indexOf(jp2Header) > -1)
    {
        transmissionState = TransmissionState::Receiving;
    }
    else if(data.endsWith(jp2Footer))
    {
        receivedPhotoData.append(data);
        UpdateProgress(tr("Finished!"), data.size());
        transmissionState = TransmissionState::Ready;
        totalBytesReceived = 0;

        QImage image(160, 120, QImage::Format_RGB32);
        if(!image.loadFromData(receivedPhotoData))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not load new photo"));
        }
        else
        {
            QPixmap map(160, 120);
            if(!map.convertFromImage(image))
            {
                QMessageBox::critical(this, tr("Error"), tr("Could not create pixmap"));
            }
            else
            {
                ui->photoLabel->setPixmap(map);
            }
        }
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
