#include <src/widgets/SelectNode.h>
#include <src/hkx/HkxVariant.h>
#include <QStringListModel>


using namespace ckcmd;
using namespace ckcmd::HKX;

SelectNode::SelectNode(ProjectModel& model, const QModelIndex& index, QWidget* parent) :
	_manager(model.getResourceManager()),
	ModelDialog(model, index, parent)
{
	setupUi(this);

	auto desidered_superclass = _model.rowType(index);
	_file_index = _model.getFileIndex(index);
	
	_compatible_nodes = _manager.findCompatibleNodes(_file_index, desidered_superclass);
	QStringList compatible_nodes_names;
	for (int i = 0; i < _compatible_nodes.size(); ++i)
	{
		compatible_nodes_names << HkxVariant(*_manager.at(_file_index, _compatible_nodes.at(i).first)).name();
	}
	auto compatible_nodes_model = new QStringListModel(compatible_nodes_names, this);
	_compatible_nodes_proxyModel = new QSortFilterProxyModel(this);
	_compatible_nodes_proxyModel->setSourceModel(compatible_nodes_model);
	referenceExistingTreeView->setModel(_compatible_nodes_proxyModel);
	_compatible_nodes_proxyModel->sort(0);

	QStringList compatible_classes;
	_compatible_classes = _manager.findCompatibleClasses(desidered_superclass);
	for (int i = 0; i < _compatible_classes.size(); ++i)
	{
		compatible_classes << _compatible_classes.at(i)->getName();
	}
	auto compatible_classes_model = new QStringListModel(compatible_classes, this);
	_compatible_classes_proxyModel = new QSortFilterProxyModel(this);
	_compatible_classes_proxyModel->setSourceModel(compatible_classes_model);
	createNewTreeView->setModel(_compatible_classes_proxyModel);
	_compatible_classes_proxyModel->sort(0);

	connect(referenceExistingTreeView->selectionModel(),
		&QItemSelectionModel::currentChanged, this, &SelectNode::referenceExistingTreeViewCurrentChanged);

	connect(createNewTreeView->selectionModel(),
		&QItemSelectionModel::currentChanged, this, &SelectNode::createNewTreeViewCurrentChanged);
}


void SelectNode::referenceExistingTreeViewCurrentChanged(const QModelIndex& selected, const QModelIndex& deselected)
{
	if (selected.isValid())
	{
		int real_row = _compatible_nodes_proxyModel->mapToSource(selected).row();
		_variant = _manager.at(_file_index, _compatible_nodes.at(real_row).first);
		createNewTreeView->selectionModel()->clearSelection();
	}
	else {
		_variant = nullptr;
	}
}

void SelectNode::createNewTreeViewCurrentChanged(const QModelIndex& selected, const QModelIndex& deselected)
{
	if (selected.isValid())
	{
		int real_row = _compatible_classes_proxyModel->mapToSource(selected).row();
		_class = _compatible_classes.at(real_row);
		referenceExistingTreeView->selectionModel()->clearSelection();
		_variant = nullptr;
	}
	else {
		_class = nullptr;
	}
}

std::pair<std::pair<const hkClass*, QString>, hkVariant*>  SelectNode::getNode(
	ProjectModel& model,
	const QModelIndex& index,
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<SelectNode> dialog = 
		std::make_unique<SelectNode>(model, index, parent);
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

