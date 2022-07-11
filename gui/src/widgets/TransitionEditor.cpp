#include <src/widgets/TransitionEditor.h>

#include <src/models/ValuesProxyModel.h>
#include <src/hkx/ItemsDelegate.h>
#include <src/items/HkxItemPointer.h>

#include <hkbStateMachineTransitionInfo_1.h>

using namespace ckcmd::HKX;

TransitionEditorWidget::TransitionEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) :
	ModelWidget(model, parent)
{
	setupUi(this);

	dataBindingtable.push_back({ triggerWindowBeginSpinBox, {}, "triggerInterval.enterTime", 0 });
	dataBindingtable.push_back({ triggerWindowEndSpinBox, {}, "triggerInterval.exitTime", 0 });
	dataBindingtable.push_back({ initiateWindowBeginSpinBox, {}, "initiateInterval.enterTime", 0 });
	dataBindingtable.push_back({ initiateWindowEndSpinBox, {}, "initiateInterval.exitTime", 0 });
}

void TransitionEditorWidget::OnIndexSelected()
{
	if (_members.find("fromStateId") != _members.end())
	{
		fromStateTableView->setEnabled(true);
		fromNestedStateTableView->setEnabled(true);
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["fromStateId"].first }, 1, _index, this);
		fromStateTableView->setModel(editModel);
		fromStateTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	else {
		fromStateTableView->setEnabled(false);
		fromNestedStateTableView->setEnabled(false);
	}
	if (fromNestedStateTableView->isEnabled() && _members.find("fromNestedStateId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["fromNestedStateId"].first }, 1, _index, this);
		fromNestedStateTableView->setModel(editModel);
		fromNestedStateTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("toStateId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["toStateId"].first }, 1, _index, this);
		toStateTableView->setModel(editModel);
		toStateTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("toNestedStateId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["toNestedStateId"].first }, 1, _index, this);
		toNestedStateTableView->setModel(editModel);
		toNestedStateTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("eventId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["eventId"].first }, 1, _index, this);
		eventTableView->setModel(editModel);
		eventTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("condition") != _members.end())
	{
		auto row_index = _model.index(_members["condition"].first, 1, _index);
		auto data = row_index.data();
		if (data.canConvert<HkxItemPointer>())
		{
			auto ptr = data.value<HkxItemPointer>();
			if (ptr.get() == nullptr)
			{
				conditionTableView->setEnabled(false);
			}
			else {
				conditionTableView->setEnabled(true);
			}
		}
		else {
			conditionTableView->setEnabled(true);
		}
		if (conditionTableView->isEnabled())
		{
			ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["condition"].first }, 1, _index, this);
			conditionTableView->setModel(editModel);
			conditionTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
		}
	}
	if (_members.find("transition") != _members.end())
	{
		auto row_index = _model.index(_members["transition"].first, 1, _index);
		auto data = row_index.data();
		if (data.canConvert<HkxItemPointer>())
		{
			auto ptr = data.value<HkxItemPointer>();
			if (ptr.get() == nullptr)
			{
				effectTableView->setEnabled(false);
			}
			else {
				effectTableView->setEnabled(true);
			}
		}
		else {
			effectTableView->setEnabled(true);
		}
		if (effectTableView->isEnabled())
		{
			ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["transition"].first }, 1, _index, this);
			effectTableView->setModel(editModel);
			effectTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
		}
	}
	if (_members.find("flags") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["flags"].first }, 1, _index, this);
		flagsTableView->setModel(editModel);
		flagsTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
		flagsTableView->resizeRowsToContents();
	}
}