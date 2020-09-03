#ifndef BEHAVIORGRAPHUI_H
#define BEHAVIORGRAPHUI_H

#include <QGroupBox>


class QGridLayout;
class TableWidget;
class BehaviorGraphView;
class LineEdit;
class ComboBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class hkbBehaviorGraph;

class BehaviorGraphUI final: public QGroupBox
{
    Q_OBJECT
public:
    BehaviorGraphUI();
    BehaviorGraphUI& operator=(const BehaviorGraphUI&) = delete;
    BehaviorGraphUI(const BehaviorGraphUI &) = delete;
    ~BehaviorGraphUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *generators);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setVariableMode(int index);
    void viewSelectedChild(int row, int column);
    void setRootGenerator(int index, const QString &name);
private:
    void toggleSignals(bool toggleconnections);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    hkbBehaviorGraph *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    ComboBox *variableMode;
};
}
#endif // BEHAVIORGRAPHUI_H
