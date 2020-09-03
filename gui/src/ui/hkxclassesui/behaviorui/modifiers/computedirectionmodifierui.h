#ifndef COMPUTEDIRECTIONMODIFIERUI_H
#define COMPUTEDIRECTIONMODIFIERUI_H

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
class hkbComputeDirectionModifier;
class hkbVariableBindingSet;

class ComputeDirectionModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    ComputeDirectionModifierUI();
    ComputeDirectionModifierUI& operator=(const ComputeDirectionModifierUI&) = delete;
    ComputeDirectionModifierUI(const ComputeDirectionModifierUI &) = delete;
    ~ComputeDirectionModifierUI() = default;
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
    void setPointIn();
    void setPointOut();
    void setGroundAngleOut();
    void setUpAngleOut();
    void setVerticalOffset();
    void setReverseGroundAngle();
    void setReverseUpAngle();
    void setProjectPoint();
    void setNormalizePoint();
    void setComputeOnlyOnce();
    void setComputedOutput();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbComputeDirectionModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *pointIn;
    QuadVariableWidget *pointOut;
    DoubleSpinBox *groundAngleOut;
    DoubleSpinBox *upAngleOut;
    DoubleSpinBox *verticalOffset;
    CheckBox *reverseGroundAngle;
    CheckBox *reverseUpAngle;
    CheckBox *projectPoint;
    CheckBox *normalizePoint;
    CheckBox *computeOnlyOnce;
    CheckBox *computedOutput;
};
}
#endif // COMPUTEDIRECTIONMODIFIERUI_H
