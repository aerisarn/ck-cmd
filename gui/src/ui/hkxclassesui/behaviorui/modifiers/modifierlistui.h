#ifndef MODIFIERLISTUI_H
#define MODIFIERLISTUI_H

#include <QGroupBox>

#include "src/utility.h"


class BehaviorGraphView;
class GenericTableWidget;
class TableWidget;
class SpinBox;
class ComboBox;
class LineEdit;
class CheckBox;
class QGridLayout;


namespace UI {

class HkxObject;
class hkbVariableBindingSet;
class hkbModifierList;

class ModifierListUI final: public QGroupBox
{
    Q_OBJECT
public:
    ModifierListUI();
    ModifierListUI& operator=(const ModifierListUI&) = delete;
    ModifierListUI(const ModifierListUI &) = delete;
    ~ModifierListUI() = default;
public:
    void loadData(HkxObject *data);
    void setBehaviorView(BehaviorGraphView *view);
    void modifierRenamed(const QString & name, int index);
    void variableRenamed(const QString & name, int index);
    void connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties);
signals:
    void modifierNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewModifiers(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setModifier(int index, const QString & name);
    void setBindingVariable(int index, const QString & name);
    void viewSelectedChild(int row, int column);
    void swapGeneratorIndices(int index1, int index2);
private:
    void toggleSignals(bool toggleconnections);
    void addModifier();
    void removeModifier(int index);
    void selectTableToView(bool viewproperties, const QString & path);
    void loadDynamicTableRows();
private:
    static const QStringList types;
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    hkbModifierList *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    ComboBox *typeSelectorCB;
};
}
#endif // MODIFIERLISTUI_H
