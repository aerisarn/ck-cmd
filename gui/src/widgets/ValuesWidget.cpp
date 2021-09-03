#include "ValuesWidget.h"

#include "ui_ValuesWidget.h"

#include <src/hkx/RefDelegate.h>

using namespace ckcmd::HKX;

ValuesWidget::ValuesWidget(const ckcmd::HKX::ResourceManager& manager, QWidget* parent) :
	_manager(manager),
	ui(new Ui::ValuesWidget),
	ads::CDockWidget("Values",parent)
{
	ui->setupUi(this);
	ui->valuesView->setRowHeight(0,1024);
	ui->valuesView->setItemDelegate(new RefDelegate(_manager, this));
	setWidget(ui->verticalLayoutWidget);
}

ValuesWidget::~ValuesWidget()
{
	delete ui;
}

#undef max

void ValuesWidget::setVariant(int file_index, hkVariant* v, hkVariant* parent)
{
	auto actual_model = dynamic_cast<HkxItemTableModel*>(ui->valuesView->model());
	if (actual_model)
	{
		disconnect(
			actual_model, &HkxItemTableModel::HkxItemPointerChanged,
			this, &ValuesWidget::modelHasSetNewHkxItemPointer
		);
	}
	auto new_model = new HkxItemTableModel(v, file_index, parent, this);
	ISpecialFieldsHandler* fields_handler = _manager.fieldsHandler(file_index);
	if (fields_handler != NULL)
	{
		std::vector<member_id_t> hand = fields_handler->getHandledFields();
		new_model->registerFieldHandler(fields_handler);
	}
	connect(
		new_model, &HkxItemTableModel::HkxItemPointerChanged,
		this, &ValuesWidget::modelHasSetNewHkxItemPointer
	);
	ui->valuesView->setModel(new_model);
	ui->valuesView->setVisible(false);
	QRect vporig = ui->valuesView->viewport()->geometry();
	QRect vpnew = vporig;
	vpnew.setWidth(std::numeric_limits<int>::max());
	ui->valuesView->viewport()->setGeometry(vpnew);
	ui->valuesView->resizeColumnsToContents();
	ui->valuesView->resizeRowsToContents();
	ui->valuesView->viewport()->setGeometry(vporig);
	ui->valuesView->setVisible(true);
	//ui->valuesView->resizeColumnsToContents();
	if (NULL != actual_model)
		delete actual_model;
}

void ValuesWidget::modelHasSetNewHkxItemPointer(HkxItemPointer old_value, HkxItemPointer new_value, int file, hkVariant* variant)
{
	emit HkxItemPointerChanged(old_value, new_value, file, variant);
}

