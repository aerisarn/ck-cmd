#ifndef BSMODIFYONCEMODIFIERUI_H
#define BSMODIFYONCEMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class BehaviorGraphView;

class QGridLayout;
class TableWidget;
class LineEdit;
class GenericTableWidget;
class CheckBox;

namespace UI {

class HkxObject;
class BSModifyOnceModifier;
class hkbVariableBindingSet;

class BSModifyOnceModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSModifyOnceModifierUI();
    BSModifyOnceModifierUI& operator=(const BSModifyOnceModifierUI&) = delete;
    BSModifyOnceModifierUI(const BSModifyOnceModifierUI &) = delete;
    ~BSModifyOnceModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void variableRenamed(const QString & name, int index);
    void modifierRenamed(const QString & name, int index);
    void connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties);
    void setBehaviorView(BehaviorGraphView *view);
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
    void viewSelected(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSModifyOnceModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
};
}
#endif // BSMODIFYONCEMODIFIERUI_H
