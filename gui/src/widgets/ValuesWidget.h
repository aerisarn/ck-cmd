#ifndef VALUESWIDGET_H
#define VALUESWIDGET_H

#include "DockWidget.h"
#include <src/widgets/HkTopNavigatorWidget.h>
#include <src/models/HkxItemTableModel.h>
#include <src/models/ProjectTreeModel.h>
#include <src/hkx/ResourceManager.h>
#include <src/log.h>

#include <QPlainTextEdit>
#include <QItemSelectionModel>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ValuesWidget;
}
QT_END_NAMESPACE

class ValuesWidget : public ::ads::CDockWidget
{
    Q_OBJECT

    ckcmd::HKX::ProjectTreeModel* _model;
    QModelIndex _index;
    ckcmd::HKX::CommandManager& _command_manager;
    const ckcmd::HKX::ResourceManager & _manager;
    TopInfoWidget* _top_info;
    QPlainTextEdit* _empty_panel;
    QGridLayout* _mainLayout;

public:
    explicit ValuesWidget(ckcmd::HKX::ProjectTreeModel* model, ckcmd::HKX::CommandManager& command_manager, const ckcmd::HKX::ResourceManager& manager, QWidget* parent = 0);
    ~ValuesWidget();

public slots:
    void treeSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    //void modelHasSetNewHkxItemPointer(ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);

//signals:
//    void HkxItemPointerChanged(QModelIndex index, QModelIndex parent, ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);
};

#endif //VALUESWIDGET_H