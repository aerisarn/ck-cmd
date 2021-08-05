#include "ValuesWidget.h"

#include "ui_ValuesWidget.h"

#include <src/hkx/RefDelegate.h>

using namespace ckcmd::HKX;

ValuesWidget::ValuesWidget(QWidget* parent) :
	ui(new Ui::ValuesWidget),
	ads::CDockWidget("Values",parent)
{
	ui->setupUi(this);
	ui->valuesView->setRowHeight(0,1024);
	ui->valuesView->setItemDelegate(new RefDelegate(this));
	setWidget(ui->verticalLayoutWidget);
}

ValuesWidget::~ValuesWidget()
{
	delete ui;
}

#undef max

void ValuesWidget::setVariant(hkVariant* v)
{
	auto actual_model = ui->valuesView->model();
	ui->valuesView->setModel(
		new HkxItemTableModel(v, this)
	);
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

