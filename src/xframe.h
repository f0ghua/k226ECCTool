#ifndef XFRAME_H
#define XFRAME_H

#include <QByteArray>

class XFrame
{
public:
    XFrame(int bus = 0, const QByteArray &data = QByteArray()) :
    	m_bus(bus),
        m_dlc(0),
        m_id(0),
        m_data(data)
    {
        //Q_UNUSED(reserved);
    }
    XFrame(const XFrame& f)
    {
        m_bus = f.m_bus;
		m_dlc = f.m_dlc;
		m_id = f.m_id;
		m_data = f.m_data;
    }
    int bus() const {return m_bus;}
    void setBus(int i) {m_bus = i;}
    int dlc() const {return m_dlc;}
    void setDlc(int dlc)
    {
        if (dlc > m_dlc) {
            QByteArray ba((dlc-m_dlc), 0);
            m_data.append(ba);
        } else {
            m_data.truncate(dlc);
        }
        m_dlc = dlc;
    }
    quint32 id() const {return m_id;}
    void setId(quint32 v) {m_id = v;}
    const QByteArray data() const {return m_data;}
	QByteArray &data() {return m_data;}
	void setData(const QByteArray &data) { m_data = data; }
    void setData(int i, char ch)
    {
        if (i < m_data.count()) {
            m_data[i] = ch;
        }
    }

	int m_bus;
    int m_dlc;  		// data payload length
    quint32 m_id;
	QByteArray m_data; 	// msg data, no include header and tailer
};

#endif // XFRAME_H
