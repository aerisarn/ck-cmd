#ifndef BSEVENTEVERYNEVENTSMODIFIERUI_H
#define BSEVENTEVERYNEVENTSMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class QLineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class SpinBox;

namespace UI {

class HkxObject;
class BSEventEveryNEventsModifier;
class hkbVariableBindingSet;

class BSEventEveryNEventsModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSEventEveryNEventsModifierUI();
    BSEventEveryNEventsModifierUI& operator=(const BSEventEveryNEventsModifierUI&) = delete;
    BSEventEveryNEventsModifierUI(const BSEventEveryNEventsModifierUI &) = delete;
    ~BSEventEveryNEventsModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setEventToCheckForPayload();
    void setEventToSendPayload();
    void setNumberOfEventsBeforeSend();
    void setMinimumNumberOfEventsBeforeSend();
    void setRandomizeNumberOfEvents();
    void eventTableElementSelected(int index, const QString &name);
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void setEventToCheckForId(int index, const QString & name);
    void setEventToSendId(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSEventEveryNEventsModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QLineEdit *eventToCheckForPayload;
    QLineEdit *eventToSendPayload;
    SpinBox *numberOfEventsBeforeSend;
    SpinBox *minimumNumberOfEventsBeforeSend;
    CheckBox *randomizeNumberOfEvents;
};
}
#endif // BSEVENTEVERYNEVENTSMODIFIERUI_H
