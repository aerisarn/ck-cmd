#ifndef BSOFFSETANIMATIONGENERATORUI_H
#define BSOFFSETANIMATIONGENERATORUI_H

#include <QGroupBox>

#include "src/utility.h"


class BehaviorGraphView;
class QGridLayout;
class TableWidget;
class LineEdit;
class GenericTableWidget;
class DoubleSpinBox;

namespace UI {
	
class HkxObject;
class BSOffsetAnimationGenerator;
class hkbVariableBindingSet;

class BSOffsetAnimationGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSOffsetAnimationGeneratorUI();
    BSOffsetAnimationGeneratorUI& operator=(const BSOffsetAnimationGeneratorUI&) = delete;
    BSOffsetAnimationGeneratorUI(const BSOffsetAnimationGeneratorUI &) = delete;
    ~BSOffsetAnimationGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties);
    void setBehaviorView(BehaviorGraphView *view);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setBindingVariable(int index, const QString & name);
    void setGenerator(int index, const QString & name);
    void setFOffsetVariable();
    void setFOffsetRangeStart();
    void setFOffsetRangeEnd();
    void viewSelected(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSOffsetAnimationGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    DoubleSpinBox *fOffsetVariable;
    DoubleSpinBox *fOffsetRangeStart;
    DoubleSpinBox *fOffsetRangeEnd;
};
}
#endif // BSOFFSETANIMATIONGENERATORUI_H
