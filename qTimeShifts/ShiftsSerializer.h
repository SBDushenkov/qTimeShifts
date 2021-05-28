#pragma once

#include <qfile.h>
#include <memory>

#include "ShiftsItem.h"
#include "Global.h"

class ShiftsSerializer
{
public:
	static int readFile(QString path, std::shared_ptr<ShiftsItem>& rootItem);

	static int writeFile(QString path, std::shared_ptr<ShiftsItem>& rootItem);


	enum Status
	{
		FILE_DOESTN_EXIST,
		FILE_OPEN_ERROR,
		FILE_READ_ERROR,
		FILE_READ_SUCCESS,
		FILE_WRITE_ERROR,
		FILE_WRITE_SUCCESS
	};

private:
	static bool parseLine(
		std::shared_ptr<ShiftsItem>& rootItem,
		QSet<QString>& names,
		QSet<QDateTime>& dates,
		bool& skipLine,
		const QString& line
	);

	static bool parseName(
		std::shared_ptr<ShiftsItem>& rootItem, 
		QSet<QString>& names,
		QSet<QDateTime>& dates,
		bool& skipLine,
		const QString& line
	);

	static bool parseShift(
		std::shared_ptr<ShiftsItem>& rootItem, 
		QSet<QString>& names,
		QSet<QDateTime>& dates,
		const QString& line
	);

	static inline QString shiftString(const QDateTime& dateTime, bool sign, const QTime& time, QString comment)
	{
		return QStringLiteral(u"%1, %2%3, %4")
			.arg(TimeConverter::dateTimeToString(dateTime))
			.arg(sign ? QStringLiteral(u"-") : QStringLiteral(u"+"))
			.arg(TimeConverter::timeToString(time))
			.arg(comment);
	}

	//std::shared_ptr<ShiftsItem> rootItem;

	static const int prefixLength = 25;
	static const QString ignored;


	static const QString fileHeader;
};

