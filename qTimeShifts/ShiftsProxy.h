#pragma once

#include <qsortfilterproxymodel.h>
#include <qdatetime.h>

#include "ShiftsModel.h"
#include "ShiftsItem.h"

class ShiftsProxy :
	public QSortFilterProxyModel
{
public:
	Q_OBJECT
public:
	ShiftsProxy(QObject* parent) : QSortFilterProxyModel(parent)
	{
		setDynamicSortFilter(true);
	}

private:
	QVariant data(const QModelIndex& index, int role) const override;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

	static bool isEqual(const QVariant& a, const QVariant& b);

};

