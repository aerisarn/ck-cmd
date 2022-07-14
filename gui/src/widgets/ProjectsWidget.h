#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "DockWidget.h"

#include "ui_ProjectsWidget.h"

#include <src/log.h>
#include <src/models/ProjectTreeModel.h>
#include <src/hkx/CommandManager.h>
#include <src/hkx/ResourceManager.h>
#include <src/hkx/HkxSimulation.h>
#include <src/items/HkxItemPointer.h>
#include <src/widgets/ActionHandler.h>
#include <src/widgets/TreeContextMenuBuilder.h>

#include <QTreeView>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ProjectsWidget;
}
QT_END_NAMESPACE

class ProjectsWidget : public ::ads::CDockWidget, private Ui::ProjectsWidget
{
    Q_OBJECT

    ckcmd::HKX::ProjectTreeModel* _model;
    ckcmd::HKX::CommandManager& _commandManager;
    ckcmd::HKX::ResourceManager& _manager;
    ckcmd::HKX::TreeContextMenuBuilder _menuBuilder;
    HkxSimulation* _simulation;

public:
    explicit ProjectsWidget(
        ckcmd::HKX::ProjectModel* model,
        ckcmd::HKX::CommandManager& commandManager,
        ckcmd::HKX::ResourceManager& manager,
        ckcmd::HKX::ActionHandler& actionsHandler,
        HkxSimulation* simulation,
        QWidget* parent = 0);
    ~ProjectsWidget();

    QModelIndex currentIndex() { return _model->mapToSource(treeView->selectionModel()->currentIndex()); }

    QTreeView& view();

private slots:
    void on_treeView_doubleClicked(const QModelIndex& index);
    void on_treeView_customContextMenuRequested(const QPoint& pos);
    void on_treeView_selectionChanged(const QModelIndex& current, const QModelIndex& previous);

public slots:

    void on_search_found(const QModelIndex& index);

    void modelHasSetNewHkxItemPointer(
        const QModelIndex& parent, 
        const QModelIndex& index, 
        ckcmd::HKX::HkxItemPointer old_value, 
        ckcmd::HKX::HkxItemPointer new_value, 
        int file, 
        hkVariant* variant
    );

signals:

    void selectionChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif //PROJECTSWIDGET_H