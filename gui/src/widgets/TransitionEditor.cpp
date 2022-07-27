#include <src/widgets/TransitionEditor.h>

#include <src/models/ValuesProxyModel.h>
#include <src/hkx/ItemsDelegate.h>
#include <src/items/HkxItemPointer.h>
#include <src/widgets/SelectNode.h>

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
	auto row_name_index = _model.index(0, 0, _index);
	int columns = _model.columnCount(row_name_index);
	if (columns == 2)
	{
		fromStateLineEdit->setEnabled(true);
		fromNestedStateTableView->setEnabled(true);
		fromStateLineEdit->setText(_model.index(0, 1, _index).data().toString());
	}
	else {
		fromStateLineEdit->setEnabled(false);
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
		else {
			conditionTableView->setModel(nullptr);
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
		else {
			effectTableView->setModel(nullptr);
		}
	}
	if (_members.find("flags") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["flags"].first }, 1, _index, this);
		flagsTableView->setModel(editModel);
		flagsTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
		flagsTableView->resizeRowsToContents();
	}
	if (_members.find("initiateInterval.enterEventId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["initiateInterval.enterEventId"].first }, 1, _index, this);
		initiateWindowEnterEventTableView->setModel(editModel);
		initiateWindowEnterEventTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("initiateInterval.exitEventId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["initiateInterval.exitEventId"].first }, 1, _index, this);
		initiateWindowExitEventTableView->setModel(editModel);
		initiateWindowExitEventTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("triggerInterval.enterEventId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["triggerInterval.enterEventId"].first }, 1, _index, this);
		triggerWindowEnterEventTableView->setModel(editModel);
		triggerWindowEnterEventTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
	if (_members.find("triggerInterval.exitEventId") != _members.end())
	{
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, { _members["triggerInterval.exitEventId"].first }, 1, _index, this);
		triggerWindowExitEventTableView->setModel(editModel);
		triggerWindowExitEventTableView->setItemDelegate(new ItemsDelegate(*editModel, this));
	}
}

void TransitionEditorWidget::addOrSet(const char* member_name)
{
	auto binding = _members.at(member_name);
	auto index = _model.index(binding.first, 0, _index);
	//row index to be set or container to be modified;
	bool ok;
	auto selection = SelectNode::getNode(_model, index, nullptr, &ok);
	if (ok)
	{
		void* to_set_or_add = selection.second == nullptr ? nullptr : selection.second->m_object;
		if (to_set_or_add == nullptr)
		{
			std::string name = selection.first.second.toUtf8().constData();
			to_set_or_add = _model.getResourceManager().createObject(_model.getFileIndex(index), selection.first.first, name);
		}
		HkxItemPointer p(to_set_or_add);
		QVariant value; value.setValue(p);
		bool isArray = _model.isArray(index);
		if (isArray)
		{
			int columns = _model.columnCount(index);
			bool result = _model.insertColumns(index.row(), columns, 1, index);
			QModelIndex new_index = _model.index(index.row(), columns, index);
			_model.setData(new_index, value);
		}
		else {
			QModelIndex new_index = _model.index(index.row(), 1, index);
			_model.setData(new_index, value);
		}
	}
}

void TransitionEditorWidget::on_effectToolButton_clicked()
{
	addOrSet("transition");
}

void TransitionEditorWidget::on_conditionToolButton_clicked()
{
	addOrSet("condition");
}