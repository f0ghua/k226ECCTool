#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QMutex>
#include <QSettings>

class XFrame;

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = 0);
    ~SerialWorker();

public slots:
    void sendFrame(const XFrame *, int);

signals:
    void deviceSucc(int);
    void showMessage(QString);
    void frameUpdateRapid(int);

private slots:
    void readSerialData();

public slots:
    void run();
    int setSerialPort(QSerialPortInfo *);
    void closeSerialPort();

private:
    void procRXChar(unsigned char c);
    void openDevice(QString tryDev);
    void getAllDevice(QStringList &listDev);

    QSerialPort *serial;
    XFrame *buildFrame;
    QList<QSerialPortInfo> ports;
    int framesRapid;
};

#endif // SERIALWORKER_H
