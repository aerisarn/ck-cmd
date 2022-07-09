#pragma once

#include <src/widgets/ModelWidget.h>

#include "ui_VariableEditor.h"

class VariableEditor : public ckcmd::ModelWidget, private Ui::VariableEditor
{
    Q_OBJECT

    QWidget* defaultValueEditor(int type, const QVariant& value);
    QWidget* _editor;

private slots:

    virtual void OnIndexSelected() override;

public:
    explicit VariableEditor(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

private slots:
    //void on_SilentCheckBox_stateChanged(int state);
    //void on_SyncPointCheckBox_stateChanged(int state);

private:

    StaticBindingTable table =
    {

    };

    virtual const StaticBindingTable& bindingTable() const override {
        return table;
    }
};