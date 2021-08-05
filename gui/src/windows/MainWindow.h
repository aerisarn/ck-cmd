#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include "DockManager.h"

#include <src/widgets/LogControl.h>
#include <src/models/ProjectTreeModel.h>
#include <src/widgets/ProjectsWidget.h>
#include <src/widgets/ValuesWidget.h>
#include <src/animdata/AnimDataManager.h>





QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    ILogger* getLogger() { return _logger; }

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:

    struct LogSetter {
        LogSetter(ILogger* logger) {
            ckcmd::Log::setBackend(logger);
        }
    };

    ads::CDockManager* m_DockManager;
    fs::path _workspace;

    //UI
    Ui::MainWindow* ui;
    ckcmd::GUI::LogControl* _logger;
    LogSetter _log_setter;
    ProjectsWidget* _projectTreeView;
    ValuesWidget* _valuesTableView;
    
    //Data
    ckcmd::HKX::ProjectTreeModel _model;
    ckcmd::HKX::AnimationManager _animation_manager;
    ckcmd::HKX::ResourceManager _resource_manager;


};

#endif // MAINWINDOW_H
