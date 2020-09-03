#ifndef EVALUATEHANDLEMODIFIERUI_H
#define EVALUATEHANDLEMODIFIERUI_H

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
class hkbEvaluateHandleModifier;
class hkbVariableBindingSet;

class EvaluateHandleModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    EvaluateHandleModifierUI();
    EvaluateHandleModifierUI& operator=(const EvaluateHandleModifierUI&) = delete;
    EvaluateHandleModifierUI(const EvaluateHandleModifierUI &) = delete;
    ~EvaluateHandleModifierUI() = default;
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
    void setHandlePositionOut();
    void setHandleRotationOut();
    void setIsValidOut();
    void setExtrapolationTimeStep();
    void setHandleChangeSpeed();
    void setHandleChangeMode(int index);
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbEvaluateHandleModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *handlePositionOut;
    QuadVariableWidget *handleRotationOut;
    CheckBox *isValidOut;
    DoubleSpinBox *extrapolationTimeStep;
    DoubleSpinBox *handleChangeSpeed;
    ComboBox *handleChangeMode;
};
}
#endif // EVALUATEHANDLEMODIFIERUI_H
