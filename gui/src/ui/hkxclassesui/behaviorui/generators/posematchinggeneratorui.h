#ifndef POSEMATCHINGGENERATORUI_H
#define POSEMATCHINGGENERATORUI_H

#include <QStackedWidget>

#include "src/utility.h"

#include "src/hkxclasses/behavior/generators/hkbposematchinggenerator.h"

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
class QuadVariableWidget;

namespace UI {

class hkbPoseMatchingGenerator;
class HkxObject;
class hkbVariableBindingSet;
class BlenderGeneratorChildUI;

class PoseMatchingGeneratorUI final: public QStackedWidget
{
    Q_OBJECT
public:
    PoseMatchingGeneratorUI();
    PoseMatchingGeneratorUI& operator=(const PoseMatchingGeneratorUI&) = delete;
    PoseMatchingGeneratorUI(const PoseMatchingGeneratorUI &) = delete;
    ~PoseMatchingGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
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
    void setWorldFromModelRotation();
    void setBlendSpeed();
    void setMinSpeedToSwitch();
    void setMinSwitchTimeNoError();
    void setMinSwitchTimeFullError();
    void setStartPlayingEventId(int index, const QString &name);
    void setStartMatchingEventId(int index, const QString &name);
    void setRootBoneIndex(int index);
    void setOtherBoneIndex(int index);
    void setAnotherBoneIndex(int index);
    void setPelvisIndex(int index);
    void setMode(int index);
    void viewSelectedChild(int row, int column);
    void returnToWidget(bool reloadData);
    void eventTableElementSelected(int index, const QString &name);
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
    void setFlag(CheckBox *flagcheckbox, hkbPoseMatchingGenerator::BlenderFlag flagtoset);
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
    hkbPoseMatchingGenerator *bsData;
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
    QuadVariableWidget *worldFromModelRotation;
    DoubleSpinBox *blendSpeed;
    DoubleSpinBox *minSpeedToSwitch;
    DoubleSpinBox *minSwitchTimeNoError;
    DoubleSpinBox *minSwitchTimeFullError;
    ComboBox *rootBoneIndex;
    ComboBox *otherBoneIndex;
    ComboBox *anotherBoneIndex;
    ComboBox *pelvisIndex;
    ComboBox *mode;
};
}
#endif // POSEMATCHINGGENERATORUI_H
