#include "ShiftsSerializer.h"

const QString ShiftsSerializer::fileHeader = QStringLiteral(
u"# #comment - comments will be removed\r\n"
u"# *FILEMASK - file mask (case insentetive)\r\n"
u"# time shift begins (yyyy-MM-ddTHH:mm:ss.zzz), signed shift (+HH:mm:ss.zzz), comment");

const QString ShiftsSerializer::ignored =QStringLiteral(u"ignored: ");

int ShiftsSerializer::readFile(QString path, std::shared_ptr<ShiftsItem>& rootItem)
{
	QFile file(path);
	bool skipLine = false;
	QSet<QString> names;
	QSet<QDateTime> dates;

	if (!file.exists())
		return FILE_DOESTN_EXIST;
	if (!file.open(QFile::ReadOnly))
		return FILE_OPEN_ERROR;


	int result = FILE_READ_SUCCESS;

	QTextStream in(&file);
	in.setEncoding(QStringConverter::encodingForName("Windows-1251").value_or(QStringConverter::System));
	in.setAutoDetectUnicode(true);
	while (!in.atEnd())
	{
		QString line = in.readLine().simplified();
		if (line.isNull())
		{
			result = FILE_READ_ERROR;
			break;
		}

		if (!parseLine(rootItem, names, dates, skipLine, line))
			qInfo() << (ignored + line);
	}

	file.close();
	return result;
}

int ShiftsSerializer::writeFile(QString path, std::shared_ptr<ShiftsItem>& rootItem)
{
	QFile file(path);
	if (!file.open(QIODeviceBase::WriteOnly))
		return FILE_OPEN_ERROR;

	int result = FILE_WRITE_SUCCESS;

	QTextStream out(&file);
	out.setGenerateByteOrderMark(true);
	out << fileHeader << Qt::endl << Qt::endl;


	for (int i = 0; i < rootItem->childCount(); ++i)
	{
		ShiftsItem* maskItem = rootItem->child(i);
		if (i == 0)
			out << QStringLiteral(u"# Default shifts") << Qt::endl;
		else
		{
			out
				<< QStringLiteral(u"*")
				<< maskItem->data(ShiftsItem::Columns::MASK).toString()
				<< Qt::endl;
		}
		for (int j = 1; j < maskItem->childCount(); ++j)
		{
			ShiftsItem* si = maskItem->child(j);
			const QVector<QVariant>& shiftsData = maskItem->child(j)->getRawData();
			out << shiftString(
				shiftsData.at(ShiftsItem::Columns::MASK).toDateTime(),
				shiftsData.at(ShiftsItem::Columns::NEGATIVE).toBool(),
				shiftsData.at(ShiftsItem::Columns::SHIFT).toTime(),
				shiftsData.at(ShiftsItem::Columns::COMMENT).toString()
			);
			out << Qt::endl;
		}
	}

	file.close();
	return result;
}

bool ShiftsSerializer::parseLine(std::shared_ptr<ShiftsItem>& rootItem, QSet<QString>& names, QSet<QDateTime>& dates, bool& skipLine, const QString& line)
{
	int len = line.size();
	if (line.isEmpty())
		return false;
	QChar ch = line[0];
	if (ch == u'#')
		return false;

	if (ch == u'*')
		return parseName(rootItem, names, dates, skipLine, line);

	if (skipLine)
		return false;
	else
		return parseShift(rootItem, names, dates, line);
}

bool ShiftsSerializer::parseName(std::shared_ptr<ShiftsItem>& rootItem, QSet<QString>& names, QSet<QDateTime>& dates, bool& skipLine, const QString& line)
{
	QString name = line.mid(1Ui64).simplified();
	QString toList;

	if (name.isEmpty())
		return false;

	if (names.contains(name))
	{
		skipLine = true;
		return false;
	}
	names.insert(name);

	skipLine = false;
	dates.clear();

	QVector<QVariant> fileNameData(ShiftsItem::Columns::COUNT, QVariant());
	fileNameData.replace(ShiftsItem::Columns::MASK, name);
	rootItem->appendChild(new ShiftsItem(fileNameData, ShiftsItem::Level::FILE_MASK, rootItem.get()));

	toList = QStringLiteral(u"%1%2")
		.arg(QStringLiteral(u"Mask (%1): ").arg(rootItem->childCount(), 3), prefixLength)
		.arg(name);
	qInfo().noquote() << toList;

	return true;
}

 bool ShiftsSerializer::parseShift(std::shared_ptr<ShiftsItem>& rootItem, QSet<QString>& names, QSet<QDateTime>& dates, const QString& line)
{
	qsizetype a = line.indexOf(u',', 0Ui64);
	if (a < 0Ui64)
		return false;
	qsizetype b = line.indexOf(u',', a + 1Ui64);

	QDateTime dateTime = TimeConverter::stringToDateTime(line.mid(0Ui64, a).simplified());
	QString shift = line.mid(a + 1Ui64, b - a - 1Ui64).simplified();

	if (shift.isEmpty())
		return false;

	QChar ch = shift.at(0Ui64);
	bool sign = ch == u'-';
	qsizetype n = 0;
	if (ch == u'-' || ch == u'+')
		++n;

	shift = shift.mid(n);
	QTime time = TimeConverter::stringToTime(shift);

	if (!dateTime.isValid() || !time.isValid() || dateTime < ShiftsItem::getZeroDateTime())
		return false;

	QString comment;
	if (b > 0)
		comment = line.mid(b + 1Ui64).simplified();

	if (dates.contains(dateTime))
		return false;

	dates.insert(dateTime);

	QVector<QVariant> shiftDefault(ShiftsItem::Columns::COUNT, QVariant());
	shiftDefault.replace(ShiftsItem::Columns::MASK, dateTime);
	shiftDefault.replace(ShiftsItem::Columns::NEGATIVE, sign);
	shiftDefault.replace(ShiftsItem::Columns::SHIFT, time);
	shiftDefault.replace(ShiftsItem::Columns::COMMENT, comment);
	ShiftsItem* shiftItem = rootItem->child(rootItem->childCount() - 1);
	shiftItem->appendChild(new ShiftsItem(shiftDefault, ShiftsItem::Level::FILE_SHIFT, shiftItem));

	QString toList = QStringLiteral(u"%1")
		.arg(QStringLiteral(u"Shift (%1): ").arg(shiftItem->childCount(), 3), prefixLength);

	toList = toList + shiftString(dateTime, sign, time, comment);
	qInfo().noquote() << toList;
	return true;
}

