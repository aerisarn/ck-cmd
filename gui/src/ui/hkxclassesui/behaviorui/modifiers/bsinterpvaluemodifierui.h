#ifndef BSINTERPVALUEMODIFIERUI_H
#define BSINTERPVALUEMODIFIERUI_H

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
class BSInterpValueModifier;
class hkbVariableBindingSet;

class BSInterpValueModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSInterpValueModifierUI();
    BSInterpValueModifierUI& operator=(const BSInterpValueModifierUI&) = delete;
    BSInterpValueModifierUI(const BSInterpValueModifierUI &) = delete;
    ~BSInterpValueModifierUI() = default;
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
    void setSource();
    void setTarget();
    void setResult();
    void setGain();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSInterpValueModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *source;
    DoubleSpinBox *target;
    DoubleSpinBox *result;
    DoubleSpinBox *gain;
};
}
#endif // BSINTERPVALUEMODIFIERUI_H
