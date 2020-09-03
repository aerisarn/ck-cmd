#ifndef RIGIDBODYRAGDOLLCONTROLSMODIFIERUI_H
#define RIGIDBODYRAGDOLLCONTROLSMODIFIERUI_H

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
class hkbRigidBodyRagdollControlsModifier;
class hkbVariableBindingSet;
class BoneIndexArrayUI;

class RigidBodyRagdollControlsModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    RigidBodyRagdollControlsModifierUI();
    RigidBodyRagdollControlsModifierUI& operator=(const RigidBodyRagdollControlsModifierUI&) = delete;
    RigidBodyRagdollControlsModifierUI(const RigidBodyRagdollControlsModifierUI &) = delete;
    ~RigidBodyRagdollControlsModifierUI() = default;
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
    void setHierarchyGain();
    void setVelocityDamping();
    void setAccelerationGain();
    void setVelocityGain();
    void setPositionGain();
    void setPositionMaxLinearVelocity();
    void setPositionMaxAngularVelocity();
    void setSnapGain();
    void setSnapMaxLinearVelocity();
    void setSnapMaxAngularVelocity();
    void setSnapMaxLinearDistance();
    void setSnapMaxAngularDistance();
    void setDurationToBlend();
    void toggleBones(bool enable);
    void viewBones();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        CHILD_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbRigidBodyRagdollControlsModifier *bsData;
    QGridLayout *topLyt;
    QGroupBox *groupBox;
    TableWidget *table;
    BoneIndexArrayUI *boneIndexUI;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *hierarchyGain;
    DoubleSpinBox *velocityDamping;
    DoubleSpinBox *accelerationGain;
    DoubleSpinBox *velocityGain;
    DoubleSpinBox *positionGain;
    DoubleSpinBox *positionMaxLinearVelocity;
    DoubleSpinBox *positionMaxAngularVelocity;
    DoubleSpinBox *snapGain;
    DoubleSpinBox *snapMaxLinearVelocity;
    DoubleSpinBox *snapMaxAngularVelocity;
    DoubleSpinBox *snapMaxLinearDistance;
    DoubleSpinBox *snapMaxAngularDistance;
    DoubleSpinBox *durationToBlend;
    CheckButtonCombo *bones;
};
}
#endif // RIGIDBODYRAGDOLLCONTROLSMODIFIERUI_H
