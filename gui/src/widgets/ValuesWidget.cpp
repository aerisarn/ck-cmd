#include "ValuesWidget.h"
#include "WidgetFactory.h"

#include <src/models/ProjectTreeModel.h>
#include <hkbNode_1.h>

using namespace ckcmd;
using namespace ckcmd::HKX;

ValuesWidget::ValuesWidget(ProjectModel* model, CommandManager& command_manager, const ResourceManager& manager, QWidget* parent) :
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
	ckcmd::WidgetFactory::loadAddWidgets(_editorPanelLayout, *_model);

	_mainLayout->addWidget(_editorPanel, 1, 0);

	setWidget(_mainWidget);
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

void ValuesWidget::setEditor(const QModelIndex& index)
{
	widgetType type = widgetType::invalid;
	switch (_model->nodeType(index))
	{
	case NodeType::CharacterHkxNode:
		type = widgetType::CharacterEditor;
		break;
	case NodeType::HavokNative:
		type = widgetType::GenericWidget;
		break;
	case NodeType::behaviorEventName:
		type = widgetType::EventEditor;
		break;
	default:
		break;
	}
	if (type != widgetType::invalid)
	{
		auto selected_widget = static_cast<ModelWidget*>(_editorPanelLayout->widget(*type));
		selected_widget->setIndex(index);
	}
	_editorPanelLayout->setCurrentIndex(*type);
}

void ValuesWidget::treeSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{

	checkTopInfoPanel(current);
	checkBindings(current);
	setEditor(current);
}
