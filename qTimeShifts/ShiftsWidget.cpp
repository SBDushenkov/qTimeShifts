#include "ShiftsWidget.h"

#include "ShiftsDelegate.h"
#include "ShiftsSerializer.h"
ShiftsWidget::ShiftsWidget(QWidget* parent) : QWidget(parent), result(Result::SAVE)
{
	setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMinMaxButtonsHint);
	ui.setupUi(this);
	createConnections();
}

int ShiftsWidget::createConnections()
{
	connect(ui.addButton, &QPushButton::pressed, this, &ShiftsWidget::add);
	connect(ui.removeButton, &QPushButton::pressed, this, &ShiftsWidget::remove);
	connect(ui.applyButton, &QPushButton::pressed, this, &ShiftsWidget::apply);
	connect(ui.rejectButton, &QPushButton::pressed, this, &ShiftsWidget::reject);
	return 0;
}


void ShiftsWidget::setModel(std::shared_ptr<ShiftsItem> root)
{
	rootItem = root;
	model = new ShiftsModel(rootItem, this);
	setModelToProxy();
}

int ShiftsWidget::getResult()
{
	return result;
}

void ShiftsWidget::setModelToProxy()
{
	if (ui.shiftsTreeView->selectionModel())
		disconnect(ui.shiftsTreeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ShiftsWidget::rowChanged);
	proxy = new ShiftsProxy(this);
	proxy->setSourceModel(model);
	ui.shiftsTreeView->setModel(proxy);
	ui.shiftsTreeView->setItemDelegate(new ShiftsDelegate(ui.shiftsTreeView));
	connect(ui.shiftsTreeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ShiftsWidget::rowChanged);

	ui.shiftsTreeView->expandAll();
	for (int c = 0; c < model->columnCount(QModelIndex()); ++c)
		ui.shiftsTreeView->resizeColumnToContents(c);

	QModelIndex sourceIndex = proxy->mapFromSource(model->firstIndex());
	ui.shiftsTreeView->setCurrentIndex(sourceIndex);
	rowChanged(sourceIndex, QModelIndex());
	
	proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxy->sort(0);
}

ShiftsWidget::~ShiftsWidget()
{
	//model->deleteLater();
}

void ShiftsWidget::add()
{
	const QModelIndex& proxyIndex = ui.shiftsTreeView->selectionModel()->currentIndex();
	const QModelIndex& localIndex = proxy->mapToSource(proxyIndex);
	if (!proxyIndex.isValid())
		return;

	QModelIndex emptyElement = model->getEmptyNeighbors(localIndex);
	if (emptyElement.isValid())
	{
		ui.shiftsTreeView->setCurrentIndex(proxy->mapFromSource(emptyElement));
	}
	else
	{
		proxy->setDynamicSortFilter(false);
		proxy->insertRow(proxyIndex.row() + 1, proxyIndex.parent());
		const QModelIndex& newProxyIndex = proxy->index(proxyIndex.row() + 1, 0, proxyIndex.parent());
		const QModelIndex& newLocalIndex = proxy->mapToSource(newProxyIndex);
		proxy->setDynamicSortFilter(true);
		ui.shiftsTreeView->setCurrentIndex(
			proxy->mapFromSource(newLocalIndex)
		);
		ui.shiftsTreeView->expand(proxy->mapFromSource(newLocalIndex));
	}
	ui.shiftsTreeView->setFocus();
}

void ShiftsWidget::remove()
{
	const QModelIndex proxyIndex = ui.shiftsTreeView->selectionModel()->currentIndex();
	if (proxyIndex.isValid())
	{
		QAbstractItemModel* model = ui.shiftsTreeView->model();
		proxy->removeRow(proxyIndex.row(), proxyIndex.parent());
	}
}

void ShiftsWidget::reject()
{
	result = Result::REJECT;
	close();
}

void ShiftsWidget::apply()
{
	result = Result::SAVE;
	close();
}

void ShiftsWidget::rowChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (current.isValid())
	{
		if (model->canChange(current))
			ui.removeButton->setEnabled(true);
		else
			ui.removeButton->setEnabled(false);
	}
	else
	{
		ui.removeButton->setEnabled(false);
		ui.addButton->setEnabled(false);
	}
}