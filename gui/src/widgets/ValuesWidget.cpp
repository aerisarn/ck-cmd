#include "ValuesWidget.h"
#include <src/models/ProjectTreeModel.h>

#include "ui_HkTopNavigatorWidget.h"

#include <src/hkx/RefDelegate.h>

using namespace ckcmd::HKX;

ValuesWidget::ValuesWidget(ProjectTreeModel* model, CommandManager& command_manager, const ResourceManager& manager, QWidget* parent) :
	_model(model),
	_command_manager(command_manager),
	_manager(manager),
	ads::CDockWidget("Values",parent)
{

	QWidget* main = new QWidget(this);

	_top_info = new TopInfoWidget(main);
	_top_info->setVisible(false);
	_empty_panel = new QPlainTextEdit(main);
	_empty_panel->setEnabled(false);

	_mainLayout = new QGridLayout(main);
	_mainLayout->setContentsMargins(0, 0, 0, 0);
	_mainLayout->addWidget(_top_info, 0, 0);
	_mainLayout->addWidget(_empty_panel, 1, 0);

	setWidget(main);
	//ui->verticalLayout->addWidget(w);

	//ui->valuesView->setRowHeight(0,1024);
	//ui->valuesView->setItemDelegate(new RefDelegate(_manager, this));
	//setWidget(ui->verticalLayoutWidget);
	//auto table_model = new HkxItemTableModel(command_manager, nullptr, -1, nullptr, this);
	//connect(
	//	table_model, &HkxItemTableModel::HkxItemPointerChanged,
	//	this, &ValuesWidget::modelHasSetNewHkxItemPointer
	//);
	//ui->valuesView->setModel(table_model);
	//ui->valuesView->setVisible(false);
}

ValuesWidget::~ValuesWidget()
{
}

void ValuesWidget::treeSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	_top_info->setVisible(true);
}

//void ValuesWidget::setIndex(int file_index, QModelIndex index)
//{
	/*_index = index;
	ProjectNode* node = _model->getNode(index);

	if (node->isVariant())
	{
		auto actual_model = dynamic_cast<HkxItemTableModel*>(ui->valuesView->model());
		ISpecialFieldsHandler* fields_handler = _manager.fieldsHandler(file_index);
		if (fields_handler != NULL)
		{
			actual_model->registerFieldHandler(fields_handler);
		}
		actual_model->setVariant(file_index, node->variant());
		QRect vporig = ui->valuesView->viewport()->geometry();
		QRect vpnew = vporig;
		vpnew.setWidth(std::numeric_limits<int>::max());
		ui->valuesView->viewport()->setGeometry(vpnew);
		ui->valuesView->resizeColumnsToContents();
		ui->valuesView->resizeRowsToContents();
		ui->valuesView->viewport()->setGeometry(vporig);
		ui->valuesView->setVisible(true);
	}*/
//}
//
//void ValuesWidget::modelHasSetNewHkxItemPointer(HkxItemPointer old_value, HkxItemPointer new_value, int file, hkVariant* variant)
//{
//	//emit HkxItemPointerChanged(_index, _model->getParentIndex(_index), old_value, new_value, file, variant);
//}

