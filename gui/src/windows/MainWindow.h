#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include "DockManager.h"

#include <src/widgets/LogControl.h>
#include <src/workspace.h>
#include <src/models/ProjectTreeModel.h>
#include <src/widgets/ProjectsWidget.h>
#include <src/widgets/ValuesWidget.h>
#include <src/widgets/ActionHandler.h>

#include <src/animdata/AnimDataManager.h>
#include <src/hkx/HkxSimulation.h>
#include <src/hkx/CommandManager.h>

#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>



QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    HWND _vdb_handle;
public:
    explicit MainWindow(hkMemoryRouter* havok_router, QWidget* parent = 0);
    ~MainWindow();

    ILogger* getLogger() { return _logger; }

    void saveWindowSettings();
    void loadWindowSettings();

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:

    struct LogSetter {
        LogSetter(ILogger* logger) {
            ckcmd::Log::setBackend(logger);
        }
    };

    ads::CDockManager* m_DockManager;

    //UI
    Ui::MainWindow* ui;
    ckcmd::GUI::LogControl* _logger;
    LogSetter _log_setter;
    ProjectsWidget* _projectTreeView;
    ValuesWidget* _valuesTableView;
    HkxSimulation* _simulation;

    //Data
    WorkspaceConfig _workspace;
    ckcmd::HKX::ProjectTreeModel _model;
    ckcmd::HKX::CommandManager _command_manager;
    ckcmd::HKX::ResourceManager _resource_manager;
    ckcmd::HKX::AnimationManager _animation_manager;


    //Actions
    ckcmd::HKX::ActionHandler* _handler;

};

#endif // MAINWINDOW_H
