#pragma once

#include <src/widgets/ModelWidget.h>

#include <QStringListModel>
#include <QCompleter>

#include "ui_TransitionEditor.h"
#include <src/items/HkxItemReal.h>

class TransitionEditorWidget : public ckcmd::ModelWidget, private Ui::TransitionEditor
{

    Q_OBJECT

    virtual void OnIndexSelected() override;

    StaticBindingTable dataBindingtable = {
    };

    virtual const StaticBindingTable& bindingTable() const override {
        return dataBindingtable;
    }

private slots:
    void on_effectToolButton_clicked();
    void on_conditionToolButton_clicked();

public:
    explicit TransitionEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

private:
    void addOrSet(const char* member_name);
};