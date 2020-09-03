#ifndef BSBONESWITCHGENERATORUI_H
#define BSBONESWITCHGENERATORUI_H

#include <QStackedWidget>

#include "src/utility.h"

class TableWidget;
class QGridLayout;
class DoubleSpinBox;
class CheckBox;
class SpinBox;
class QPushButton;
class QGroupBox;
class LineEdit;
class BehaviorGraphView;
class ComboBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class hkbVariableBindingSet;
class BSBoneSwitchGeneratorBoneDataUI;
class BSBoneSwitchGenerator;

class BSBoneSwitchGeneratorUI final: public QStackedWidget
{
    Q_OBJECT
public:
    BSBoneSwitchGeneratorUI();
    BSBoneSwitchGeneratorUI& operator=(const BSBoneSwitchGeneratorUI&) = delete;
    BSBoneSwitchGeneratorUI(const BSBoneSwitchGeneratorUI &) = delete;
    ~BSBoneSwitchGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void viewSelectedChild(int row, int column);
    void returnToWidget(bool reloadData);
    void variableTableElementSelected(int index, const QString &name);
    void generatorTableElementSelected(int index, const QString &name);
    void swapGeneratorIndices(int index1, int index2);
private:
    void toggleSignals(bool toggleconnections);
    void setDefaultGenerator(int index, const QString & name);
    void addChildWithGenerator();
    void removeChild(int index);
    void loadDynamicTableRows();
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        CHILD_WIDGET
    };
    enum Generator_Type {
        STATE_MACHINE,
        MANUAL_SELECTOR_GENERATOR,
        BLENDER_GENERATOR,
        I_STATE_TAGGING_GENERATOR,
        BONE_SWITCH_GENERATOR,
        CYCLIC_BLEND_TRANSITION_GENERATOR,
        SYNCHRONIZED_CLIP_GENERATOR,
        MODIFIER_GENERATOR,
        OFFSET_ANIMATION_GENERATOR,
        POSE_MATCHING_GENERATOR,
        CLIP_GENERATOR,
        BEHAVIOR_REFERENCE_GENERATOR,
        GAMEBYRO_SEQUENCE_GENERATOR
    };
private:
    static const QStringList types;
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSBoneSwitchGenerator *bsData;
    QGroupBox *groupBox;
    BSBoneSwitchGeneratorBoneDataUI *childUI;
    QGridLayout *topLyt;
    ComboBox *typeSelectorCB;
    TableWidget *table;
    LineEdit *name;
};
}
#endif // BSBONESWITCHGENERATORUI_H
