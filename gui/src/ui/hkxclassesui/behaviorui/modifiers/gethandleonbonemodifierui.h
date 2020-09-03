#ifndef GETHANDLEONBONEMODIFIERUI_H
#define GETHANDLEONBONEMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class hkbGetHandleOnBoneModifier;
class hkbVariableBindingSet;

class GetHandleOnBoneModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    GetHandleOnBoneModifierUI();
    GetHandleOnBoneModifierUI& operator=(const GetHandleOnBoneModifierUI&) = delete;
    GetHandleOnBoneModifierUI(const GetHandleOnBoneModifierUI &) = delete;
    ~GetHandleOnBoneModifierUI() = default;
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
    void setLocalFrameName(int index);
    void setRagdollBoneIndex(int index);
    void setAnimationBoneIndex(int index);
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbGetHandleOnBoneModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    ComboBox *localFrameName;
    ComboBox *ragdollBoneIndex;
    ComboBox *animationBoneIndex;
};
}
#endif // GETHANDLEONBONEMODIFIERUI_H
