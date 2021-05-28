#pragma once

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>

class TimeConverter
{
public:
	static QDate stringToDate(const QString str);

	static QTime stringToTime(const QString str);

	static QDateTime stringToDateTime(const QString str);

	static QString dateTimeToString(const QDateTime dateTime);

	static QString timeToString(const QTime time);

	static const QString dateTimeFormat;
	static const QString timeFormat;
};
const QString fileName("TimeShifts.txt");