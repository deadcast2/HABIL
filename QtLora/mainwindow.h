#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>

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
    void UpdateProgress(QString message, quint16 bytesReceived);

private slots:
    void readData();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort = nullptr;
    qint16 totalBytesReceived = 0;
    TransmissionState transmissionState = TransmissionState::Ready;
    QByteArray receivedPhotoData;
};

#endif // MAINWINDOW_H
