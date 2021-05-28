#pragma once

#include <QtWidgets/qitemdelegate.h>
#include <QtWidgets/qdatetimeedit.h>

#include "ShiftsProxy.h"
#include "ShiftsItem.h"
#include "ShiftsModel.h"

class ShiftsDelegate :
	public QItemDelegate
{
	Q_OBJECT
public:
	explicit ShiftsDelegate(QObject* parent) : QItemDelegate(parent)
	{

	}

	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{

		const ShiftsProxy* proxyModel = static_cast<const ShiftsProxy*>( index.model());
		const ShiftsModel* model = static_cast<const ShiftsModel*>(proxyModel->sourceModel());
		const QModelIndex modelIndex = proxyModel->mapToSource(index);
		const ShiftsItem* si = model->getItem(modelIndex);


		if (
			si->getLevel() == ShiftsItem::Level::FILE_SHIFT && 
			index.column() == ShiftsItem::Columns::MASK
			)
		{
			QDateTimeEdit* dte = new QDateTimeEdit(ShiftsItem::getZeroDateTime(), parent);
			dte->setMinimumDateTime(ShiftsItem::getZeroDateTime());
			dte->setDisplayFormat("yyyy-MM-dd HH:mm:ss.zzz");
			dte->setTimeSpec(Qt::UTC);
			return dte;
		}
		else if (
			si->getLevel() == ShiftsItem::Level::FILE_SHIFT &&
			index.column() == ShiftsItem::Columns::SHIFT
			)
		{
			QTimeEdit* te = new QTimeEdit(ShiftsItem::getZeroTime(), parent);
			te->setDisplayFormat("HH:mm:ss.zzz");
			return te;
		}
		else
		{
			return QItemDelegate::createEditor(parent, option, index);
		}
	}

	void setEditorData(QWidget* editor,
		const QModelIndex& index) const
	{
		QVariant value = index.model()->data(index, Qt::EditRole);
		if (value.userType() == QMetaType::QDateTime)
		{
			QDateTimeEdit* dte = static_cast<QDateTimeEdit*>(editor);
			dte->setDateTime(value.toDateTime());
		} 
		else if (value.userType() == QMetaType::QTime)
		{
			QDateTimeEdit* te = static_cast<QTimeEdit*>(editor);
			te->setTime(value.toTime());
		}
		else
			QItemDelegate::setEditorData(editor, index);
	}

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex&/* index */) const override
	{
		editor->setGeometry(option.rect);
	}
private:
};

