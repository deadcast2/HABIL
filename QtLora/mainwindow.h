#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QFile>

namespace Ui {
    class MainWindow;
}

enum TransmissionState { Ready, Receiving };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadComPortList();
    void refreshComPorts();
    void updateProgress(QString message, quint16 bytesReceived);
    bool savePhoto(QDateTime timestamp);
    bool showPhoto();
    void prepareForNextTransmission();
    void startTransmissionTimer();
    void stopTransmissionTimer();
    void startLogging();
    void stopLogging();
    void writeToLog(QByteArray data);

private slots:
    void readData();
    void comPortChanged();
    void cancelTransmission();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort = nullptr;
    qint16 totalBytesReceived = 0;
    TransmissionState transmissionState = TransmissionState::Ready;
    QByteArray receivedPhotoData;
    QByteArray totalReceivedData;
    QTimer *transmissionTimer;
    QFile logFile;
};

#endif // MAINWINDOW_H
