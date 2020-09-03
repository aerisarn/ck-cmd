#ifndef GETUPMODIFIERUI_H
#define GETUPMODIFIERUI_H

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
class hkbGetUpModifier;
class hkbVariableBindingSet;	

class GetUpModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    GetUpModifierUI();
    GetUpModifierUI& operator=(const GetUpModifierUI&) = delete;
    GetUpModifierUI(const GetUpModifierUI &) = delete;
    ~GetUpModifierUI() = default;
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
    void setGroundNormal();
    void setDuration();
    void setAlignWithGroundDuration();
    void setRootBoneIndex(int index);
    void setOtherBoneIndex(int index);
    void setAnotherBoneIndex(int index);
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbGetUpModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *groundNormal;
    DoubleSpinBox *duration;
    DoubleSpinBox *alignWithGroundDuration;
    ComboBox *rootBoneIndex;
    ComboBox *otherBoneIndex;
    ComboBox *anotherBoneIndex;
};
}
#endif // GETUPMODIFIERUI_H
