#ifndef BSEVENTONDEACTIVATEMODIFIERUI_H
#define BSEVENTONDEACTIVATEMODIFIERUI_H

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
class QuadVariableWidget;

namespace UI {
	
class HkxObject;
class BSEventOnDeactivateModifier;
class hkbVariableBindingSet;

class BSEventOnDeactivateModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSEventOnDeactivateModifierUI();
    BSEventOnDeactivateModifierUI& operator=(const BSEventOnDeactivateModifierUI&) = delete;
    BSEventOnDeactivateModifierUI(const BSEventOnDeactivateModifierUI &) = delete;
    ~BSEventOnDeactivateModifierUI() = default;
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
    void setEventId(int index, const QString & name);
    void setEventPayload();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSEventOnDeactivateModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QLineEdit *eventPayload;
};
}
#endif // BSEVENTONDEACTIVATEMODIFIERUI_H
