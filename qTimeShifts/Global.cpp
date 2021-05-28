#include <qglobal.h>
#include "Global.h"

const QString TimeConverter::dateTimeFormat = QStringLiteral(u"yyyy-MM-ddTHH:mm:ss.zzz");
const QString TimeConverter::timeFormat = QStringLiteral(u"HH:mm:ss.zzz");


QString TimeConverter::dateTimeToString(const QDateTime dateTime)
{
	return dateTime.toString(dateTimeFormat);
}

QString TimeConverter::timeToString(const QTime time)
{
	return time.toString(timeFormat);
}

QDate TimeConverter::stringToDate(const QString str)
{
	bool success;

	int y = 1970;
	int M = 1;
	int d = 1;

	QStringList list = str.split(u'-');

	y = list.at(0Ui64).toInt(&success);
	if (!success)
		return QDate();

	if (list.size() > 1Ui64)
	{
		M = list.at(1Ui64).toInt(&success);
		if (!success)
			return QDate();
	}

	if (list.size() > 2Ui64)
	{
		d = list.at(2Ui64).toInt(&success);
		if (!success)
			return QDate();

	}
	return QDate(y, M, d);
}

QTime TimeConverter::stringToTime(const QString str)
{
	bool success;

	int h = 0;
	int m = 0;
	int s = 0;
	int z = 0;



	QStringList hms = str.split(u':');

	if (hms.isEmpty())
		return QTime();

	h = hms.at(0Ui64).toInt(&success);
	if (!success)
		return QTime();

	if (hms.size() > 1Ui64)
	{
		m = hms.at(1Ui64).toInt(&success);
		if (!success)
			return QTime();
	}

	if (hms.size() > 2Ui64)
	{
		QStringList timeMsec = hms.at(2Ui64).split(u'.');
		if (timeMsec.isEmpty())
			return QTime();
		s = timeMsec.at(0).toInt(&success);
		if (!success)
			return QTime();
		if (timeMsec.size() > 1Ui64)
		{
			z = timeMsec.at(1).toInt(&success);
			if (!success)
				return QTime();
		}

	}

	return QTime(h, m, s, z);
}

QDateTime TimeConverter::stringToDateTime(const QString str)
{
	if (str.isEmpty())
		return QDateTime();

	QStringList list = str.split(u'T');

	if (list.size() != 2Ui64)
		return QDateTime();

	QDate date = stringToDate(list.at(0));
	QTime time = stringToTime(list.at(1));

	return QDateTime(date, time, Qt::UTC);
}
