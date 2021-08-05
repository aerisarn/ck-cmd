#include "ProjectsWidget.h"

#include "ui_ProjectsWidget.h"

#include <src/hkx/ProjectBuilder.h>

using namespace ckcmd::HKX;

ProjectsWidget::ProjectsWidget(ProjectTreeModel* model, ResourceManager* manager, QWidget* parent) :
	_model(model),
	_manager(manager),
	ads::CDockWidget("Projects", parent),
	ui(new Ui::ProjectsWidget)
{
	ui->setupUi(this);
	setWidget(ui->verticalLayoutWidget);
	ui->treeView->setModel(model);

	connect(ui->treeView, &QTreeView::doubleClicked, this, &ProjectsWidget::nodeDoubleClicked);
	connect(ui->treeView, &QTreeView::clicked, this, &ProjectsWidget::nodeClicked);
}

void ProjectsWidget::nodeDoubleClicked(const QModelIndex& index)
{
	ProjectNode* node_clicked = _model->getNode(index);
	if (node_clicked->isProjectRoot() && node_clicked->childCount() == 0) {
		_model->beginModify(index, 0, 2);
		ProjectBuilder b(
			node_clicked,
			*_manager,
			node_clicked->data(0).toString().toUtf8().constData()
		);
		_model->endModify(index);
	}

}

void ProjectsWidget::nodeClicked(const QModelIndex& index)
{
	ProjectNode* node_clicked = _model->getNode(index);
	if (node_clicked->isVariant()) {
		emit variantSelected(
			node_clicked->data(2).value<int>(), 
			(hkVariant*)node_clicked->data(1).toULongLong()
		);
	}
}

ProjectsWidget::~ProjectsWidget()
{
	delete ui;
}

