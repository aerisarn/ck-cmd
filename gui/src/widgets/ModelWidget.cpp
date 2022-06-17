#include "ModelWidget.h"

using namespace ckcmd;
using namespace ckcmd::HKX;

ModelWidget::ModelWidget(ProjectModel& model, QWidget* parent) :
	_model(model),
	QWidget(parent)
{
}

void ModelWidget::setIndex(const QModelIndex& index) 
{ 
	_index = index; 
	OnIndexSelected(); 
}