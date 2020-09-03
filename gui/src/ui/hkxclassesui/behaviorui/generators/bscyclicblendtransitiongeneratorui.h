#ifndef BSCYCLICBLENDTRANSITIONGENERATORUI_H
#define BSCYCLICBLENDTRANSITIONGENERATORUI_H

#include <QStackedWidget>

#include "src/utility.h"

class TableWidget;
class QGridLayout;
class DoubleSpinBox;
class CheckButtonCombo;
class QPushButton;
class QGroupBox;
class LineEdit;
class BehaviorGraphView;
class ComboBox;
class GenericTableWidget;


namespace UI {

class HkxObject;
class hkbVariableBindingSet;
class EventUI;
class BSCyclicBlendTransitionGenerator;

class BSCyclicBlendTransitionGeneratorUI final: public QStackedWidget
{
    Q_OBJECT
public:
    BSCyclicBlendTransitionGeneratorUI();
    BSCyclicBlendTransitionGeneratorUI& operator=(const BSCyclicBlendTransitionGeneratorUI&) = delete;
    BSCyclicBlendTransitionGeneratorUI(const BSCyclicBlendTransitionGeneratorUI &) = delete;
    ~BSCyclicBlendTransitionGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setBlenderGenerator(int index, const QString & name);
    void setBlendParameter();
    void viewEventToFreezeBlendValue();
    void toggleEventToFreezeBlendValue(bool toggled);
    void viewEventToCrossBlend();
    void toggleEventToCrossBlend(bool toggled);
    void setTransitionDuration();
    void setBlendCurve(int index);
    void viewSelectedChild(int row, int column);
    void returnToWidget();
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        EVENT_WIDGET = 1
    };
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSCyclicBlendTransitionGenerator *bsData;
    QGroupBox *groupBox;
    EventUI *eventUI;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckButtonCombo *eventToFreezeBlendValue;
    CheckButtonCombo *eventToCrossBlend;
    DoubleSpinBox *fBlendParameter;
    DoubleSpinBox *fTransitionDuration;
    ComboBox *eBlendCurve;
};
}
#endif // BSCYCLICBLENDTRANSITIONGENERATORUI_H
