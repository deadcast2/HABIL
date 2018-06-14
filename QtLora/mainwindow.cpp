#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtJP2OpenJPEGImageHandler.h"

#include <QMessageBox>
#include <QFile>
#include <QBuffer>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new QSerialPort(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    loadComPortList();
    comPortChanged();
    startLogging();
    transmissionTimer = new QTimer(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::refreshComPorts);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::comPortChanged);
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(transmissionTimer, SIGNAL(timeout()), this, SLOT(cancelTransmission()));
}

MainWindow::~MainWindow()
{
    stopLogging();
    delete ui;
}

void MainWindow::cancelTransmission()
{
    stopTransmissionTimer();
    updateProgress(tr("Transmission cancelled due to timeout."), 0);
    prepareForNextTransmission();
}

void MainWindow::loadComPortList()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    ui->comboBox->clear();
    for(int i = 0; i < ports.size(); i++)
    {
        ui->comboBox->addItem(ports[i].portName());
    }
}

void MainWindow::refreshComPorts()
{
    loadComPortList();
}

void MainWindow::comPortChanged()
{
    if(ui->comboBox->currentText() == "") return;
    if(serialPort->isOpen()) serialPort->close();
    serialPort->setPortName(ui->comboBox->currentText());
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(!serialPort->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), QString("Could not open %1 port.").arg(ui->comboBox->currentText()));
    }
}

void MainWindow::readData()
{
    const QByteArray data = serialPort->readAll();
    totalReceivedData.append(data);
    writeToLog(data);
    signalReceived();

    if(totalReceivedData.contains("begin") && transmissionState == TransmissionState::Ready)
    {
        prepareForNextTransmission();
        if(ui->checkBox->isChecked()) QApplication::beep();
        updateProgress(tr("Incoming transmission..."), 0);
        startTransmissionTimer();
        transmissionState = TransmissionState::Receiving;
        return;
    }

    if(totalReceivedData.contains("end") && transmissionState == TransmissionState::Receiving)
    {
        stopTransmissionTimer();
        QDateTime finishedAt = QDateTime::currentDateTime();
        updateProgress(QString("Finished @ %1!").arg(finishedAt.toString("hh:mma")), 0);
        savePhoto(finishedAt);
        showPhoto();
        prepareForNextTransmission();
        return;
    }

    if(transmissionState == TransmissionState::Receiving)
    {
        receivedPhotoData.append(data);
        updateProgress(tr("Receiving new photo..."), data.size());
        startTransmissionTimer();
    }
}

bool MainWindow::showPhoto()
{
    QBuffer buffer(&receivedPhotoData);
    if(buffer.open(QIODevice::ReadOnly))
    {
        QtJP2OpenJPEGImageHandler handler(&buffer);
        QImage image;
        if(handler.read(&image))
        {
            ui->photoLabel->setPixmap(QPixmap::fromImage(image));
            return true;
        }
    }
    return false;
}

bool MainWindow::savePhoto(QDateTime timestamp)
{
    QString photoPath = QString("%1/HABIL").arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    QDir photoDirectory;
    if(photoDirectory.mkpath(photoPath))
    {
        QString photoName = QString("%1/%2.jp2").arg(photoPath, timestamp.toString("MM-dd-yy-hh-mma"));
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

void MainWindow::prepareForNextTransmission()
{
    transmissionState = TransmissionState::Ready;
    receivedPhotoData.clear();
    totalReceivedData.clear();
    totalBytesReceived = 0;
}

void MainWindow::updateProgress(QString message, quint16 bytesReceived)
{
    QString formattedSize = locale().formattedDataSize(totalBytesReceived += bytesReceived);
    statusBar()->showMessage(QString("%1 (%2 received)").arg(message, formattedSize));
}

void MainWindow::startTransmissionTimer()
{
    transmissionTimer->start(30000); // 30 seconds
}

void MainWindow::stopTransmissionTimer()
{
    transmissionTimer->stop();
}

void MainWindow::startLogging()
{
    logFile.setFileName("log.txt");
    logFile.open(QFile::WriteOnly | QFile::Truncate);
}

void MainWindow::stopLogging()
{
    if(logFile.isOpen()) logFile.close();
}

void MainWindow::writeToLog(QByteArray data)
{
    if(logFile.isOpen()) logFile.write(data);
}

void MainWindow::signalReceived()
{
    ui->signalLabel->setEnabled(true);
    QTimer::singleShot(500, [&]{ ui->signalLabel->setEnabled(false); });
}
