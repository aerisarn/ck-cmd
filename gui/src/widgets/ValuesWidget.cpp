#include "ValuesWidget.h"
#include <src/models/ProjectTreeModel.h>

#include "ui_ValuesWidget.h"

#include <src/hkx/RefDelegate.h>

using namespace ckcmd::HKX;

ValuesWidget::ValuesWidget(ProjectTreeModel* model, CommandManager& command_manager, const ResourceManager& manager, QWidget* parent) :
	_model(model),
	_command_manager(command_manager),
	_manager(manager),
	ui(new Ui::ValuesWidget),
	ads::CDockWidget("Values",parent)
{
	ui->setupUi(this);
	ui->valuesView->setRowHeight(0,1024);
	ui->valuesView->setItemDelegate(new RefDelegate(_manager, this));
	setWidget(ui->verticalLayoutWidget);
	auto table_model = new HkxItemTableModel(command_manager, nullptr, -1, nullptr, this);
	connect(
		table_model, &HkxItemTableModel::HkxItemPointerChanged,
		this, &ValuesWidget::modelHasSetNewHkxItemPointer
	);
	ui->valuesView->setModel(table_model);
	ui->valuesView->setVisible(false);
}

ValuesWidget::~ValuesWidget()
{
	delete ui;
}

#undef max

void ValuesWidget::setIndex(int file_index, QModelIndex index)
{
	_index = index;
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
	}
}

void ValuesWidget::modelHasSetNewHkxItemPointer(HkxItemPointer old_value, HkxItemPointer new_value, int file, hkVariant* variant)
{
	emit HkxItemPointerChanged(_index, _model->getParentIndex(_index), old_value, new_value, file, variant);
}

