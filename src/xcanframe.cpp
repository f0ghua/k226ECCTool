#include "xcanframe.h"
#include <QDateTime>
#include <QDebug>

XCanFrame::XCanFrame() :
    XFrame(),
	m_id(),
    m_isExtended(),
    m_isReceived(),
    m_hasTimeStamp(),
    m_type(FrameType::DataFrame),
    m_timestamp()
{

}

XCanFrame::XCanFrame(int bus, quint32 id, const QByteArray &msgData)
{
	m_bus = bus;
	m_id = id;
	m_isExtended = IS_EXTENDED(id)?true:false;
	m_isReceived = true;
	m_hasTimeStamp = false;
	m_type = FrameType::DataFrame;
	m_msgLen = msgData.size();
	m_msgStart = (m_isExtended)?5:3;
	m_timestamp = (quint16)QDateTime::currentMSecsSinceEpoch();

	buildRawData(msgData);
}

void XCanFrame::buildRawData(const QByteArray &msgData)
{
	QByteArray &ba = this->m_data;
	char protocol, c;

	ba.clear();
	protocol = ((m_bus == 0)?PROTOCOL_ID_CAN1:PROTOCOL_ID_CAN2);
	c = (protocol) + (m_isReceived?0:(1<<1)) + (m_hasTimeStamp?1:0);
	ba.append(c);
	c = 0;
	if (m_isExtended) SET_BIT(c, 7);
	if (m_type != FrameType::DataFrame) SET_BIT(c, 5);
	if (!m_isExtended) {
		c |= (m_id >> 8)&0x07;
		ba.append(c);
		ba.append(m_id & 0xFF);
	}
	else {
		c |= (m_id >> 24)&0x1F;
		ba.append(c);
		ba.append((m_id >> 16) & 0xFF);
		ba.append((m_id >> 8) & 0xFF);
		ba.append((m_id) & 0xFF);
	}

	ba.append(msgData);

	if (m_hasTimeStamp) {
		ba.append((m_timestamp >> 8) & 0xFF);
		ba.append(m_timestamp & 0xFF);
	}
}

int XCanFrame::parseByteFrame()
{
    int protocol;
    int i = 0;
    QByteArray *byteFrame = &this->m_data;
    unsigned char c = byteFrame->at(i++);
    m_msgStart = 0;

    if (byteFrame->count() < 3) {
        qDebug() << "parseByteFrame byteFrame->count() = " << byteFrame->count();
        return -1;
    }

    protocol = c & SAINT_PROTOCOL_ID_MASK;
    /* We process CAN frames only */
    if ((protocol != PROTOCOL_ID_CAN1) &&
        (protocol != PROTOCOL_ID_CAN2)) {
        qDebug() << "protocol invalid: 0x" << QString::number(protocol, 16).toUpper();
        return -1;
    }
    m_bus = (protocol == PROTOCOL_ID_CAN1)?0:1;
    m_isReceived = (IS_BIT_SET(c, 1))?false:true;

    m_hasTimeStamp = c & SAINT_TIMESTAMP_MASK;

    c = byteFrame->at(i++);
    m_isExtended = (c & 0x80) >> 7;
    if (m_isExtended && (byteFrame->count() < 5)) {
        qDebug() << "extended frame with size invalid";
        return -1;
    }

    if ((c & 0x20) >> 5)
        m_type = FrameType::RemoteRequestFrame;
    else
        m_type = FrameType::DataFrame;

    if (m_type != FrameType::DataFrame) {
        qDebug() << "frame is not data frame";
        return -1;
    }

    if (!m_isExtended) {
        m_id = (c & 0x07) << 8;
        m_id |= (byteFrame->at(i++) & 0xFF) ;
    } else {
        m_id = (c & 0x1F) << 24;
        m_id |= (byteFrame->at(i++) & 0xFF) << 16;
        m_id |= (byteFrame->at(i++) & 0xFF) << 8;
        m_id |= (byteFrame->at(i++) & 0xFF);
    }

    qDebug() << "id = 0x" << QString::number(m_id, 16);
    m_msgLen = byteFrame->length() - i - 1;

    if (m_hasTimeStamp) m_msgLen -= 2;
    if (m_msgLen < 0) return -1;

    m_msgStart = i;
    qDebug() << "msgStart = " << m_msgStart << "msgLen = " << m_msgLen;
    i += m_msgLen;

    if (m_hasTimeStamp)
    {
        m_timestamp = (byteFrame->at(i++)) << 8;
        m_timestamp |= (byteFrame->at(i));
    }

	return 0;
}

QByteArray XCanFrame::buildHeader(quint32 id, int bus, bool tm, bool rx, FrameType type)
{
    QByteArray ba;
    char protocol, c;

    protocol = ((bus == 0)?PROTOCOL_ID_CAN1:PROTOCOL_ID_CAN2);
    c = (protocol) + (rx?0:(1<<1)) + (tm?1:0);
    ba.append(c);
    c = 0;
    if (IS_EXTENDED(id)) SET_BIT(c, 7);
    if (type != FrameType::DataFrame) SET_BIT(c, 5);
    if (!IS_EXTENDED(id)) {
       c |= (id >> 8)&0x07;
       ba.append(c);
       ba.append(id & 0xFF);
   }
   else {
       c |= (id >> 24)&0x1F;
       ba.append(c);
       ba.append((id >> 16) & 0xFF);
       ba.append((id >> 8) & 0xFF);
       ba.append((id) & 0xFF);
   }

   return ba;
}

QByteArray XCanFrame::buildTailer(bool tm, quint16 timestamp)
{
    QByteArray ba;

    if (tm) {
        ba.append((timestamp >> 8) & 0xFF);
        ba.append(timestamp & 0xFF);
    }

   return ba;
}
