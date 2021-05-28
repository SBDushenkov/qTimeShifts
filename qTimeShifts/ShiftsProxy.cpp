#include "ShiftsProxy.h"

bool ShiftsProxy::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	if (left.row() == 0)
		return true;
	if (right.row() == 0)
		return false;

	QVariant leftData = sourceModel()->data(left, Qt::EditRole);
	QVariant rightData = sourceModel()->data(right, Qt::EditRole);


	if (ShiftsItem::isNewValue(leftData))
		return false;
	if (ShiftsItem::isNewValue(rightData))
		return true;

	if (leftData.userType() == QMetaType::QDateTime)
		return leftData.toDateTime() < rightData.toDateTime();
	if (leftData.userType() == QMetaType::QString)
		return leftData.toString().compare(rightData.toString(), Qt::CaseInsensitive) < 0;

	assert(true);
	return false;
}

bool ShiftsProxy::isEqual(const QVariant& a, const QVariant& b)
{
	if (a.userType() == QMetaType::QDateTime)
		return a.toDateTime() == b.toDateTime();

	if (a.userType() == QMetaType::QString)
		return a.toString().compare(b.toString(), Qt::CaseInsensitive) == 0;

	assert(true);
	return false;
}

QVariant ShiftsProxy::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::FontRole)
		return QSortFilterProxyModel::data(index, role);

	ShiftsModel* model = static_cast<ShiftsModel*>(sourceModel());
	QModelIndex indexModel = mapToSource(index);

	ShiftsItem* item = model->getItem(indexModel);
	QFont f;
	if (!model->canChange(index))
		f.setItalic(true);
	else if (item->isNewItem())
		f.setBold(true);
	if (indexModel.row() != 0)
	{
		QVariant cur = data(this->index(index.row(), 0, index.parent()), Qt::EditRole);
		QVariant prev = data(this->index(index.row() - 1, 0, index.parent()), Qt::EditRole);
		QVariant next = data(this->index(index.row() + 1, 0, index.parent()), Qt::EditRole);
		if (isEqual(prev, cur) || isEqual(next, cur))
			f.setBold(true);
	}

	return f;
}

