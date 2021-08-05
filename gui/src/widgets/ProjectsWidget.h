#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "DockWidget.h"

#include <src/log.h>
#include <src/models/ProjectTreeModel.h>
#include <src/hkx/ResourceManager.h>


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

public:
    explicit ProjectsWidget(ckcmd::HKX::ProjectTreeModel* model, ckcmd::HKX::ResourceManager* manager, QWidget* parent = 0);
    ~ProjectsWidget();

public slots:
    void nodeDoubleClicked(const QModelIndex& index);
    void nodeClicked(const QModelIndex& index);

signals:
    void variantSelected(hkVariant*);

private:
    Ui::ProjectsWidget* ui;
};

#endif //PROJECTSWIDGET_H