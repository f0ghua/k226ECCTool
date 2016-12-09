#include "serialworker.h"
#include "xframe.h"
#include "xcanframe.h"

#include <QDebug>

static const char 	g_frameEndStr[2] 	= {(char)0xFF, (char)0x00};
static const quint8 g_frameEscapeChar 	= 0xFF;

SerialWorker::SerialWorker(QObject *parent):
    QObject(parent),
    serial()
{
    buildFrame = new XFrame();
    framesRapid = 0;
    emit showMessage("Please configure the device");
}

void SerialWorker::getAllDevice(QStringList &listDev)
{
    ports = QSerialPortInfo::availablePorts();
    for(int i = 0; i < ports.count(); i++) {
        listDev << ports.at(i).portName();
    }
}

void SerialWorker::openDevice(QString tryDev)
{
    QSerialPortInfo portInfo;
    int ret = -1;

    ports = QSerialPortInfo::availablePorts();
    for (int i = 0; i < ports.count(); i++)
    {
        if (ports.at(i).portName() == tryDev)
        {
            portInfo = ports.at(i);
            ret = setSerialPort(&portInfo);
            break;
        }
    }

    if(ret != -1) {
        emit deviceSucc(0);
        emit showMessage("ICITIS Open Success");
    }
    else {
        emit deviceSucc(1);
        emit showMessage("ICITIS Open Error");
    }
}

SerialWorker::~SerialWorker()
{
    if (serial != NULL)
    {
        if (serial->isOpen())
        {
            serial->clear();
            serial->close();

        }
        serial->disconnect(); //disconnect all signals
        delete serial;
    }

    //delete m_buildFrame;
}

int SerialWorker::setSerialPort(QSerialPortInfo *port)
{
    QSettings settings;

    if (serial != NULL)
    {
        if (serial->isOpen())
        {
            serial->clear();
            serial->close();
            qDebug()<<"close old Device";
        }
        serial->disconnect(); //disconnect all signals
        delete serial;
        serial = NULL;
    }

    serial = new QSerialPort(*port);
    serial->setBaudRate(57600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    //serial->setFlowControl(QSerialPort::HardwareControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        emit showMessage("Device " + port->portName() + " can not be opened");
        serial->clear();
        serial->close();
        delete serial;
        serial = NULL;
        return -1;
    }
    else {
        emit showMessage("Device has been opened");
        serial->setDataTerminalReady(true); //you do need to set these or the fan gets dirty
        serial->setRequestToSend(true);

        connect(serial, &QSerialPort::readyRead, this, &SerialWorker::readSerialData);
        return 0;
    }

}

void SerialWorker::closeSerialPort()
{
    if (serial == NULL) return;
    if (serial->isOpen())
    {
        serial->clear();
        serial->close();
    }
    serial->disconnect();
    //do not stop the ticker here. It always stays running now.
    //ticker->stop();
    delete serial;
    serial = NULL;
    emit showMessage("Device has been closed");
    emit deviceSucc(1);
}

void SerialWorker::run()
{

}

void SerialWorker::readSerialData()
{
    unsigned char c;
    QByteArray data = serial->readAll();
	
    //qDebug() << (tr("Got data from serial. Len = %0").arg(data.length()));

    for (int i = 0; i < data.length(); i++)
    {
        c = data.at(i);
        procRXChar(c);
    }
    if (framesRapid > 0)
    {
        emit frameUpdateRapid(framesRapid);
        framesRapid = 0;
    }
    //printf("\n");
}

void SerialWorker::procRXChar(unsigned char c)
{
    static unsigned char ffFlag = 0;
	static QByteArray cba = QByteArray();

    if ((c == 0xFF) && (ffFlag == 0)) { // first 0xFF
        ffFlag = 1;
        return;
    }

    if (ffFlag == 0) {
        cba.append(c);
    }
    else {
        ffFlag = 0;
        if (c == 0xFF) { // 2 0xFF, the first one means escape character
            cba.append(c);
        }
        else if (c == 0x00) { // end of message
			// buildFrame->buildFrame(cba);
			// appendFrame(*buildFrame);
			framesRapid++;

            cba.clear();
        } else { // a new message start
			// buildFrame->buildFrame(cba);
			// appendFrame(*buildFrame);
			framesRapid++;

            cba.clear();
            cba.append(c);
        }
    }
}

void SerialWorker::sendFrame(const XFrame *frame, int /*bus = 0*/)
{
    QByteArray buffer;
    QByteArray ba;

    if (serial == NULL) return;
    if (!serial->isOpen()) return;

    ba = XCanFrame::buildHeader(frame->id(), frame->bus());
    buffer.append(ba);
    for (int i = 0; i < frame->data().count(); i++) {
        buffer.append(frame->data().at(i));
        if ((quint8)frame->data().at(i) == 0xFF) {
            buffer.append(0xFF);
        }
    }

    buffer.append(QByteArray::fromRawData(g_frameEndStr, sizeof(g_frameEndStr)));

#ifndef F_NO_DEBUG
    qDebug() << "writing " << buffer.length() << " bytes to serial port";
#endif
    serial->write(buffer);
}
