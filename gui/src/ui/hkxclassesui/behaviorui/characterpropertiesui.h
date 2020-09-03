#ifndef CHARACTERPROPERTIESUI_H
#define CHARACTERPROPERTIESUI_H

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
class QTableWidgetItem;

namespace UI {

class HkxObject;
class hkbCharacterData;
class HkDataUI;
class BoneWeightArrayUI;

class CharacterPropertiesUI final: public QGroupBox
{
    Q_OBJECT
public:
    CharacterPropertiesUI(const QString & title);
    CharacterPropertiesUI& operator=(const CharacterPropertiesUI&) = delete;
    CharacterPropertiesUI(const CharacterPropertiesUI &) = delete;
    ~CharacterPropertiesUI() = default;
public:
    void setHkDataUI(HkDataUI *ui);
    void loadData(HkxObject *data);
private slots:
    void addVariable();
    void removeVariable();
    void setVariableValue(int type);
    void renameSelectedVariable(int type);
    void setVariableName(QTableWidgetItem *item);
    void viewVariable(int row, int column);
    void returnToTable();
signals:
    void variableNameChanged(const QString & newName, int index);
    void variableAdded(const QString & name);
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
    hkbCharacterData *loadedData;
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
#endif // CHARACTERPROPERTIESUI_H
