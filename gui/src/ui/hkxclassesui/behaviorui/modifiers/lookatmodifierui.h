#ifndef LOOKATMODIFIERUI_H
#define LOOKATMODIFIERUI_H

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
class hkbLookAtModifier;
class hkbVariableBindingSet;

class LookAtModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    LookAtModifierUI();
    LookAtModifierUI& operator=(const LookAtModifierUI&) = delete;
    LookAtModifierUI(const LookAtModifierUI &) = delete;
    ~LookAtModifierUI() = default;
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
    void setTargetWS();
    void setHeadForwardLS();
    void setNeckForwardLS();
    void setNeckRightLS();
    void setEyePositionHS();
    void setNewTargetGain();
    void setOnGain();
    void setOffGain();
    void setLimitAngleDegrees();
    void setLimitAngleLeft();
    void setLimitAngleRight();
    void setLimitAngleUp();
    void setLimitAngleDown();
    void setHeadIndex(int index);
    void setNeckIndex(int index);
    void setIsOn();
    void setIndividualLimitsOn();
    void setIsTargetInsideLimitCone();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbLookAtModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *targetWS;
    QuadVariableWidget *headForwardLS;
    QuadVariableWidget *neckForwardLS;
    QuadVariableWidget *neckRightLS;
    QuadVariableWidget *eyePositionHS;
    DoubleSpinBox *newTargetGain;
    DoubleSpinBox *onGain;
    DoubleSpinBox *offGain;
    DoubleSpinBox *limitAngleDegrees;
    DoubleSpinBox *limitAngleLeft;
    DoubleSpinBox *limitAngleRight;
    DoubleSpinBox *limitAngleUp;
    DoubleSpinBox *limitAngleDown;
    ComboBox *headIndex;
    ComboBox *neckIndex;
    CheckBox *isOn;
    CheckBox *individualLimitsOn;
    CheckBox *isTargetInsideLimitCone;
};
}
#endif // LOOKATMODIFIERUI_H
