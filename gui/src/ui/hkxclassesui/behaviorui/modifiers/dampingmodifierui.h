#ifndef DAMPINGMODIFIERUI_H
#define DAMPINGMODIFIERUI_H

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
class SpinBox;

namespace UI {

class HkxObject;
class hkbDampingModifier;
class hkbVariableBindingSet;

class DampingModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    DampingModifierUI();
    DampingModifierUI& operator=(const DampingModifierUI&) = delete;
    DampingModifierUI(const DampingModifierUI &) = delete;
    ~DampingModifierUI() = default;
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
    void setKP();
    void setKI();
    void setKD();
    void setEnableScalarDamping();
    void setEnableVectorDamping();
    void setRawValue();
    void setDampedValue();
    void setRawVector();
    void setDampedVector();
    void setVecErrorSum();
    void setVecPreviousError();
    void setErrorSum();
    void setPreviousError();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbDampingModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *kP;
    DoubleSpinBox *kI;
    DoubleSpinBox *kD;
    CheckBox *enableScalarDamping;
    CheckBox *enableVectorDamping;
    DoubleSpinBox *rawValue;
    DoubleSpinBox *dampedValue;
    QuadVariableWidget *rawVector;
    QuadVariableWidget *dampedVector;
    QuadVariableWidget *vecErrorSum;
    QuadVariableWidget *vecPreviousError;
    DoubleSpinBox *errorSum;
    DoubleSpinBox *previousError;
};
}
#endif // DAMPINGMODIFIERUI_H
