#ifndef XCANFRAME_H
#define XCANFRAME_H

#include "xframe.h"

#include <QObject>
#include <QByteArray>

#define PROTOCOL_ID_CONFIGURATION  	0x08
#define PROTOCOL_ID_KEYWORD_82     	0x10
#define PROTOCOL_ID_KEYWORD_71     	0x18
#define PROTOCOL_ID_IIC            	0x20
#define PROTOCOL_ID_KEYWORD_2K     	0x28
#define PROTOCOL_ID_IDB            	0x30
#define PROTOCOL_ID_ACP            	0x38
#define PROTOCOL_ID_EC             	0x40
#define PROTOCOL_ID_J1708          	0x48
#define PROTOCOL_ID_CAN1           	0x50
#define PROTOCOL_ID_CAN2           	0x58
#define PROTOCOL_ID_CLASS2         	0x60
#define PROTOCOL_ID_AOS            	0x68
#define PROTOCOL_ID_SPI            	0x70
#define PROTOCOL_ID_UART           	0x80
#define PROTOCOL_ID_J1850          	0x88
#define PROTOCOL_ID_BEAN1          	0x90
#define PROTOCOL_ID_BEAN2          	0x98
#define PROTOCOL_ID_IE_BUS         	0xB0
#define PROTOCOL_ID_LIN            	0xB8
#define PROTOCOL_ID_ISO15765_CAN1  	0xC0
#define PROTOCOL_ID_ISO15765_CAN2  	0xC8
#define PROTOCOL_ID_BLOCK_TRANSFER 	0xF8

#define SAINT_PROTOCOL_ID_MASK     	0xF8    // bit 3 ~ 7
#define SAINT_COMMAND_MASK         	0x40
#define SAINT_TXRX_MASK            	0x20
#define SAINT_TIMESTAMP_MASK       	0x01
                                   	
#define CAN1_BUS_ID                	1
#define CAN2_BUS_ID                	2
#define LIN_BUS_ID                 	3
                                   	
#define MSG_BUF_NUM                	40
#define MSG_BUF_SIZE               	256
#define MSG_MAX_LEN                	4096
#define MSG_LARGE_BUF_SIZE         	(MSG_MAX_LEN - MSG_BUF_SIZE)
                                   	
#define HOST_MSG_QUEUE_SIZE        	20
                                   	
#define TIME_STAMP_LEN             	2
#define MSG_END_STR_LEN            	2

#define DIRECTION_RX 				0
#define DIRECTION_TX 				1

#define IS_BIT_SET(data, bit)  		(((data)>>(bit))&0x1)
#define SET_BIT(data, bit) 			(data) = (data)|(1<<(bit))
#define CLEAR_BIT(data, bit) 		(data) = (data)&(~(1<<(bit)))
#define IS_EXTENDED(id) 			((id > 0xFFFF)?1:0)

class XCanFrame : public XFrame
{
public:
    enum FrameType {
        UnknownFrame        = 0x0,
        DataFrame           = 0x1,
        ErrorFrame          = 0x2,
        RemoteRequestFrame  = 0x3,
        InvalidFrame        = 0x4
    };

    explicit XCanFrame();
    XCanFrame(int bus, quint32 id, const QByteArray &msgData);
    int parseByteFrame();
    static QByteArray buildTailer(bool tm = false, quint16 timestamp = 0);
    static QByteArray buildHeader(quint32 id, int bus = 0,
        bool tm = false, bool rx = true, FrameType type = DataFrame);

    quint32 m_id;
    bool m_isExtended;
    bool m_isReceived;
    bool m_hasTimeStamp;
    FrameType m_type;
    int m_msgStart;
    int m_msgLen;  // data len
    quint16 m_timestamp;

signals:

public slots:

private:
	void buildRawData(const QByteArray &msgData);
};

#endif
