#ifndef EXTRACTRAGDOLLPOSEMODIFIERUI_H
#define EXTRACTRAGDOLLPOSEMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class QuadVariableWidget;

namespace UI {

class HkxObject;
class hkbExtractRagdollPoseModifier;
class hkbVariableBindingSet;

class ExtractRagdollPoseModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    ExtractRagdollPoseModifierUI();
    ExtractRagdollPoseModifierUI& operator=(const ExtractRagdollPoseModifierUI&) = delete;
    ExtractRagdollPoseModifierUI(const ExtractRagdollPoseModifierUI &) = delete;
    ~ExtractRagdollPoseModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties);
    void variableRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setPoseMatchingBone0(int index);
    void setPoseMatchingBone1(int index);
    void setPoseMatchingBone2(int index);
    void setEnableComputeWorldFromModel();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbExtractRagdollPoseModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    ComboBox *poseMatchingBone0;
    ComboBox *poseMatchingBone1;
    ComboBox *poseMatchingBone2;
    CheckBox *enableComputeWorldFromModel;
};
}
#endif // EXTRACTRAGDOLLPOSEMODIFIERUI_H
