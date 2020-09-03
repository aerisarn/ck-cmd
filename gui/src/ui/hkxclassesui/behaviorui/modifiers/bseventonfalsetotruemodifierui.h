#ifndef BSEVENTONFALSETOTRUEMODIFIERUI_H
#define BSEVENTONFALSETOTRUEMODIFIERUI_H

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
class hkbVariableBindingSet;
class BSEventOnFalseToTrueModifier;

class BSEventOnFalseToTrueModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSEventOnFalseToTrueModifierUI();
    BSEventOnFalseToTrueModifierUI& operator=(const BSEventOnFalseToTrueModifierUI&) = delete;
    BSEventOnFalseToTrueModifierUI(const BSEventOnFalseToTrueModifierUI &) = delete;
    ~BSEventOnFalseToTrueModifierUI() = default;
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
    void setEnableEvent1();
    void setVariableToTest1();
    void setEventToSend1Payload();
    void setEnableEvent2();
    void setVariableToTest2();
    void setEventToSend2Payload();
    void setEnableEvent3();
    void setVariableToTest3();
    void setEventToSend3Payload();
    void eventTableElementSelected(int index, const QString &name);
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void setEventToSend1Id(int index, const QString & name);
    void setEventToSend2Id(int index, const QString & name);
    void setEventToSend3Id(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSEventOnFalseToTrueModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    CheckBox *bEnableEvent1;
    CheckBox *bVariableToTest1;
    QLineEdit *eventToSend1Payload;
    CheckBox *bEnableEvent2;
    CheckBox *bVariableToTest2;
    QLineEdit *eventToSend2Payload;
    CheckBox *bEnableEvent3;
    CheckBox *bVariableToTest3;
    QLineEdit *eventToSend3Payload;
};
}
#endif // BSEVENTONFALSETOTRUEMODIFIERUI_H
