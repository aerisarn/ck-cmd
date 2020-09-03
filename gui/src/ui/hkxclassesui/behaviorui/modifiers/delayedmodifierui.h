#ifndef DELAYEDMODIFIERUI_H
#define DELAYEDMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class BehaviorGraphView;
class TableWidget;
class LineEdit;
class QGridLayout;
class CheckBox;
class GenericTableWidget;
class DoubleSpinBox;

namespace UI {

class HkxObject;
class hkbDelayedModifier;
class hkbVariableBindingSet;

class DelayedModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    DelayedModifierUI();
    DelayedModifierUI& operator=(const DelayedModifierUI&) = delete;
    DelayedModifierUI(const DelayedModifierUI &) = delete;
    ~DelayedModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties);
    void variableRenamed(const QString & name, int index);
    void modifierRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewModifiers(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setDelaySeconds();
    void setDurationSeconds();
    void setSecondsElapsed();
    void setBindingVariable(int index, const QString & name);
    void viewSelected(int row, int column);
    void setModifier(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    hkbDelayedModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *delaySeconds;
    DoubleSpinBox *durationSeconds;
    DoubleSpinBox *secondsElapsed;
};
}
#endif // DELAYEDMODIFIERUI_H
