#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPlainTextEdit>

#include <QWindow>
#include <QProcess>

static HWND g_vdb_handle;

BOOL CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring)
{
	const DWORD TITLE_SIZE = 1024;
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE))
	{
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL)
		{
			// We found the window! Stop enumerating.
			g_vdb_handle = hwnd;
			return false;
		}
	}
	return true; // Need to continue enumerating windows
}

static QWidget* createHDBWidget() {
	const TCHAR substring[] = TEXT("HavokVisualDebugger");


	QProcess::startDetached("hkVisualDebugger.exe");
	Sleep(1000);
	EnumWindows(FindWindowBySubstr, (LPARAM)substring);

	QWindow* window = QWindow::fromWinId((WId)g_vdb_handle);
	window->setFlags(Qt::FramelessWindowHint);
	return QWidget::createWindowContainer(window);
}

MainWindow::MainWindow(hkMemoryRouter* havok_router, QWidget* parent) :
	QMainWindow(parent),
	_workspace(Settings.get<std::string>("/general/workspace_folder")),
	_logger(new ckcmd::GUI::LogControl()),
	_log_setter(_logger),
	_resource_manager(_workspace),
	_animation_manager(_workspace, _resource_manager),
	_model(_command_manager, _resource_manager, this),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
	Layout->setContentsMargins(QMargins(0, 0, 0, 0));
	m_DockManager = new ads::CDockManager(ui->dockContainer);
	Layout->addWidget(m_DockManager);

	ads::CDockWidget* LogWidget = new ads::CDockWidget("Logger", this);
	LogWidget->setWidget(_logger);
	

	_model._rootNode = _resource_manager.createStatic({ _workspace.getFolder().string().c_str() }, NULL);
	_animation_manager.buildProjectTree(_model._rootNode);
	_simulation = new HkxSimulation(havok_router, _resource_manager);

	_projectTreeView = new ProjectsWidget(&_model, _command_manager, _resource_manager, _simulation, this);
	_handler = new ckcmd::HKX::ActionHandler(_resource_manager, _projectTreeView->view(), this);
	_valuesTableView = new ValuesWidget(_command_manager, _resource_manager, this);

	ads::CDockWidget*  GLWidget = new ads::CDockWidget("Havok Preview", this);
	GLWidget->setWidget(createHDBWidget());

	connect(_projectTreeView, &ProjectsWidget::variantSelected, _valuesTableView, &ValuesWidget::setVariant);
	connect(_valuesTableView, &ValuesWidget::HkxItemPointerChanged, _projectTreeView, &ProjectsWidget::modelHasSetNewHkxItemPointer);

	//File
	ui->menuFile->addAction(_handler->saveAction());

	//Edit
	ui->menuEdit->addAction(_command_manager.createUndoAction(this));
	ui->menuEdit->addAction(_command_manager.createRedoAction(this));

	//View
	ui->menuView->addAction(LogWidget->toggleViewAction());
	ui->menuView->addAction(_projectTreeView->toggleViewAction());
	ui->menuView->addAction(_valuesTableView->toggleViewAction());
	ui->menuView->addAction(GLWidget->toggleViewAction());

	//Widgets
	m_DockManager->addDockWidget(ads::CenterDockWidgetArea, _valuesTableView);
	m_DockManager->addDockWidgetTab(ads::CenterDockWidgetArea, GLWidget);
	m_DockManager->addDockWidget(ads::LeftDockWidgetArea, _projectTreeView);
	m_DockManager->addDockWidget(ads::BottomDockWidgetArea, LogWidget);

	loadWindowSettings();
}

MainWindow::~MainWindow()
{
	delete _simulation;
	delete ui;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
	saveWindowSettings();
	QMainWindow::closeEvent(event);
	if (m_DockManager)
	{
		m_DockManager->deleteLater();
	}
}

void MainWindow::saveWindowSettings()
{
	QSettings settings((_workspace.getFolder() / "layout.ini").string().c_str(), QSettings::IniFormat);
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	m_DockManager->addPerspective("dockLayout");
	m_DockManager->savePerspectives(settings);
	qDebug() << "Settings saved under:" << settings.fileName();
}

void MainWindow::loadWindowSettings()
{
	QSettings settings((_workspace.getFolder() / "layout.ini").string().c_str(), QSettings::IniFormat);
	QByteArray saved_geometry = settings.value("geometry").toByteArray();
	restoreGeometry(saved_geometry);
	QByteArray saved_state = settings.value("windowState").toByteArray();
	restoreState(saved_state);
	m_DockManager->loadPerspectives(settings);
	m_DockManager->openPerspective("dockLayout");
	qDebug() << "Settings loaded.";
}
