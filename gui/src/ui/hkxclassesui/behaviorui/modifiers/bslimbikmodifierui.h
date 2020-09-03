#ifndef BSLIMBIKMODIFIERUI_H
#define BSLIMBIKMODIFIERUI_H

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
class BSLimbIKModifier;
class hkbVariableBindingSet;

class BSLimbIKModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSLimbIKModifierUI();
    BSLimbIKModifierUI& operator=(const BSLimbIKModifierUI&) = delete;
    BSLimbIKModifierUI(const BSLimbIKModifierUI &) = delete;
    ~BSLimbIKModifierUI() = default;
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
    void setLimitAngleDegrees();
    void setStartBoneIndex(int index);
    void setEndBoneIndex(int index);
    void setGain();
    void setBoneRadius();
    void setCastOffset();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSLimbIKModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *limitAngleDegrees;
    ComboBox *startBoneIndex;
    ComboBox *endBoneIndex;
    DoubleSpinBox *gain;
    DoubleSpinBox *boneRadius;
    DoubleSpinBox *castOffset;
};
}
#endif // BSLIMBIKMODIFIERUI_H
