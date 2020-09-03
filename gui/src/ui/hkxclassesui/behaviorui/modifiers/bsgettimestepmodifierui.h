#ifndef BSGETTIMESTEPMODIFIERUI_H
#define BSGETTIMESTEPMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class BSGetTimeStepModifier;
class hkbVariableBindingSet;

class BSGetTimeStepModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSGetTimeStepModifierUI();
    BSGetTimeStepModifierUI& operator=(const BSGetTimeStepModifierUI&) = delete;
    BSGetTimeStepModifierUI(const BSGetTimeStepModifierUI &) = delete;
    ~BSGetTimeStepModifierUI() = default;
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
    void setTimeStep();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSGetTimeStepModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *timeStep;
};
}
#endif // BSGETTIMESTEPMODIFIERUI_H
