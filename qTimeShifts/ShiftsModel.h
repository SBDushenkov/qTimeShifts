#pragma once
#include "ShiftsItem.h"

#include <qfont.h>
#include <qabstractitemmodel.h>

class ShiftsModel :
	public QAbstractItemModel
{
public:
	Q_OBJECT
public:
	ShiftsModel(QObject* parent = nullptr);

	ShiftsModel(const std::shared_ptr<ShiftsItem> root, QObject* parent = nullptr);

	~ShiftsModel() { }

	QVariant data(const QModelIndex& index, int role) const override;

	int columnCount(const QModelIndex& parent) const override;

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	ShiftsItem* getItem(const QModelIndex& index) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	QModelIndex index(int row, int column, const QModelIndex& parent) const override;

	QModelIndex firstIndex() const;

	bool insertRows(int position, int rows, const QModelIndex& parent) override;

	QModelIndex parent(const QModelIndex& index) const override;

	bool removeRows(int position, int rows, const QModelIndex& parent) override;

	int rowCount(const QModelIndex& parent) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	bool canChange(const QModelIndex& index) const;

	const QModelIndex getEmptyNeighbors(QModelIndex index);

	inline const std::shared_ptr<ShiftsItem> getRootItem() const
	{
		return rootItem;
	}


private:
	std::shared_ptr<ShiftsItem> rootItem;
};

