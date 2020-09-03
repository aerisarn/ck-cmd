#ifndef TWISTMODIFIERUI_H
#define TWISTMODIFIERUI_H

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
class hkbTwistModifier;
class hkbVariableBindingSet;

class TwistModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    TwistModifierUI();
    TwistModifierUI& operator=(const TwistModifierUI&) = delete;
    TwistModifierUI(const TwistModifierUI &) = delete;
    ~TwistModifierUI() = default;
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
    void setAxisOfRotation();
    void setTwistAngle();
    void setStartBoneIndex(int index);
    void setEndBoneIndex(int index);
    void setSetAngleMethod(int index);
    void setRotationAxisCoordinates(int index);
    void setIsAdditive();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbTwistModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *axisOfRotation;
    DoubleSpinBox *twistAngle;
    ComboBox *startBoneIndex;
    ComboBox *endBoneIndex;
    ComboBox *setAngleMethod;
    ComboBox *rotationAxisCoordinates;
    CheckBox *isAdditive;
};
}
#endif // TWISTMODIFIERUI_H
