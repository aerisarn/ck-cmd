#pragma once

#include <src/widgets/ModelWidget.h>

#include "ui_HkTopNavigatorWidget.h"

class hkbNode;

class TopInfoWidget : public ckcmd::ModelWidget, private Ui::HkTopNavigator
{
    Q_OBJECT

    void refreshBindings();

private slots:
    void on_nameLineEdit_textChanged(const QString& text);
    void on_addBindingToolButton_clicked();
    void on_removeBindingToolButton_clicked();
//    void resizeNameToContent(const QString& text);
//    void on_treeView_doubleClicked(const QModelIndex& index);
//    void on_treeView_customContextMenuRequested(const QPoint& pos);
//    void on_treeView_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    virtual void OnIndexSelected() override;

public:
    explicit TopInfoWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

    QSize sizeHint() const override;

private:

    StaticBindingTable table = 
    {

    };

    virtual const StaticBindingTable& bindingTable() const override {
        return table;
    }
};