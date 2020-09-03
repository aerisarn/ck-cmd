#ifndef BSISTATETAGGINGGENERATORUI_H
#define BSISTATETAGGINGGENERATORUI_H

#include <QGroupBox>

#include "src/utility.h"

class BehaviorGraphView;
class TableWidget;
class LineEdit;
class QGridLayout;
class SpinBox;
class GenericTableWidget;


namespace UI {

class HkxObject;
class hkbVariableBindingSet;
class BSiStateTaggingGenerator;

class BSiStateTaggingGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSiStateTaggingGeneratorUI();
    BSiStateTaggingGeneratorUI& operator=(const BSiStateTaggingGeneratorUI&) = delete;
    BSiStateTaggingGeneratorUI(const BSiStateTaggingGeneratorUI &) = delete;
    ~BSiStateTaggingGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *generators);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void generatorNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setIStateToSetAs();
    void setIPriority();
    void setBindingVariable(int index, const QString & name);
    void viewSelected(int row, int column);
    void setDefaultGenerator(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSiStateTaggingGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    SpinBox *iStateToSetAs;
    SpinBox *iPriority;
};
}
#endif // BSISTATETAGGINGGENERATORUI_H
