#pragma once
#include <qlist.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <QVector>
#include <algorithm>

class ShiftsItem
{

public:
	explicit ShiftsItem(const QVector<QVariant>& data, int _level = 0, ShiftsItem* parent = nullptr);
	ShiftsItem(ShiftsItem&) = delete;
	ShiftsItem(ShiftsItem&&) = delete;

	static ShiftsItem* generateRoot();

	~ShiftsItem();

	void appendDefaultMask();
	void appendDefaultShift();

	ShiftsItem* child(int number);
	
	inline int childCount() const { return childItems.size(); }

	QVariant data(int column) const;

	bool insertChildren(int position, int count, int _level);

	inline ShiftsItem* parent() { return parentItem; }

	bool removeChildren(int position, int count);

	int childNumber() const;

	bool setData(int column, const QVariant& value);


	const QVector<QVariant> getRawData() const
	{
		return itemData;
	}

	int getEmptyChild() const;

	enum Level 
	{
		ROOT = 0,
		FILE_MASK,
		FILE_SHIFT,
		LEVEL_COUNT
	};
	enum Columns 
	{
		MASK = 0,
		NEGATIVE,
		SHIFT,
		COMMENT,
		COUNT
	};
	inline void appendChild(ShiftsItem* si) { childItems.push_back(si); }

	int getLevel() const noexcept { return level; }

	inline bool isNewItem() const { return isNewValue(itemData.at(0)); }
	
	static bool isNewValue(const QVariant& value);

	std::shared_ptr<ShiftsItem> makeCopy() const;

	static bool lessMask(ShiftsItem* a, ShiftsItem* b);
	static bool lessShift(ShiftsItem* a, ShiftsItem* b);
	static bool isEqualMask(ShiftsItem* a, ShiftsItem* b);
	static bool isEqualShift(ShiftsItem* a, ShiftsItem* b);

	std::shared_ptr<ShiftsItem> getClearRootItem() const;

	static QChar getAsterix() { return asterix; }
	static QDateTime getZeroDateTime() { return zeroDateTime; }
	static QTime getZeroTime(){ return zeroTime; }

protected:
private:
	explicit ShiftsItem(const QVector<QVariant>& _data, const int _level, const QList<ShiftsItem*>& _childItems, ShiftsItem* _parent);

	QList<ShiftsItem*> childItems;
	QVector<QVariant> itemData;
	ShiftsItem* parentItem;
	int level;

	static QChar asterix;
	static QTime zeroTime;
	static QDateTime zeroDateTime;
};

