#include "ProjectsWidget.h"

#include "ui_ProjectsWidget.h"

#include <src/hkx/ProjectBuilder.h>
#include <src/widgets/TreeContextMenuBuilder.h>

using namespace ckcmd::HKX;

ProjectsWidget::ProjectsWidget(
	ProjectTreeModel* model,
	CommandManager& command_manager,
	ResourceManager& manager, 
	HkxSimulation* simulation,
	QWidget* parent
) :
	_model(model),
	_commandManager(command_manager),
	_manager(manager),
	_simulation(simulation),
	ads::CDockWidget("Projects", parent),
	ui(new Ui::ProjectsWidget)
{
	ui->setupUi(this);
	setWidget(ui->verticalLayoutWidget);
	ui->treeView->setModel(model);
	ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->treeView, &QTreeView::doubleClicked, this, &ProjectsWidget::nodeDoubleClicked);
	connect(ui->treeView, &QTreeView::clicked, this, &ProjectsWidget::nodeClicked);
	connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeMenu(QPoint)));
}

void ProjectsWidget::nodeDoubleClicked(const QModelIndex& index)
{
	ProjectNode* node_clicked = _model->getNode(index);
	if (node_clicked->isProjectRoot() && node_clicked->childCount() == 0) {
		_model->notifyBeginInsertRows(index, 0, 2);
		ProjectBuilder b(
			node_clicked,
			_commandManager,
			_manager,
			node_clicked->data(0).toString().toUtf8().constData()
		);
		_model->notifyEndInsertRows();
	}
	else if (node_clicked->isSkeleton()) {
		
		_simulation->addSkeleton(
			node_clicked->data(1).toString().toUtf8().constData()
		);
	}

}

void ProjectsWidget::modelHasSetNewHkxItemPointer(
	ckcmd::HKX::HkxItemPointer old_value, 
	ckcmd::HKX::HkxItemPointer new_value, 
	int file, 
	hkVariant* variant)
{
	if (old_value.get() == new_value.get())
		return;

	QModelIndex parent_index;
	size_t row = 0;
	ProjectNode* old_parent = nullptr;

	if (old_value.get() != nullptr)
	{
		auto old_index = _manager.findIndex(file, old_value.get());
		auto old_variant = _manager.at(file, old_index);
		ProjectNode* old_node = _manager.findNode(file, old_variant);
		row = _model->getIndex(old_node).row();

		old_parent = old_node->parentItem();
		parent_index = _model->getIndex(old_parent);
		_model->notifyBeginRemoveRows(parent_index, row, row);
		old_parent->removeChild(row);
		old_node->removeParent(old_parent);
		_model->notifyEndRemoveRows();
	}

	if (new_value.get() != nullptr)
	{
		if (!parent_index.isValid())
		{
			old_parent = _manager.findNode(file, variant);
			parent_index = _model->getIndex(old_parent);
			row = old_parent->childCount();
		}

		auto new_index = _manager.findIndex(file, new_value.get());
		auto new_variant = _manager.at(file, new_index);
		ProjectNode* new_node = _manager.findNode(file, new_variant);

		_model->notifyBeginInsertRows(parent_index, row, row);
		old_parent->insertChild(row, new_node);
		new_node->setParent(old_parent);
		_model->notifyEndInsertRows();

	}
	auto clicked = _model->index(row, 0, parent_index);
	ui->treeView->setCurrentIndex(clicked);
	nodeClicked(clicked);
}

void ProjectsWidget::nodeClicked(const QModelIndex& index)
{
	ProjectNode* node_clicked = _model->getNode(index);
	if (node_clicked->isVariant()) {
		emit variantSelected(
			node_clicked->data(3).value<int>(), 
			(hkVariant*)node_clicked->data(1).toULongLong(),
			(hkVariant*)node_clicked->data(2).toULongLong()
		);
	}
}

void ProjectsWidget::treeMenu(QPoint p)
{
	const QModelIndex& index = ui->treeView->indexAt(p);
	ProjectNode* node_clicked = _model->getNode(index);
	/*QMenu* menu = TreeContextMenuBuilder().build(node_clicked);
	if (menu != nullptr) {
		menu->exec(this->mapToGlobal(p));
	}*/
}

ProjectsWidget::~ProjectsWidget()
{
	delete ui;
}

QTreeView& ProjectsWidget::view()
{
	return *ui->treeView;
}

