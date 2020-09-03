#ifndef COMPUTEROTATIONFROMAXISANGLEMODIFIERUI_H
#define COMPUTEROTATIONFROMAXISANGLEMODIFIERUI_H

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
class hkbComputeRotationFromAxisAngleModifier;
class hkbVariableBindingSet;

class ComputeRotationFromAxisAngleModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    ComputeRotationFromAxisAngleModifierUI();
    ComputeRotationFromAxisAngleModifierUI& operator=(const ComputeRotationFromAxisAngleModifierUI&) = delete;
    ComputeRotationFromAxisAngleModifierUI(const ComputeRotationFromAxisAngleModifierUI &) = delete;
    ~ComputeRotationFromAxisAngleModifierUI() = default;
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
    void setRotationOut();
    void setAxis();
    void setAngleDegrees();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbComputeRotationFromAxisAngleModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *rotationOut;
    QuadVariableWidget *axis;
    DoubleSpinBox *angleDegrees;
};
}
#endif // COMPUTEROTATIONFROMAXISANGLEMODIFIERUI_H
