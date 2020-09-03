#ifndef HANDIKCONTROLSMODIFIERUI_H
#define HANDIKCONTROLSMODIFIERUI_H

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
class QuadVariableWidget;
class CheckButtonCombo;

namespace UI {
	
class HkxObject;
class hkbHandIkControlsModifier;
class hkbVariableBindingSet;
class HandUI;

class HandIkControlsModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    HandIkControlsModifierUI();
    HandIkControlsModifierUI& operator=(const HandIkControlsModifierUI&) = delete;
    HandIkControlsModifierUI(const HandIkControlsModifierUI &) = delete;
    ~HandIkControlsModifierUI() = default;
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
    void viewSelectedChild(int row, int column);
    void variableTableElementSelected(int index, const QString &name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void addHand();
    void removeHand(int index);
    void loadDynamicTableRows();
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        HAND_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbHandIkControlsModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    HandUI *handUI;
    LineEdit *name;
    CheckBox *enable;
};
}
#endif // HANDIKCONTROLSMODIFIERUI_H
