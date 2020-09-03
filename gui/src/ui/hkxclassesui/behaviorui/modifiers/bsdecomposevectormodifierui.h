#ifndef BSDECOMPOSEVECTORMODIFIERUI_H
#define BSDECOMPOSEVECTORMODIFIERUI_H

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
class BSDecomposeVectorModifier;
class hkbVariableBindingSet;

class BSDecomposeVectorModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSDecomposeVectorModifierUI();
    BSDecomposeVectorModifierUI& operator=(const BSDecomposeVectorModifierUI&) = delete;
    BSDecomposeVectorModifierUI(const BSDecomposeVectorModifierUI &) = delete;
    ~BSDecomposeVectorModifierUI() = default;
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
    void setVector();
    void setX();
    void setY();
    void setZ();
    void setW();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSDecomposeVectorModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *vector;
    DoubleSpinBox *x;
    DoubleSpinBox *y;
    DoubleSpinBox *z;
    DoubleSpinBox *w;
};
}
#endif // BSDECOMPOSEVECTORMODIFIERUI_H
