#ifndef POWEREDRAGDOLLCONTROLSMODIFIERUI_H
#define POWEREDRAGDOLLCONTROLSMODIFIERUI_H

#include <QStackedWidget>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class QuadVariableWidget;
class SpinBox;
class CheckButtonCombo;
class QGroupBox;

namespace UI {

class HkxObject;
class hkbPoweredRagdollControlsModifier;
class hkbVariableBindingSet;
class BoneIndexArrayUI;
class BoneWeightArrayUI;

class PoweredRagdollControlsModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    PoweredRagdollControlsModifierUI();
    PoweredRagdollControlsModifierUI& operator=(const PoweredRagdollControlsModifierUI&) = delete;
    PoweredRagdollControlsModifierUI(const PoweredRagdollControlsModifierUI &) = delete;
    ~PoweredRagdollControlsModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *ragdollBones);
    void variableRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewRagdollBones(int index);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setMaxForce();
    void setTau();
    void setDamping();
    void setProportionalRecoveryVelocity();
    void setConstantRecoveryVelocity();
    void toggleBones(bool enable);
    void viewBones();
    void setPoseMatchingBone0(int index);
    void setPoseMatchingBone1(int index);
    void setPoseMatchingBone2(int index);
    void setMode(int index);
    void toggleBoneWeights(bool enable);
    void viewBoneWeights();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        BONE_INDEX_WIDGET,
        BONE_WEIGHT_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbPoweredRagdollControlsModifier *bsData;
    QGridLayout *topLyt;
    QGroupBox *groupBox;
    TableWidget *table;
    BoneIndexArrayUI *boneIndexUI;
    BoneWeightArrayUI *boneWeightsUI;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *maxForce;
    DoubleSpinBox *tau;
    DoubleSpinBox *damping;
    DoubleSpinBox *proportionalRecoveryVelocity;
    DoubleSpinBox *constantRecoveryVelocity;
    CheckButtonCombo *bones;
    ComboBox *poseMatchingBone0;
    ComboBox *poseMatchingBone1;
    ComboBox *poseMatchingBone2;
    ComboBox *mode;
    CheckButtonCombo *boneWeights;
};
}
#endif // POWEREDRAGDOLLCONTROLSMODIFIERUI_H
