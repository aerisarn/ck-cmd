#include "EventEditor.h"

#include <hkbEventInfo_0.h>

using namespace ckcmd::HKX;

EventEditor::EventEditor(ckcmd::HKX::ProjectModel& model, QWidget* parent)
	: ModelWidget(model, parent)
{
	setupUi(this);
}

void EventEditor::on_SilentCheckBox_stateChanged(int state)
{
	auto index = _model.index(1, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbEventInfo::FLAG_SILENT;
	else
		value &= ~hkbEventInfo::FLAG_SILENT;
	_model.setData(index, value);
}

void EventEditor::on_SyncPointCheckBox_stateChanged(int state)
{
	auto index = _model.index(1, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbEventInfo::FLAG_SYNC_POINT;
	else
		value &= ~hkbEventInfo::FLAG_SYNC_POINT;
	_model.setData(index, value);
}

void EventEditor::OnIndexSelected()
{
	auto value = _model.data(_model.index(1, 1, _index)).value<int>();
	if (value & hkbEventInfo::FLAG_SILENT)
	{
		SilentCheckBox->setChecked(true);
	}
	else
	{
		SilentCheckBox->setChecked(false);
	}
	if (value & hkbEventInfo::FLAG_SYNC_POINT)
	{
		SyncPointCheckBox->setChecked(true);
	}
	else
	{
		SyncPointCheckBox->setChecked(false);
	}
}