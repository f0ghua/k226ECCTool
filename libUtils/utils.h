#pragma once

#include <Qt>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>

namespace Utils {
class Base
{
	public:

		static bool decimalMode;

		static uint64_t parseStringToNum(QByteArray input)
		{
			uint64_t temp = 0;

			input = input.toUpper();
			if (input.startsWith("0X") || input.startsWith("X")) //hex number
			{
				if (input.length() < 3) temp = 0;
				else temp = input.right(input.size() - 2).toLongLong(NULL, 16);
			}
			else if (input.startsWith("B")) //binary number
			{
				input = input.right(input.size() - 1); //remove the B
				for (int i = 0; i < input.length(); i++)
				{
					if (input[i] == '1') temp += (uint64_t)1 << (input.length() - i - 1);
				}
			}
			else //decimal number
			{
				temp = input.toLongLong();
			}

			return temp;
		}

		static uint64_t parseStringToNum(QString input)
		{
			return parseStringToNum(input.toUtf8());
		}

		// either with "0x" prepend or not return the right value
		static uint64_t parseHexStringToNum(const QString &input)
		{
			return input.toLongLong(NULL, 16);
		}

		static uint64_t parseHexStringToNum(const QVariant &input)
		{
			return input.toString().toLongLong(NULL, 16);
		}

		static long getTimeMS()
		{
			QDateTime stamp = QDateTime::currentDateTime();
			return (long)(((stamp.time().hour() * 3600) + (stamp.time().minute() * 60) + (stamp.time().second()) * 1000) + stamp.time().msec());
		}

		//prints hex numbers in uppercase with 0's filling out the number depending
		//on the size needed. Promotes hex numbers to either 2, 4, or 8 digits
		static QString formatHexNum(uint64_t input)
		{
			if (input < 256)
				return "0x" + QString::number(input, 16).toUpper().rightJustified(2,'0');
			if (input < 65536)
				return "0x" + QString::number(input, 16).toUpper().rightJustified(4,'0');
			if (input < 4294967296)
				return "0x" + QString::number(input, 16).toUpper().rightJustified(8,'0');
			return "0x" + QString::number(input, 16).toUpper().rightJustified(16,'0');
		}

		static QString formatHexNumber(uint64_t input)
		{
			if (input < 256)
				return QString::number(input, 16).toUpper().rightJustified(2,'0');
			if (input < 65536)
				return QString::number(input, 16).toUpper().rightJustified(4,'0');
			if (input < 4294967296)
				return QString::number(input, 16).toUpper().rightJustified(8,'0');
			return QString::number(input, 16).toUpper().rightJustified(16,'0');
		}

		static QString formatByteArray(const QByteArray *pba)
		{
			QString tempStr;

			for (int i = 0; i < pba->count(); i++)
			{
				tempStr.append(formatHexNumber(pba->at(i)&0xFF));
				if (i < pba->count()-1)
					tempStr.append(" ");
			}
			return tempStr;
		}

		static QString formatByteArrayML(const QByteArray *pba)
		{
			QString tempStr;

			for (int i = 0; i < pba->count(); i++)
			{
				tempStr.append(formatHexNumber(pba->at(i)&0xFF));
				if (i < pba->count()-1)
				{
					if ((i+1) % 16 == 0) tempStr.append("\n");
					else tempStr.append(" ");
				}
			}
			return tempStr;
		}

		//uses decimalMode to see if it should show value as decimal or hex
		static QString formatNumber(uint64_t value)
		{
			if (decimalMode)
			{
				return QString::number(value, 10);
			}
			else return formatHexNum(value);
		}

		static QString formatByteAsBinary(uint8_t value)
		{
			QString output;
			for (int b = 7; b >= 0; b--)
			{
				if (value & (1 << b)) output += "1";
				else output += "0";
			}
			return output;
		}
};

} // namespace Helper

