#ifndef BLENDERGENERATORUI_H
#define BLENDERGENERATORUI_H

#include <QStackedWidget>

#include "src/utility.h"
#include "src/hkxclasses/behavior/generators/hkbblendergenerator.h"

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
class hkbBlenderGenerator;
class BlenderGeneratorChildUI;
class hkbVariableBindingSet;

class BlenderGeneratorUI final: public QStackedWidget
{
    Q_OBJECT
public:
    BlenderGeneratorUI();
    BlenderGeneratorUI& operator=(const BlenderGeneratorUI&) = delete;
    BlenderGeneratorUI(const BlenderGeneratorUI &) = delete;
    ~BlenderGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void setBehaviorView(BehaviorGraphView *view);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setReferencePoseWeightThreshold();
    void setBlendParameter();
    void setMinCyclicBlendParameter();
    void setMaxCyclicBlendParameter();
    void setIndexOfSyncMasterChild();
    void setFlagSync();
    void setFlagSmoothGeneratorWeights();
    void setFlagDontDeactivateChildrenWithZeroWeights();
    void setFlagParametricBlend();
    void setFlagIsParametricBlendCyclic();
    void setFlagForceDensePose();
    void setSubtractLastChild();
    void viewSelectedChild(int row, int column);
    void returnToWidget(bool reloadData);
    void variableTableElementSelected(int index, const QString &name);
    void generatorTableElementSelected(int index, const QString &name);
    void swapGeneratorIndices(int index1, int index2);
private:
    void toggleSignals(bool toggleconnections);
    void addChildWithGenerator();
    void removeChild(int index);
    void selectTableToView(bool viewproperties, const QString & path);
    void loadDynamicTableRows();
    void setBindingVariable(int index, const QString & name);
    void setFlag(CheckBox *flagcheckbox, hkbBlenderGenerator::BlenderFlag flagtoset);
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
    hkbBlenderGenerator *bsData;
    QGroupBox *groupBox;
    BlenderGeneratorChildUI *childUI;
    QGridLayout *topLyt;
    ComboBox *typeSelectorCB;
    TableWidget *table;
    LineEdit *name;
    DoubleSpinBox *referencePoseWeightThreshold;
    DoubleSpinBox *blendParameter;
    DoubleSpinBox *minCyclicBlendParameter;
    DoubleSpinBox *maxCyclicBlendParameter;
    SpinBox *indexOfSyncMasterChild;
    CheckBox *flagSync;
    CheckBox *flagSmoothGeneratorWeights;
    CheckBox *flagDontDeactivateChildrenWithZeroWeights;
    CheckBox *flagParametricBlend;
    CheckBox *flagIsParametricBlendCyclic;
    CheckBox *flagForceDensePose;
    CheckBox *subtractLastChild;
};
}
#endif // BLENDERGENERATORUI_H
