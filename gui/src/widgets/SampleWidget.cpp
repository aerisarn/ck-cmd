#include "SampleWidget.h"

using namespace ckcmd;
using namespace ckcmd::HKX;

SampleWidget::SampleWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) :
	ModelWidget(model, parent)
{
	setupUi(this);
}

void SampleWidget::OnIndexSelected()
{

}