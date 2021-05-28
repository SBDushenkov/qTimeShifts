#include "ShiftsItem.h"

QChar ShiftsItem::asterix = u'*';
QTime ShiftsItem::zeroTime = QTime::fromMSecsSinceStartOfDay(0Ui64);
QDateTime ShiftsItem::zeroDateTime = QDateTime::fromSecsSinceEpoch(0Ui64, Qt::UTC);

ShiftsItem::ShiftsItem(const QVector<QVariant>& data, int _level, ShiftsItem* parent)
	: itemData(data), parentItem(parent), level(_level)
{
	switch (level) {
	case Level::ROOT:
		appendDefaultMask();
		break;
	case Level::FILE_MASK:
		appendDefaultShift();
		break;
	case Level::FILE_SHIFT:
		break;
	}
}

ShiftsItem* ShiftsItem::generateRoot()
{
	QVector<QVariant> rootData(ShiftsItem::Columns::COUNT);
	rootData.replace(ShiftsItem::Columns::MASK, QObject::tr("Name"));
	rootData.replace(ShiftsItem::Columns::NEGATIVE, QObject::tr("Backward?"));
	rootData.replace(ShiftsItem::Columns::SHIFT, QObject::tr("Shift"));
	rootData.replace(ShiftsItem::Columns::COMMENT, QObject::tr("Comment"));
	return new ShiftsItem(rootData);
}

ShiftsItem::~ShiftsItem() {
	qDeleteAll(childItems);
}

void ShiftsItem::appendDefaultMask() {
	QVector<QVariant> maskDefault(ShiftsItem::Columns::COUNT, QVariant());
	maskDefault.replace(ShiftsItem::Columns::MASK, QString(QObject::tr("Defualt")));
	ShiftsItem* si = new ShiftsItem(maskDefault, ShiftsItem::Level::FILE_MASK, this);
	appendChild(si);
}

void ShiftsItem::appendDefaultShift() {
	QVector<QVariant> shiftDefault(ShiftsItem::Columns::COUNT, QVariant());
	shiftDefault.replace(ShiftsItem::Columns::MASK, zeroDateTime);
	shiftDefault.replace(ShiftsItem::Columns::NEGATIVE, QVariant(false));
	shiftDefault.replace(ShiftsItem::Columns::SHIFT, zeroTime);
	appendChild(new ShiftsItem(shiftDefault, ShiftsItem::Level::FILE_SHIFT, this));
}

ShiftsItem* ShiftsItem::child(int number)
{
	if (number < 0 || number >= childItems.size())
		return nullptr;
	return childItems.at(number);
}

QVariant ShiftsItem::data(int column) const
{
	if (column < 0 || column >= itemData.size())
		return QVariant();
	return itemData.at(column);
}

bool ShiftsItem::insertChildren(int position, int count, int _level)
{
	if (position < 0 || position > childItems.size() || _level >= Level::LEVEL_COUNT)
		return false;
	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(Columns::COUNT, QString(""));

		switch (_level) {
		case Level::FILE_MASK:
			data.fill(QString(""));
			data.replace(Columns::MASK, QString(asterix));
			break;
		case Level::FILE_SHIFT:
			data.replace(Columns::MASK, zeroDateTime);
			data.replace(Columns::NEGATIVE, false);
			data.replace(Columns::SHIFT, QTime(0, 0, 0));
			break;
		}
		ShiftsItem* si = new ShiftsItem(data, level + 1, this);

		childItems.insert(position, si);
	}
	return true;
}

bool ShiftsItem::removeChildren(int position, int count)
{
	if (position < 0 || (qsizetype)position + (qsizetype)count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

int ShiftsItem::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<ShiftsItem*>(this));
	return 0;
}

bool ShiftsItem::setData(int column, const QVariant& value)
{
	if (column < 0 || column >= itemData.size())
		return false;
	if (column < 0 || column >= itemData.size())
		return false;


	if (value.userType() == QMetaType::QString)
	{
		QString str = value.toString().simplified();
		if (level == Level::FILE_MASK && column == Columns::MASK)
		{
			QChar ch;
			if (str.isEmpty() || str.at(0) == asterix)
				ch = asterix;
		}
		itemData[column] = str;
	}

	itemData[column] = value;
	return true;
}

int ShiftsItem::getEmptyChild() const
{
	for (int i = 1; i < childItems.size(); ++i)
	{
		if (ShiftsItem::isNewValue(childItems.at(i)->data(0)))
			return i;
	}
	return -1;
}

bool ShiftsItem::isNewValue(const QVariant& value)
{
	if (value.userType() == QMetaType::QString)
	{
		return value.toString().at(0) == asterix;
	}
	if (value.userType() == QMetaType::QDateTime)
	{
		return value.toDateTime() == zeroDateTime;
	}
	return false;
}

inline std::shared_ptr<ShiftsItem> ShiftsItem::makeCopy() const
{
	if (level != ROOT)
		return std::shared_ptr<ShiftsItem>();
	std::shared_ptr<ShiftsItem> root = std::shared_ptr<ShiftsItem>(new ShiftsItem(itemData, level, childItems, parentItem));
	return root;
}

inline bool ShiftsItem::lessMask(ShiftsItem* a, ShiftsItem* b)
{
	QString aString = a->getRawData().at(Columns::MASK).toString();
	QString bString = b->getRawData().at(Columns::MASK).toString();
	return aString.compare(bString, Qt::CaseInsensitive) < 0;
}

inline bool ShiftsItem::lessShift(ShiftsItem* a, ShiftsItem* b)
{
	QDateTime aDT = a->getRawData().at(Columns::MASK).toDateTime();
	QDateTime bDT = b->getRawData().at(Columns::MASK).toDateTime();
	return aDT < bDT;
}

inline bool ShiftsItem::isEqualMask(ShiftsItem* a, ShiftsItem* b)
{
	QString aString = a->getRawData().at(Columns::MASK).toString();
	QString bString = b->getRawData().at(Columns::MASK).toString();
	return aString.compare(bString, Qt::CaseInsensitive) == 0;
}

inline bool ShiftsItem::isEqualShift(ShiftsItem* a, ShiftsItem* b)
{
	QDateTime aDT = a->getRawData().at(Columns::MASK).toDateTime();
	QDateTime bDT = b->getRawData().at(Columns::MASK).toDateTime();
	return aDT == bDT;
}

std::shared_ptr<ShiftsItem> ShiftsItem::getClearRootItem() const
{
	if (level != ROOT)
		return std::shared_ptr<ShiftsItem>();
	std::shared_ptr<ShiftsItem> clearRoot = std::shared_ptr<ShiftsItem>(new ShiftsItem(itemData, level, childItems, parentItem));
	std::sort(clearRoot->childItems.begin() + 1, clearRoot->childItems.end(), lessMask);

	int i = 1;
	while (i < clearRoot->childCount())
	{
		if (ShiftsItem::isEqualMask(clearRoot->child(i - 1), clearRoot->child(i)) ||
			clearRoot->child(i)->isNewItem())
		{
			clearRoot->childItems.removeAt(i);
		}
		else
			++i;
	}

	for (int i = 0; i < clearRoot->childCount(); ++i)
	{
		ShiftsItem* maskItem = clearRoot->child(i);
		std::sort(maskItem->childItems.begin() + 1, maskItem->childItems.end(), lessShift);
		int j = 1;
		while (j < maskItem->childCount())
		{
			if (ShiftsItem::isEqualShift(maskItem->child(j - 1), maskItem->child(j)) ||
				maskItem->child(j)->isNewItem())
			{
				maskItem->childItems.removeAt(j);
			}
			else
				++j;
		}
	}
	return clearRoot;
}

ShiftsItem::ShiftsItem(const QVector<QVariant>& _data, const int _level, const QList<ShiftsItem*>& _childItems, ShiftsItem* _parent)
	: itemData(_data), parentItem(_parent), level(_level)
{
	for (ShiftsItem* si : _childItems)
		appendChild(new ShiftsItem(si->itemData, si->level, si->childItems, this));
}
