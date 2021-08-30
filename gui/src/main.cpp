#include "windows/mainwindow.h"

#include <QApplication>
#include <QColor>
#include <QPushButton>
#include <QMessageBox>
#include <QTranslator>

#include <src/windows/WorkspaceLauncher.h>

#include <commands/CommandBase.h>
#include <core/games.h>
#include <src/log.h>

#include "src/config.h"



//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}

static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static hkMemoryRouter* InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
	return pMemoryRouter;
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>






int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);



	//const fs::path workspace = "D:\\skywind\\Data\\meshes";

	//GlobalConfig::GetInstance()._workspace_folder = workspace.string();

	//fs::path animDataPath = workspace / "animationdatasinglefile.txt";
	//fs::path animDataSetPath = workspace / "animationsetdatasinglefile.txt";
	//Log::Info("Loading cache, loading %s and %s", animDataPath.string().c_str(), animDataSetPath.string().c_str());
	//AnimationCache cache(animDataPath, animDataSetPath);
	//Log::Info("Loaded %d projects", cache.getNumProjects());
	
	
	//MainWindow w;
	//InitializeHavok();
    //w.setStyleSheet("QWidget {background: blue;font-weight: bold; color: red}QComboBox {background: yellow}");
    //w.setWindowTitle("Skyrim Behavior Tool");
    //w.show();

	//QAbstractItemModel* model = new ProjectListModel(cache);

	//QListView* view = new QListView;

	//view->setWindowTitle("Projects");
	//view->setModel(model);
	//view->show();

	//ProjectInfoModel* table_model = new ProjectInfoModel(
	//	{ 
	//		cache.projects_index.begin()->first,
	//		cache.projects_index.begin()->second
	//	}
	//);

	//QTableView* table_view = new QTableView;
	//table_view->setWindowTitle("Project Info");
	//table_view->setModel(table_model);
	//table_view->show();
	//table_view->setTextElideMode(Qt::ElideNone);
	//table_view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	//QItemSelectionModel* sm = view->selectionModel();
	//QObject::connect(sm, &QItemSelectionModel::currentRowChanged,
	//	[&](const QModelIndex& current, const QModelIndex& previous) 
	//		{ 
	//			table_model->setData(
	//				{
	//					cache.project_at(current.row()),
	//					cache.find(cache.project_at(current.row()))
	//				}
	//			);
	//		}
	//);

	QApplication a(argc, argv);

	std::string workspace_path = Settings.get<std::string>("/general/workspace_folder");
	if (workspace_path.empty() || !fs::exists(workspace_path))
	{
		//Select workspace dialog
		WorkspaceLauncher launcher;
		if (launcher.exec() != QDialog::Accepted) {
			return 1; // Error, for example
		}
		Settings.set("/general/workspace_folder", launcher._workspace_folder);
		workspace_path = launcher._workspace_folder.toStdString();
	}



	MainWindow w(InitializeHavok());
	w.show();

	//Check if the workspace is empty
	if (fs::is_empty(workspace_path)) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			NULL, 
			QMessageBox::tr("Workspace empty"), 
			QMessageBox::tr("The selected workspace is empty, would you like to unpack vanilla LE resources?"),
			QMessageBox::Yes | QMessageBox::No
		);
		if (reply == QMessageBox::Yes) {
			fs::path data_path;
			auto& games = Games::Instance();
			if (!games.isGameInstalled(Games::Game::TES5)) {
				
			}
			else {
				data_path = games.data(Games::Game::TES5);
			}
			
		}
		else {
			
		}
	}

	int res = a.exec();
	CloseHavok();
	return res;
}
