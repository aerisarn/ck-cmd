#pragma once

#include <src/widgets/ModelWidget.h>

#include "ui_EventEditor.h"

class hkbNode;

class EventEditor : public ckcmd::ModelWidget, private Ui::EventEditor
{
    Q_OBJECT

private slots:

    virtual void OnIndexSelected() override;

public:
    explicit EventEditor(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

private slots:
    void on_SilentCheckBox_stateChanged(int state);
    void on_SyncPointCheckBox_stateChanged(int state);

private:

    StaticBindingTable table =
    {

    };

    virtual const StaticBindingTable& bindingTable() const override {
        return table;
    }
};