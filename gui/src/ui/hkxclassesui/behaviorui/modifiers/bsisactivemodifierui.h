#ifndef BSISACTIVEMODIFIERUI_H
#define BSISACTIVEMODIFIERUI_H

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
class BSIsActiveModifier;
class hkbVariableBindingSet;

class BSIsActiveModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSIsActiveModifierUI();
    BSIsActiveModifierUI& operator=(const BSIsActiveModifierUI&) = delete;
    BSIsActiveModifierUI(const BSIsActiveModifierUI &) = delete;
    ~BSIsActiveModifierUI() = default;
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
    void setIsActive0();
    void setInvertActive0();
    void setIsActive1();
    void setInvertActive1();
    void setIsActive2();
    void setInvertActive2();
    void setIsActive3();
    void setInvertActive3();
    void setIsActive4();
    void setInvertActive4();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSIsActiveModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    CheckBox *bIsActive0;
    CheckBox *bInvertActive0;
    CheckBox *bIsActive1;
    CheckBox *bInvertActive1;
    CheckBox *bIsActive2;
    CheckBox *bInvertActive2;
    CheckBox *bIsActive3;
    CheckBox *bInvertActive3;
    CheckBox *bIsActive4;
    CheckBox *bInvertActive4;
};
}
#endif // BSISACTIVEMODIFIERUI_H
