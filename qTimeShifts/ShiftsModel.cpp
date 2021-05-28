#include "ShiftsModel.h"

ShiftsModel::ShiftsModel(QObject* parent) : QAbstractItemModel(parent)
{
	rootItem = std::shared_ptr<ShiftsItem>(ShiftsItem::generateRoot());
}

ShiftsModel::ShiftsModel(const std::shared_ptr<ShiftsItem> root, QObject* parent) : QAbstractItemModel(parent)
{
	rootItem = root;
}

QVariant ShiftsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	ShiftsItem* item = getItem(index);
	QVariant& value = item->data(index.column());

	if (value.userType() == QMetaType::Bool)
	{
		if (role == Qt::CheckStateRole)
			return value.toBool() ? Qt::Checked : Qt::Unchecked;
		else
			return QVariant();
	}
	else if (role == Qt::EditRole)
	{
		return value;
	}
	else if (role == Qt::DisplayRole)
	{
		switch (value.userType())
		{
		case QMetaType::QDateTime:
			return value.toDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
		case QMetaType::QTime:
			return value.toTime().toString("HH:mm:ss.zzz");
		default:
			return value;
		}
	}
	return QVariant();

}

int ShiftsModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return ShiftsItem::Columns::COUNT;
}

Qt::ItemFlags ShiftsModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (!canChange(index)) {
		return  QAbstractItemModel::flags(index);
	}

	ShiftsItem* item = getItem(index);
	if (item->getLevel() == ShiftsItem::Level::FILE_MASK && index.column() > ShiftsItem::Columns::MASK) {
		return QAbstractItemModel::flags(index);
	}
	if (item->getLevel() == ShiftsItem::Level::FILE_SHIFT && index.column() == ShiftsItem::Columns::NEGATIVE)
		return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	return  QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

ShiftsItem* ShiftsModel::getItem(const QModelIndex& index) const
{
	if (index.isValid()) {
		ShiftsItem* item = static_cast<ShiftsItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem.get();
}

QVariant ShiftsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex ShiftsModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	ShiftsItem* parentItem = getItem(parent);
	if (!parentItem)
		return QModelIndex();

	ShiftsItem* childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

QModelIndex ShiftsModel::firstIndex() const
{
	ShiftsItem* childItem = rootItem->child(0);
	if (childItem)
		return createIndex(0, 0, childItem);
	return QModelIndex();
}

bool ShiftsModel::insertRows(int position, int rows, const QModelIndex& parent)
{
	ShiftsItem* parentItem = getItem(parent);
	if (!parentItem)
		return false;

	beginInsertRows(parent, position, position + rows - 1);
	const bool success = parentItem->insertChildren(position, rows, parentItem->getLevel() + 1);
	endInsertRows();

	return success;
}

QModelIndex ShiftsModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	ShiftsItem* childItem = getItem(index);
	ShiftsItem* parentItem = childItem ? childItem->parent() : nullptr;

	if (parentItem == rootItem.get() || !parentItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool ShiftsModel::removeRows(int position, int rows, const QModelIndex& parent)
{
	ShiftsItem* parentItem = getItem(parent);
	if (position == 0 || !parentItem)
		return false;

	beginRemoveRows(parent, position, position + rows - 1);
	const bool success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

int ShiftsModel::rowCount(const QModelIndex& parent) const
{
	const ShiftsItem* parentItem = getItem(parent);

	return parentItem ? parentItem->childCount() : 0;
}

bool ShiftsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	bool result = false;

	ShiftsItem* item = getItem(index);
	if (
		role == Qt::CheckStateRole &&
		item->getLevel() == ShiftsItem::Level::FILE_SHIFT &&
		index.column() == ShiftsItem::Columns::NEGATIVE
		)
	{
		bool v = value.toInt() == Qt::Checked;
		result = item->setData(index.column(), QVariant(v));
	}
	else if (role == Qt::EditRole)
	{
		result = item->setData(index.column(), value);
	}


	if (result)
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole, Qt::CheckStateRole, Qt::FontRole });

	return result;
}

bool ShiftsModel::canChange(const QModelIndex& index) const
{
	if (!index.isValid())
		return false;

	const QModelIndex& parent = index.parent();
	if (index.row() == 0)
		return false;
	return index.row() != 0;
}

const QModelIndex ShiftsModel::getEmptyNeighbors(QModelIndex index)
{
	if (!index.isValid())
		return QModelIndex();
	ShiftsItem* si = getItem(index);
	ShiftsItem* parent = si->parent();
	int idx = parent->getEmptyChild();
	if (idx < 0)
		return QModelIndex();
	else
	{
		return this->index(idx, 0, index.parent());
	}
}
