#pragma once
#include <qwidget.h>

#include "ui_ShiftsWidget.h"
#include "ShiftsItem.h"
#include "ShiftsModel.h"
#include "ShiftsProxy.h"
#include "ShiftsSerializer.h"

class ShiftsWidget :
	public QWidget
{
	Q_OBJECT
public:
	ShiftsWidget(QWidget* parent = Q_NULLPTR);
	~ShiftsWidget();
	void setModel(std::shared_ptr<ShiftsItem> root);

	enum Result
	{
		SAVE,
		REJECT
	};
	int getResult();

private:
	void setModelToProxy();
	int ShiftsWidget::createConnections();

	ShiftsProxy* proxy;
	Ui::ShiftsWidget ui;
	ShiftsModel* model;

	int result;
	std::shared_ptr<ShiftsItem> rootItem;

private slots:
	void add();
	void remove();
	void apply();
	void reject();
	void rowChanged(const QModelIndex& current, const QModelIndex& previous);

};

