#include <src/widgets/TransitionCreator.h>

#include <QSortFilterProxyModel>

using namespace ckcmd::HKX;

TransitionCreator::TransitionCreator(ProjectModel& model, QModelIndex index, QWidget* parent) :
	ModelDialog(model, index, parent)
{
	setupUi(this);

	auto nodetype = _model.nodeType(index);
	if (nodetype == NodeType::FSMWildcardTransitions)
	{
		fromStateLabel->setVisible(false);
		fromStateTableView->setVisible(false);
		_result.fromStateId = -1;
	}
	else {
		auto from_state_model = model.editModel(index.parent(), AssetType::FSM_states, "", Qt::DisplayRole);
		fromStateTableView->setModel(from_state_model);
		fromStateTableView->resizeRowsToContents();
	}

	auto to_state_model = model.editModel(index.parent(), AssetType::FSM_states, "", Qt::DisplayRole);
	auto event_model = model.editModel(index, AssetType::events, "", Qt::DisplayRole);
	auto proxy_model = new QSortFilterProxyModel(this);  proxy_model->setSourceModel(event_model);
	proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
	connect(eventLineEdit, &QLineEdit::textChanged, proxy_model, &QSortFilterProxyModel::setFilterFixedString);

	toStateTableView->setModel(to_state_model);
	toStateTableView->resizeRowsToContents();
	onEventTableView->setModel(proxy_model);
}

void TransitionCreator::accept()
{
	if (fromStateTableView->isVisible() && fromStateTableView->currentIndex().isValid())
	{
		_result.fromStateId = fromStateTableView->currentIndex().row();
	}
	if (toStateTableView->currentIndex().isValid())
	{
		_result.toStateId = toStateTableView->currentIndex().row();
	}
	if (onEventTableView->currentIndex().isValid())
	{
		_result.eventIndex = onEventTableView->currentIndex().row();
	}
	QDialog::accept();
}

NewTransition TransitionCreator::getTransition(
	ckcmd::HKX::ProjectModel& model, QModelIndex index,
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<TransitionCreator> dialog = std::make_unique<TransitionCreator>(model, index, parent);
	const int ret = dialog->exec();
	if (ok)
		*ok = !!ret;
	if (ret) {
		return dialog->selected();
	}
	else {
		return {};
	}
}