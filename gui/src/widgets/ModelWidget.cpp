#include "ModelWidget.h"

using namespace ckcmd;
using namespace ckcmd::HKX;

ModelWidget::ModelWidget(ProjectTreeModel& model, QWidget* parent) :
	_model(model),
	QWidget(parent)
{
}