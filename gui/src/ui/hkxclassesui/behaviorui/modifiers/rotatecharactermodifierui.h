#ifndef ROTATECHARACTERMODIFIERUI_H
#define ROTATECHARACTERMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class QGridLayout;
class TableWidget;
class LineEdit;
class QuadVariableWidget;
class CheckBox;
class GenericTableWidget;

class DoubleSpinBox;

namespace UI {

class HkxObject;
class hkbRotateCharacterModifier;
class hkbVariableBindingSet;

class RotateCharacterModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    RotateCharacterModifierUI();
    RotateCharacterModifierUI& operator=(const RotateCharacterModifierUI&) = delete;
    RotateCharacterModifierUI(const RotateCharacterModifierUI &) = delete;
    ~RotateCharacterModifierUI() = default;
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
    void setDegreesPerSecond();
    void setSpeedMultiplier();
    void setAxisOfRotation();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbRotateCharacterModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *degreesPerSecond;
    DoubleSpinBox *speedMultiplier;
    QuadVariableWidget *axisOfRotation;
};
}
#endif // ROTATECHARACTERMODIFIERUI_H
