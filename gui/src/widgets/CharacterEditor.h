#pragma once

#include <src/widgets/ModelWidget.h>

#include "ui_CharacterEditor.h"

class hkbNode;

class CharacterEditorWidget : public ckcmd::ModelWidget, private Ui::CharacterEditor
{
    Q_OBJECT

    virtual void OnIndexSelected() override;

    virtual size_t dataBindingRowStart() const override { return 6; } //TODO: HandleCharacterData::SUPPORT_END

    StaticBindingTable dataBindingtable = {
    };

    virtual const StaticBindingTable& bindingTable() const override {
        return dataBindingtable;
    }


private slots:
    //    void resizeNameToContent(const QString& text);
    //    void on_treeView_doubleClicked(const QModelIndex& index);
    //    void on_treeView_customContextMenuRequested(const QPoint& pos);
    //    void on_treeView_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

public:
    explicit CharacterEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

    QSize sizeHint() const override;

private:

    QModelIndex _selected;
};