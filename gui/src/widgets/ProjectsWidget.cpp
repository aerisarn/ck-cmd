#include "ProjectsWidget.h"

#include <src/widgets/TreeContextMenuBuilder.h>

using namespace ckcmd::HKX;

ProjectsWidget::ProjectsWidget(
	ProjectTreeModel* model,
	CommandManager& command_manager,
	ResourceManager& manager,
	ActionHandler& actionsHandler,
	HkxSimulation* simulation,
	QWidget* parent
) :
	_model(model),
	_commandManager(command_manager),
	_manager(manager),
	_simulation(simulation),
	_menuBuilder(actionsHandler),
	ads::CDockWidget("Projects", parent)
{
	setupUi(this);
	setWidget(verticalLayoutWidget);
	workspaceLabel->setText(QCoreApplication::translate("ProjectsWidget", "Workspace:", nullptr) + " " + _manager.workspace().getFolder().string().c_str());

	treeView->setHeaderHidden(true);
	treeView->setModel(model);
	treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ProjectsWidget::on_treeView_selectionChanged);
	connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeMenu(QPoint)));
}

//void ProjectsWidget::nodeDoubleClicked(const QModelIndex& index)
void ProjectsWidget::on_treeView_doubleClicked(const QModelIndex& index)
{
	_model->activate(index);
	//else if (node_clicked->isSkeleton()) {
	//	
	//	_simulation->addSkeleton(
	//		node_clicked->data(1).toString().toUtf8().constData()
	//	);
	//}

}

void ProjectsWidget::modelHasSetNewHkxItemPointer(
	const QModelIndex& parent,
	const QModelIndex& index,
	ckcmd::HKX::HkxItemPointer old_value,
	ckcmd::HKX::HkxItemPointer new_value,
	int file,
	hkVariant* variant
)
{
	//if (old_value.get() == new_value.get())
	//	return;

	//size_t row = 0;
	//ProjectNode* old_parent = nullptr;

	//if (old_value.get() != nullptr)
	//{
	//	auto old_index = _manager.findIndex(file, old_value.get());
	//	auto old_variant = _manager.at(file, old_index);
	//	ProjectNode* old_node = _manager.findNode(file, old_variant);
	//	row = index.row();

	//	old_parent = old_node->parentItem();
	//	_model->notifyBeginRemoveRows(parent, row, row);
	//	old_parent->removeChild(row);
	//	old_node->removeParent(old_parent);
	//	_model->notifyEndRemoveRows();
	//}

	//if (new_value.get() != nullptr)
	//{
	//	if (!parent.isValid())
	//	{
	//		old_parent = _manager.findNode(file, variant);
	//		row = old_parent->childCount();
	//	}

	//	auto new_index = _manager.findIndex(file, new_value.get());
	//	auto new_variant = _manager.at(file, new_index);
	//	ProjectNode* new_node = _manager.findNode(file, new_variant);

	//	_model->notifyBeginInsertRows(parent, row, row);
	//	old_parent->insertChild(row, new_node);
	//	new_node->setParent(old_parent);
	//	_model->notifyEndInsertRows();

	//}
	//auto clicked = _model->index(row, 0, parent);
	//ui->treeView->setCurrentIndex(clicked);
	//nodeClicked(clicked);
}

void ProjectsWidget::nodeClicked(const QModelIndex& index)
{
	//ProjectNode* node_clicked = _model->getNode(index);
	//if (node_clicked->isVariant()) {
	//	emit variantSelected(
	//		node_clicked->data(3).value<int>(), 
	//		index
	//	);
	//}
}

//void ProjectsWidget::treeMenu(QPoint p)
void ProjectsWidget::on_treeView_customContextMenuRequested(const QPoint& pos)
{
	const QModelIndex& index = treeView->indexAt(pos);
	QMenu* menu = _menuBuilder.build(_model->nodeType(index));
	if (menu != nullptr) {
		menu->exec(this->mapToGlobal(pos));
	}
	//ProjectNode* node_clicked = _model->getNode(index);

}

void ProjectsWidget::on_treeView_selectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	emit selectionChanged(current, previous);
}

ProjectsWidget::~ProjectsWidget()
{
}

QTreeView& ProjectsWidget::view()
{
	return *treeView;
}

