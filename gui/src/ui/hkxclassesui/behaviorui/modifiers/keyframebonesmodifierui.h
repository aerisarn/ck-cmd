#ifndef KEYFRAMEBONESMODIFIERUI_H
#define KEYFRAMEBONESMODIFIERUI_H

#include <QStackedWidget>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class SpinBox;
class QGroupBox;
class QuadVariableWidget;
class CheckButtonCombo;

namespace UI {

class HkxObject;
class hkbKeyframeBonesModifier;
class hkbVariableBindingSet;

class KeyframeInfoUI;
class BoneIndexArrayUI;

class KeyframeBonesModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    KeyframeBonesModifierUI();
    KeyframeBonesModifierUI& operator=(const KeyframeBonesModifierUI&) = delete;
    KeyframeBonesModifierUI(const KeyframeBonesModifierUI &) = delete;
    ~KeyframeBonesModifierUI() = default;
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
    void toggleKeyframedBonesList(bool enable);
    void viewKeyframedBonesList();
    void viewSelectedChild(int row, int column);
    void variableTableElementSelected(int index, const QString &name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void addKeyframeInfo();
    void removeKeyframeInfo(int index);
    void loadDynamicTableRows();
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        KEYFRAME_WIDGET,
        BONE_INDEX_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbKeyframeBonesModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    BoneIndexArrayUI *boneIndexUI;
    KeyframeInfoUI *keyframeInfoUI;
    LineEdit *name;
    CheckBox *enable;
    CheckButtonCombo *keyframedBonesList;
};
}
#endif // KEYFRAMEBONESMODIFIERUI_H
