#include "WidgetFactory.h"

#include "CharacterEditor.h"
#include "SampleWidget.h"

using namespace ckcmd;
using namespace ckcmd::HKX;

QWidget* WidgetFactory::getWidget(widgetType type, HKX::ProjectModel& model, QWidget* parent)
{
	switch (type)
	{
	case widgetType::CharacterEditor:
		return new CharacterEditorWidget(model, parent);
	case widgetType::SampleWidget:
		return new SampleWidget(model, parent);
	default:
		break;
	}
	//unsupported
	return nullptr;
}

void WidgetFactory::loadAddWidgets(QStackedLayout* stack, HKX::ProjectModel& model, QWidget* parent)
{
	for (int i = *widgetType::invalid + 1;
		i < *widgetType::allEditors;
		++i)
	{
		stack->addWidget(getWidget(static_cast<widgetType>(i), model, parent));
	}
}