#include "ValuesWidget.h"
#include "WidgetFactory.h"

#include <src/models/ProjectTreeModel.h>
#include <hkbNode_1.h>

//#include "ui_HkTopNavigatorWidget.h"
//#include "ui_CharacterEditor.h"

//#include <src/hkx/RefDelegate.h>

using namespace ckcmd::HKX;

ValuesWidget::ValuesWidget(ProjectTreeModel* model, CommandManager& command_manager, const ResourceManager& manager, QWidget* parent) :
	_model(model),
	_command_manager(command_manager),
	_manager(manager),
	ads::CDockWidget("Values",parent)
{

	_mainWidget = new QWidget(this);

	_top_info = new TopInfoWidget(*model, _mainWidget);
	_top_info->setVisible(false);
	_empty_panel = new QPlainTextEdit(_mainWidget);
	_empty_panel->setEnabled(false);

	_mainLayout = new QGridLayout(_mainWidget);
	_mainLayout->setContentsMargins(0, 0, 0, 0);
	_mainLayout->addWidget(_top_info, 0, 0);

	_editorPanel = new QWidget(_mainWidget);
	_editorPanelLayout = new QStackedLayout(_editorPanel);

	_editorPanelLayout->addWidget(_empty_panel);
	QPlainTextEdit* another_editor = new QPlainTextEdit(_mainWidget);
	_editorPanelLayout->addWidget(another_editor);

	_mainLayout->addWidget(_editorPanel, 1, 0);

	setWidget(_mainWidget);
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

void ValuesWidget::checkTopInfoPanel(const QModelIndex& index)
{
	if (_model->isVariant(index))
	{
		_top_info->setVisible(true);
	}
	else {
		_top_info->setVisible(false);
	}
	_top_info->setIndex(index);
}

void ValuesWidget::checkBindings(const QModelIndex& index)
{
}

void ValuesWidget::treeSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{

		checkTopInfoPanel(current);
		checkBindings(current);


	//if (_top_info->isVisible())
	//{
	//	_top_info->setVisible(false);
	//	_editorPanelLayout->setCurrentIndex(1);
	//}
	//else {
	//	_top_info->setVisible(true);
	//	_editorPanelLayout->setCurrentIndex(0);
	//}
	//_top_info->setVisible(true);
	//_empty_panel->setVisible(false);
	//_mainLayout->removeWidget(_empty_panel);
	////QWidget* editor = new QWidget(_mainWidget);
	////Ui::CharacterEditor* editGUI = new Ui::CharacterEditor();
	////editGUI->setupUi(editor);
	////editor->set
	//QPlainTextEdit* another_editor = new QPlainTextEdit(_mainWidget);
	//_mainLayout->addWidget(another_editor, 1, 0);
	//_mainLayout->removeWidget(_empty_panel);
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

