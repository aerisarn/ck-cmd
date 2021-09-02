#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "DockWidget.h"

#include <src/log.h>
#include <src/models/ProjectTreeModel.h>
#include <src/hkx/ResourceManager.h>
#include <src/hkx/HkxSimulation.h>
#include <src/hkx/HkxItemPointer.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ProjectsWidget;
}
QT_END_NAMESPACE

class ProjectsWidget : public ::ads::CDockWidget
{
    Q_OBJECT

    ckcmd::HKX::ProjectTreeModel* _model;
    ckcmd::HKX::ResourceManager* _manager;
    HkxSimulation* _simulation;

public:
    explicit ProjectsWidget(
        ckcmd::HKX::ProjectTreeModel* model,
        ckcmd::HKX::ResourceManager* manager,
        HkxSimulation* simulation,
        QWidget* parent = 0);
    ~ProjectsWidget();

public slots:
    void nodeDoubleClicked(const QModelIndex& index);
    void nodeClicked(const QModelIndex& index);
    void treeMenu(QPoint);
    void modelHasSetNewHkxItemPointer(ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);

signals:
    //file index in resource manager, object in file
    void variantSelected(size_t, hkVariant*, hkVariant*);

private:
    Ui::ProjectsWidget* ui;
};

#endif //PROJECTSWIDGET_H