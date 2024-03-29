#ifndef VALUESWIDGET_H
#define VALUESWIDGET_H

#include "DockWidget.h"
#include <src/widgets/HkTopNavigatorWidget.h>
#include <src/models/ProjectTreeModel.h>
#include <src/hkx/ResourceManager.h>
#include <src/log.h>

#include <QPlainTextEdit>
#include <QItemSelectionModel>
#include <QStackedLayout>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ValuesWidget;
}
QT_END_NAMESPACE

class ValuesWidget : public ::ads::CDockWidget
{
    Q_OBJECT

    ckcmd::HKX::ProjectModel* _model;
    QModelIndex _index;
    ckcmd::HKX::CommandManager& _command_manager;
    const ckcmd::HKX::ResourceManager & _manager;
    TopInfoWidget* _top_info;
    QPlainTextEdit* _empty_panel;
    QGridLayout* _mainLayout;
    QWidget* _mainWidget;
    QWidget* _editorPanel;
    QStackedLayout* _editorPanelLayout;


    void checkTopInfoPanel(const QModelIndex& index);
    void checkBindings(const QModelIndex& index);
    void setEditor(const QModelIndex& index);

public:
    explicit ValuesWidget
    (
        ckcmd::HKX::ProjectModel* model, 
        ckcmd::HKX::CommandManager& command_manager, 
        const ckcmd::HKX::ResourceManager& manager, 
        QWidget* parent = 0
    );
    ~ValuesWidget();

public slots:
    void treeSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif //VALUESWIDGET_H