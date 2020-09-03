#ifndef COMPUTEROTATIONTOTARGETMODIFIERUI_H
#define COMPUTEROTATIONTOTARGETMODIFIERUI_H

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
class hkbComputeRotationToTargetModifier;
class hkbVariableBindingSet;

class ComputeRotationToTargetModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    ComputeRotationToTargetModifierUI();
    ComputeRotationToTargetModifierUI& operator=(const ComputeRotationToTargetModifierUI&) = delete;
    ComputeRotationToTargetModifierUI(const ComputeRotationToTargetModifierUI &) = delete;
    ~ComputeRotationToTargetModifierUI() = default;
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
    void setTargetPosition();
    void setCurrentPosition();
    void setCurrentRotation();
    void setLocalAxisOfRotation();
    void setLocalFacingDirection();
    void setResultIsDelta();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbComputeRotationToTargetModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *rotationOut;
    QuadVariableWidget *targetPosition;
    QuadVariableWidget *currentPosition;
    QuadVariableWidget *currentRotation;
    QuadVariableWidget *localAxisOfRotation;
    QuadVariableWidget *localFacingDirection;
    CheckBox *resultIsDelta;
};
}
#endif // COMPUTEROTATIONTOTARGETMODIFIERUI_H
