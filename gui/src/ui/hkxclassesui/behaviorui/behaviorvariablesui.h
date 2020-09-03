#ifndef BEHAVIORVARIABLESUI_H
#define BEHAVIORVARIABLESUI_H

#include <QGroupBox>


class QVBoxLayout;
class QHBoxLayout;
class QSignalMapper;
class TableWidget;
class QPushButton;
class CheckBox;
class ComboBox;
class LineEdit;
class QStackedLayout;
class SpinBox;
class DoubleSpinBox;
class QuadVariableWidget;

namespace UI {

class HkxObject;
class hkbBehaviorGraphData;
class HkDataUI;
class BoneWeightArrayUI;

class BehaviorVariablesUI final: public QGroupBox
{
    Q_OBJECT
public:
    BehaviorVariablesUI(const QString & title);
    BehaviorVariablesUI& operator=(const BehaviorVariablesUI&) = delete;
    BehaviorVariablesUI(const BehaviorVariablesUI &) = delete;
    ~BehaviorVariablesUI() = default;
public:
    void setHkDataUI(HkDataUI *ui);
    void loadData(HkxObject *data);
    void clear();
private slots:
    void addVariable();
    void removeVariable();
    void setVariableValue(int type);
    void renameSelectedVariable(int type);
    void viewVariable(int row, int column);
    void returnToTable();
signals:
    void variableNameChanged(const QString & newName, int index);
    void variableAdded(const QString & name, const QString & type);
    void variableRemoved(int index);
private:
    void loadVariable(CheckBox *variableWid);
    void loadVariable(SpinBox *variableWid);
    void loadVariable(DoubleSpinBox *variableWid);
    void loadVariable(QuadVariableWidget *variableWid);
    void addVariableToTable(const QString & name, const QString & type);
    void removeVariableFromTable(int row);
    void hideOtherVariables(int indexToView);
private:
    enum View {
        TABLE_WIDGET,
        VARIABLE_WIDGET,
        BONE_WEIGHTS_WIDGET
    };
    enum hkTypes {
        VARIABLE_TYPE_BOOL,
        VARIABLE_TYPE_INT32,
        VARIABLE_TYPE_REAL,
        VARIABLE_TYPE_POINTER,
        VARIABLE_TYPE_VECTOR4,
        VARIABLE_TYPE_QUATERNION
    };
private:
    static const QStringList types;
    static const QStringList headerLabels;
    HkDataUI *dataUI;
    QVBoxLayout *verLyt;
    hkbBehaviorGraphData *loadedData;
    TableWidget *table;
    QPushButton *addObjectPB;
    QPushButton *removeObjectPB;
    QHBoxLayout *buttonLyt;
    LineEdit *boolName;
    LineEdit *intName;
    LineEdit *doubleName;
    LineEdit *quadName;
    CheckBox *boolCB;
    SpinBox *intSB;
    DoubleSpinBox *doubleSB;
    QuadVariableWidget *quadWidget;
    TableWidget *variableWidget;
    BoneWeightArrayUI *boneWeightArrayWidget;
    QStackedLayout *stackLyt;
    QPushButton *returnBoolPB;
    QPushButton *returnIntPB;
    QPushButton *returnDoublePB;
    QPushButton *returnQuadPB;
    ComboBox *typeSelector;
    QSignalMapper *nameMapper;
    QSignalMapper *valueMapper;
};
}
#endif // BEHAVIORVARIABLESUI_H
