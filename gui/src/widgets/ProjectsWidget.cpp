#include "ProjectsWidget.h"

#include "ui_ProjectsWidget.h"

#include <src/hkx/ProjectBuilder.h>

using namespace ckcmd::HKX;

ProjectsWidget::ProjectsWidget(
	ProjectTreeModel* model, 
	ResourceManager* manager, 
	HkxSimulation* simulation,
	QWidget* parent
) :
	_model(model),
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
			*_manager,
			node_clicked->data(0).toString().toUtf8().constData()
		);
		_model->notifyEndInsertRows(index);
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
	//auto old_index = _manager->findIndex(file, old_value.get());
	//auto new_index = _manager->findIndex(file, new_value.get());
	//auto old_variant = _manager->at(file, old_index);
	//auto new_variant = _manager->at(file, new_index);
	//ProjectNode* old_node = _manager->findNode(file, old_variant);
	//ProjectNode* new_node = _manager->findNode(file, new_variant);
	//auto old_model_index = _model->getIndex(old_node);
	//auto new_model_index = _model->getIndex(new_node);
	//_model->notifyBeginMoveRows(old_model_index, old_model_index.row(), old_model_index.row()+1, new_model_index, new_model_index.row());
	//old_node->parentItem()->setChild(new_model_index.row(), new_node);
	//// TODO: relink
	//_model->notifyEndMoveRows();
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
}

ProjectsWidget::~ProjectsWidget()
{
	delete ui;
}

