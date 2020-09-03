#ifndef EVALUATEEXPRESSIONMODIFIERUI_H
#define EVALUATEEXPRESSIONMODIFIERUI_H

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
class hkbEvaluateExpressionModifier;
class hkbVariableBindingSet;
class ExpressionDataArrayUI;

class EvaluateExpressionModifierUI final: public QStackedWidget
{
    Q_OBJECT
public:
    EvaluateExpressionModifierUI();
    EvaluateExpressionModifierUI& operator=(const EvaluateExpressionModifierUI&) = delete;
    EvaluateExpressionModifierUI(const EvaluateExpressionModifierUI &) = delete;
    ~EvaluateExpressionModifierUI() = default;
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
    void viewSelectedChild(int row, int column);
    void variableTableElementSelected(int index, const QString &name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void addExpression();
    void removeExpression(int index);
    void loadDynamicTableRows();
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        CHILD_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbEvaluateExpressionModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    ExpressionDataArrayUI *expressionUI;
    LineEdit *name;
    CheckBox *enable;
};
}
#endif // EVALUATEEXPRESSIONMODIFIERUI_H
