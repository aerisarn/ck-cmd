#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPlainTextEdit>


MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	_workspace(Settings.get<std::string>("/general/workspace_folder")),
	_logger(new ckcmd::GUI::LogControl()),
	_log_setter(_logger),
	_animation_manager(_workspace),
	_resource_manager(_workspace),
	_model(this),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
	Layout->setContentsMargins(QMargins(0, 0, 0, 0));
	m_DockManager = new ads::CDockManager(ui->dockContainer);
	Layout->addWidget(m_DockManager);

	ads::CDockWidget* DockWidget = new ads::CDockWidget("Logger", this);
	DockWidget->setWidget(_logger);
	

	_model._rootNode = ckcmd::HKX::ProjectNode::createSupport({ _workspace.c_str() }, NULL);
	_animation_manager.buildProjectTree(_model._rootNode);


	_projectTreeView = new ProjectsWidget(&_model, &_resource_manager, this);
	_valuesTableView = new ValuesWidget(this);

	connect(_projectTreeView, &ProjectsWidget::variantSelected, _valuesTableView, &ValuesWidget::setVariant);

	ui->menuView->addAction(DockWidget->toggleViewAction());
	ui->menuView->addAction(_projectTreeView->toggleViewAction());
	ui->menuView->addAction(_valuesTableView->toggleViewAction());
	m_DockManager->addDockWidget(ads::CenterDockWidgetArea, _valuesTableView);
	m_DockManager->addDockWidget(ads::LeftDockWidgetArea, _projectTreeView);
	m_DockManager->addDockWidget(ads::BottomDockWidgetArea, DockWidget);
	

}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
	if (m_DockManager)
	{
		m_DockManager->deleteLater();
	}
}
