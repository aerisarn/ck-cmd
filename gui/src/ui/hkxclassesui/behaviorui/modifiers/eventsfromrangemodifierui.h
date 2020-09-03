#ifndef EVENTSFROMRANGEMODIFIERUI_H
#define EVENTSFROMRANGEMODIFIERUI_H

#include <QStackedWidget>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class SpinBox;
class QGroupBox;

namespace UI {
	
class HkxObject;
class hkbEventsFromRangeModifier;
class hkbVariableBindingSet;
class EventRangeDataUI;

class EventsFromRangeModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    EventsFromRangeModifierUI();
    EventsFromRangeModifierUI& operator=(const EventsFromRangeModifierUI&) = delete;
    EventsFromRangeModifierUI(const EventsFromRangeModifierUI &) = delete;
    ~EventsFromRangeModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void eventRenamed(const QString & name, int index);
    void variableRenamed(const QString & name, int index);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setInputValue();
    void setLowerBound();
    void viewSelectedChild(int row, int column);
    void variableTableElementSelected(int index, const QString &name);
    void returnToWidget();
private:
    void addRange();
    void removeRange(int index);
    void loadDynamicTableRows();
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET{
        MAIN_WIDGET,
        CHILD_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbEventsFromRangeModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    EventRangeDataUI *rangeUI;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *inputValue;
    DoubleSpinBox *lowerBound;
};
}
#endif // EVENTSFROMRANGEMODIFIERUI_H
