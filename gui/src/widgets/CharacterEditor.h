#pragma once

#include <src/widgets/ModelWidget.h>

#include <QStringListModel>
#include <QCompleter>

#include "ui_CharacterEditor.h"
#include <src/hkx/HkxItemReal.h>

class hkbNode;

class CharacterEditorWidget : public ckcmd::ModelWidget, private Ui::CharacterEditor
{
    Q_OBJECT
    Q_PROPERTY(ckcmd::HKX::HkxItemReal mirrorAxis READ readMirrorAxis WRITE writeMirrorAxis USER true NOTIFY mirrorAxisChanged)

    virtual void OnIndexSelected() override;

    virtual size_t dataBindingRowStart() const override { return 6; } //TODO: HandleCharacterData::SUPPORT_END

    StaticBindingTable dataBindingtable = {
    };

    virtual const StaticBindingTable& bindingTable() const override {
        return dataBindingtable;
    }

    void setBehaviorField();
    void setRigField();
    void setRigControls();
    void setAnimationSetsControls();

    void updateSetEvents(int set_index);
    void updateSetAttacks(int set_index);
    void updateSetConditions(int set_index);
    void updateSetAnimations(int set_index);

private slots:
    void on_mirroringXDoubleSpinBox_valueChanged(double d);
    void on_mirroringYDoubleSpinBox_valueChanged(double d);
    void on_mirroringZDoubleSpinBox_valueChanged(double d);
    void on_selectBoneTreeView_clicked(const QModelIndex& current);
    void on_selectPairedBoneTreeView_clicked(const QModelIndex& current);
    void on_setCurrentComboBox_currentIndexChanged(int index);

public:
    explicit CharacterEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

    QSize sizeHint() const override;

    ckcmd::HKX::HkxItemReal readMirrorAxis();
    void writeMirrorAxis(ckcmd::HKX::HkxItemReal value);

Q_SIGNALS:
    void mirrorAxisChanged(ckcmd::HKX::HkxItemReal value);

private:

    QModelIndex _selected;
    QStringListModel* _behavior_file_options;
    QCompleter* _behavior_completer;
    QStringListModel* _rig_file_options;
    QCompleter* _rig_completer;
    QAbstractItemModel* _rig_model;
};