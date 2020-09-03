#ifndef BSSPEEDSAMPLERMODIFIERUI_H
#define BSSPEEDSAMPLERMODIFIERUI_H

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
class BSSpeedSamplerModifier;
class hkbVariableBindingSet;

class BSSpeedSamplerModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSSpeedSamplerModifierUI();
    BSSpeedSamplerModifierUI& operator=(const BSSpeedSamplerModifierUI&) = delete;
    BSSpeedSamplerModifierUI(const BSSpeedSamplerModifierUI &) = delete;
    ~BSSpeedSamplerModifierUI() = default;
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
    void setState();
    void setDirection();
    void setGoalSpeed();
    void setSpeedOut();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSSpeedSamplerModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *state;
    DoubleSpinBox *direction;
    DoubleSpinBox *goalSpeed;
    DoubleSpinBox *speedOut;
};
}
#endif // BSSPEEDSAMPLERMODIFIERUI_H
