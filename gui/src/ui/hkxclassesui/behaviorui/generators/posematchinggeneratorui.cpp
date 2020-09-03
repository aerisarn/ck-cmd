#include "posematchinggeneratorui.h"

#include "blendergeneratorchildui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbposematchinggenerator.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 26

#define NAME_ROW 0
#define REFERENCE_POSE_WEIGHT_THRESHOLD_ROW 1
#define BLEND_PARAMETER_ROW 2
#define MIN_CYCLIC_BLEND_PARAMETER_ROW 3
#define MAX_CYCLIC_BLEND_PARAMETER_ROW 4
#define INDEX_OF_SYNC_MASTER_CHILD_ROW 5
#define FLAG_SYNC_ROW 6
#define FLAG_SMOOTH_GENERATOR_WEIGHTS_ROW 7
#define FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS_ROW 8
#define FLAG_PARAMETRIC_BLEND_ROW 9
#define FLAG_IS_PARAMETRIC_BLEND_CYCLIC_ROW 10
#define FLAG_FORCE_DENSE_POSE_ROW 11
#define SUBTRACT_LAST_CHILD_ROW 12
#define WORLD_FROM_MODEL_ROTATION_ROW 13
#define BLEND_SPEED_ROW 14
#define MIN_SPEED_TO_SWITCH_ROW 15
#define MIN_SWITCH_TIME_NO_ERROR_ROW 16
#define MIN_SWITCH_TIME_FULL_ERROR_ROW 17
#define START_PLAYING_EVENT_ID_ROW 18
#define START_MATCHING_EVENT_ID_ROW 19
#define ROOT_BONE_INDEX_ROW 20
#define OTHER_BONE_INDEX_ROW 21
#define ANOTHER_BONE_INDEX_ROW 22
#define PELVIS_INDEX_ROW 23
#define MODE_ROW 24
#define ADD_CHILD_ROW 25

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList PoseMatchingGeneratorUI::types = {
    "hkbStateMachine",
    "hkbManualSelectorGenerator",
    "hkbBlenderGenerator",
    "BSiStateTaggingGenerator",
    "BSBoneSwitchGenerator",
    "BSCyclicBlendTransitionGenerator",
    "BSSynchronizedClipGenerator",
    "hkbModifierGenerator",
    "BSOffsetAnimationGenerator",
    "hkbPoseMatchingGenerator",
    "hkbClipGenerator",
    "hkbBehaviorReferenceGenerator",
    "BGSGamebryoSequenceGenerator"
};

const QStringList PoseMatchingGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

PoseMatchingGeneratorUI::PoseMatchingGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      groupBox(new QGroupBox("hkbBlenderGenerator")),
      childUI(new BlenderGeneratorChildUI),
      topLyt(new QGridLayout),
      typeSelectorCB(new ComboBox),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      referencePoseWeightThreshold(new DoubleSpinBox),
      blendParameter(new DoubleSpinBox),
      minCyclicBlendParameter(new DoubleSpinBox),
      maxCyclicBlendParameter(new DoubleSpinBox),
      indexOfSyncMasterChild(new SpinBox),
      flagSync(new CheckBox),
      flagSmoothGeneratorWeights(new CheckBox),
      flagDontDeactivateChildrenWithZeroWeights(new CheckBox),
      flagParametricBlend(new CheckBox),
      flagIsParametricBlendCyclic(new CheckBox),
      flagForceDensePose(new CheckBox),
      subtractLastChild(new CheckBox),
      worldFromModelRotation(new QuadVariableWidget),
      blendSpeed(new DoubleSpinBox),
      minSpeedToSwitch(new DoubleSpinBox),
      minSwitchTimeNoError(new DoubleSpinBox),
      minSwitchTimeFullError(new DoubleSpinBox),
      rootBoneIndex(new ComboBox),
      otherBoneIndex(new ComboBox),
      anotherBoneIndex(new ComboBox),
      pelvisIndex(new ComboBox),
      mode(new ComboBox)
{
    table->setAcceptDrops(true);
    table->viewport()->setAcceptDrops(true);
    table->setDragDropOverwriteMode(true);
    table->setDropIndicatorShown(true);
    table->setDragDropMode(QAbstractItemView::InternalMove);
    table->setRowSwapRange(BASE_NUMBER_OF_ROWS);
    typeSelectorCB->insertItems(0, types);
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, NAME_COLUMN, new TableWidgetItem("referencePoseWeightThreshold"));
    table->setItem(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, VALUE_COLUMN, referencePoseWeightThreshold);
    table->setItem(BLEND_PARAMETER_ROW, NAME_COLUMN, new TableWidgetItem("blendParameter"));
    table->setItem(BLEND_PARAMETER_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(BLEND_PARAMETER_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BLEND_PARAMETER_ROW, VALUE_COLUMN, blendParameter);
    table->setItem(MIN_CYCLIC_BLEND_PARAMETER_ROW, NAME_COLUMN, new TableWidgetItem("minCyclicBlendParameter"));
    table->setItem(MIN_CYCLIC_BLEND_PARAMETER_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MIN_CYCLIC_BLEND_PARAMETER_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MIN_CYCLIC_BLEND_PARAMETER_ROW, VALUE_COLUMN, minCyclicBlendParameter);
    table->setItem(MAX_CYCLIC_BLEND_PARAMETER_ROW, NAME_COLUMN, new TableWidgetItem("maxCyclicBlendParameter"));
    table->setItem(MAX_CYCLIC_BLEND_PARAMETER_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MAX_CYCLIC_BLEND_PARAMETER_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MAX_CYCLIC_BLEND_PARAMETER_ROW, VALUE_COLUMN, maxCyclicBlendParameter);
    table->setItem(INDEX_OF_SYNC_MASTER_CHILD_ROW, NAME_COLUMN, new TableWidgetItem("indexOfSyncMasterChild"));
    table->setItem(INDEX_OF_SYNC_MASTER_CHILD_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(INDEX_OF_SYNC_MASTER_CHILD_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INDEX_OF_SYNC_MASTER_CHILD_ROW, VALUE_COLUMN, indexOfSyncMasterChild);
    table->setItem(FLAG_SYNC_ROW, NAME_COLUMN, new TableWidgetItem("flagSync"));
    table->setItem(FLAG_SYNC_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_SYNC_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_SYNC_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_SYNC_ROW, VALUE_COLUMN, flagSync);
    table->setItem(FLAG_SMOOTH_GENERATOR_WEIGHTS_ROW, NAME_COLUMN, new TableWidgetItem("flagSmoothGeneratorWeights"));
    table->setItem(FLAG_SMOOTH_GENERATOR_WEIGHTS_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_SMOOTH_GENERATOR_WEIGHTS_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_SMOOTH_GENERATOR_WEIGHTS_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_SMOOTH_GENERATOR_WEIGHTS_ROW, VALUE_COLUMN, flagSmoothGeneratorWeights);
    table->setItem(FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS_ROW, NAME_COLUMN, new TableWidgetItem("flagDontDeactivateChildrenWithZeroWeights"));
    table->setItem(FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS_ROW, VALUE_COLUMN, flagDontDeactivateChildrenWithZeroWeights);
    table->setItem(FLAG_PARAMETRIC_BLEND_ROW, NAME_COLUMN, new TableWidgetItem("flagParametricBlend"));
    table->setItem(FLAG_PARAMETRIC_BLEND_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_PARAMETRIC_BLEND_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_PARAMETRIC_BLEND_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_PARAMETRIC_BLEND_ROW, VALUE_COLUMN, flagParametricBlend);
    table->setItem(FLAG_IS_PARAMETRIC_BLEND_CYCLIC_ROW, NAME_COLUMN, new TableWidgetItem("flagIsParametricBlendCyclic"));
    table->setItem(FLAG_IS_PARAMETRIC_BLEND_CYCLIC_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_IS_PARAMETRIC_BLEND_CYCLIC_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_IS_PARAMETRIC_BLEND_CYCLIC_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_IS_PARAMETRIC_BLEND_CYCLIC_ROW, VALUE_COLUMN, flagIsParametricBlendCyclic);
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, NAME_COLUMN, new TableWidgetItem("flagForceDensePose"));
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_FORCE_DENSE_POSE_ROW, VALUE_COLUMN, flagForceDensePose);
    table->setItem(SUBTRACT_LAST_CHILD_ROW, NAME_COLUMN, new TableWidgetItem("subtractLastChild"));
    table->setItem(SUBTRACT_LAST_CHILD_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(SUBTRACT_LAST_CHILD_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(SUBTRACT_LAST_CHILD_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(SUBTRACT_LAST_CHILD_ROW, VALUE_COLUMN, subtractLastChild);
    table->setItem(WORLD_FROM_MODEL_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("worldFromModelRotation"));
    table->setItem(WORLD_FROM_MODEL_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(WORLD_FROM_MODEL_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(WORLD_FROM_MODEL_ROTATION_ROW, VALUE_COLUMN, worldFromModelRotation);
    table->setItem(BLEND_SPEED_ROW, NAME_COLUMN, new TableWidgetItem("blendSpeed"));
    table->setItem(BLEND_SPEED_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(BLEND_SPEED_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BLEND_SPEED_ROW, VALUE_COLUMN, blendSpeed);
    table->setItem(MIN_SPEED_TO_SWITCH_ROW, NAME_COLUMN, new TableWidgetItem("minSpeedToSwitch"));
    table->setItem(MIN_SPEED_TO_SWITCH_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MIN_SPEED_TO_SWITCH_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MIN_SPEED_TO_SWITCH_ROW, VALUE_COLUMN, minSpeedToSwitch);
    table->setItem(MIN_SWITCH_TIME_NO_ERROR_ROW, NAME_COLUMN, new TableWidgetItem("minSwitchTimeNoError"));
    table->setItem(MIN_SWITCH_TIME_NO_ERROR_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MIN_SWITCH_TIME_NO_ERROR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MIN_SWITCH_TIME_NO_ERROR_ROW, VALUE_COLUMN, minSwitchTimeNoError);
    table->setItem(MIN_SWITCH_TIME_FULL_ERROR_ROW, NAME_COLUMN, new TableWidgetItem("minSwitchTimeFullError"));
    table->setItem(MIN_SWITCH_TIME_FULL_ERROR_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MIN_SWITCH_TIME_FULL_ERROR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MIN_SWITCH_TIME_FULL_ERROR_ROW, VALUE_COLUMN, minSwitchTimeFullError);
    table->setItem(START_PLAYING_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("startPlayingEventId"));
    table->setItem(START_PLAYING_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(START_PLAYING_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(START_PLAYING_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(START_MATCHING_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("startMatchingEventId"));
    table->setItem(START_MATCHING_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(START_MATCHING_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(START_MATCHING_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(ROOT_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("rootBoneIndex"));
    table->setItem(ROOT_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ROOT_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROOT_BONE_INDEX_ROW, VALUE_COLUMN, rootBoneIndex);
    table->setItem(OTHER_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("otherBoneIndex"));
    table->setItem(OTHER_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(OTHER_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OTHER_BONE_INDEX_ROW, VALUE_COLUMN, otherBoneIndex);
    table->setItem(ANOTHER_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("anotherBoneIndex"));
    table->setItem(ANOTHER_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ANOTHER_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANOTHER_BONE_INDEX_ROW, VALUE_COLUMN, anotherBoneIndex);
    table->setItem(PELVIS_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("pelvisIndex"));
    table->setItem(PELVIS_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(PELVIS_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(PELVIS_INDEX_ROW, VALUE_COLUMN, pelvisIndex);
    table->setItem(MODE_ROW, NAME_COLUMN, new TableWidgetItem("mode"));
    table->setItem(MODE_ROW, TYPE_COLUMN, new TableWidgetItem("Mode", Qt::AlignCenter));
    table->setItem(MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(MODE_ROW, VALUE_COLUMN, mode);
    table->setItem(ADD_CHILD_ROW, NAME_COLUMN, new TableWidgetItem("Add Child With Generator", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new blender generator child with a generator of the type specified in the adjacent combo box"));
    table->setCellWidget(ADD_CHILD_ROW, TYPE_COLUMN, typeSelectorCB);
    table->setItem(ADD_CHILD_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Child", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected blender generator child"));
    table->setItem(ADD_CHILD_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Child", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected blender generator child"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(childUI);
    toggleSignals(true);
}

void PoseMatchingGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(referencePoseWeightThreshold, SIGNAL(editingFinished()), this, SLOT(setReferencePoseWeightThreshold()), Qt::UniqueConnection);
        connect(blendParameter, SIGNAL(editingFinished()), this, SLOT(setBlendParameter()), Qt::UniqueConnection);
        connect(minCyclicBlendParameter, SIGNAL(editingFinished()), this, SLOT(setMinCyclicBlendParameter()), Qt::UniqueConnection);
        connect(maxCyclicBlendParameter, SIGNAL(editingFinished()), this, SLOT(setMaxCyclicBlendParameter()), Qt::UniqueConnection);
        connect(indexOfSyncMasterChild, SIGNAL(editingFinished()), this, SLOT(setIndexOfSyncMasterChild()), Qt::UniqueConnection);
        connect(flagSync, SIGNAL(released()), this, SLOT(setFlagSync()), Qt::UniqueConnection);
        connect(flagSmoothGeneratorWeights, SIGNAL(released()), this, SLOT(setFlagSmoothGeneratorWeights()), Qt::UniqueConnection);
        connect(flagDontDeactivateChildrenWithZeroWeights, SIGNAL(released()), this, SLOT(setFlagDontDeactivateChildrenWithZeroWeights()), Qt::UniqueConnection);
        connect(flagParametricBlend, SIGNAL(released()), this, SLOT(setFlagParametricBlend()), Qt::UniqueConnection);
        connect(flagIsParametricBlendCyclic, SIGNAL(released()), this, SLOT(setFlagIsParametricBlendCyclic()), Qt::UniqueConnection);
        connect(flagForceDensePose, SIGNAL(released()), this, SLOT(setFlagForceDensePose()), Qt::UniqueConnection);
        connect(subtractLastChild, SIGNAL(released()), this, SLOT(setSubtractLastChild()), Qt::UniqueConnection);
        connect(worldFromModelRotation, SIGNAL(editingFinished()), this, SLOT(setWorldFromModelRotation()), Qt::UniqueConnection);
        connect(blendSpeed, SIGNAL(editingFinished()), this, SLOT(setBlendSpeed()), Qt::UniqueConnection);
        connect(minSpeedToSwitch, SIGNAL(editingFinished()), this, SLOT(setMinSpeedToSwitch()), Qt::UniqueConnection);
        connect(minSwitchTimeNoError, SIGNAL(editingFinished()), this, SLOT(setMinSwitchTimeNoError()), Qt::UniqueConnection);
        connect(minSwitchTimeFullError, SIGNAL(editingFinished()), this, SLOT(setMinSwitchTimeFullError()), Qt::UniqueConnection);
        connect(rootBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setRootBoneIndex(int)), Qt::UniqueConnection);
        connect(otherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setOtherBoneIndex(int)), Qt::UniqueConnection);
        connect(anotherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnotherBoneIndex(int)), Qt::UniqueConnection);
        connect(pelvisIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setPelvisIndex(int)), Qt::UniqueConnection);
        connect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(setMode(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(referencePoseWeightThreshold, SIGNAL(editingFinished()), this, SLOT(setReferencePoseWeightThreshold()));
        disconnect(blendParameter, SIGNAL(editingFinished()), this, SLOT(setBlendParameter()));
        disconnect(minCyclicBlendParameter, SIGNAL(editingFinished()), this, SLOT(setMinCyclicBlendParameter()));
        disconnect(maxCyclicBlendParameter, SIGNAL(editingFinished()), this, SLOT(setMaxCyclicBlendParameter()));
        disconnect(indexOfSyncMasterChild, SIGNAL(editingFinished()), this, SLOT(setIndexOfSyncMasterChild()));
        disconnect(flagSync, SIGNAL(released()), this, SLOT(setFlagSync()));
        disconnect(flagSmoothGeneratorWeights, SIGNAL(released()), this, SLOT(setFlagSmoothGeneratorWeights()));
        disconnect(flagDontDeactivateChildrenWithZeroWeights, SIGNAL(released()), this, SLOT(setFlagDontDeactivateChildrenWithZeroWeights()));
        disconnect(flagParametricBlend, SIGNAL(released()), this, SLOT(setFlagParametricBlend()));
        disconnect(flagIsParametricBlendCyclic, SIGNAL(released()), this, SLOT(setFlagIsParametricBlendCyclic()));
        disconnect(flagForceDensePose, SIGNAL(released()), this, SLOT(setFlagForceDensePose()));
        disconnect(subtractLastChild, SIGNAL(released()), this, SLOT(setSubtractLastChild()));
        disconnect(worldFromModelRotation, SIGNAL(editingFinished()), this, SLOT(setWorldFromModelRotation()));
        disconnect(blendSpeed, SIGNAL(editingFinished()), this, SLOT(setBlendSpeed()));
        disconnect(minSpeedToSwitch, SIGNAL(editingFinished()), this, SLOT(setMinSpeedToSwitch()));
        disconnect(minSwitchTimeNoError, SIGNAL(editingFinished()), this, SLOT(setMinSwitchTimeNoError()));
        disconnect(minSwitchTimeFullError, SIGNAL(editingFinished()), this, SLOT(setMinSwitchTimeFullError()));
        disconnect(rootBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setRootBoneIndex(int)));
        disconnect(otherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setOtherBoneIndex(int)));
        disconnect(anotherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnotherBoneIndex(int)));
        disconnect(pelvisIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setPelvisIndex(int)));
        disconnect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(setMode(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)));
        disconnect(childUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)));
        disconnect(childUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(childUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(childUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)));
    }
}

void PoseMatchingGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_POSE_MATCHING_GENERATOR){
            bsData = static_cast<hkbPoseMatchingGenerator *>(data);
            name->setText(bsData->getName());
            referencePoseWeightThreshold->setValue(bsData->getReferencePoseWeightThreshold());
            blendParameter->setValue(bsData->getBlendParameter());
            minCyclicBlendParameter->setValue(bsData->getMinCyclicBlendParameter());
            maxCyclicBlendParameter->setValue(bsData->getMaxCyclicBlendParameter());
            indexOfSyncMasterChild->setValue(bsData->getIndexOfSyncMasterChild());
            auto ok = true;
            hkbPoseMatchingGenerator::BlenderFlags flags(bsData->flags.toInt(&ok));
            auto testflag = [&](CheckBox * checkbox, hkbPoseMatchingGenerator::BlenderFlag flagtotest){
                (flags.testFlag(flagtotest)) ? checkbox->setChecked(true) : checkbox->setChecked(false);
            };
            if (ok){
                testflag(flagSync, hkbPoseMatchingGenerator::FLAG_SYNC);
                testflag(flagSmoothGeneratorWeights, hkbPoseMatchingGenerator::FLAG_SMOOTH_GENERATOR_WEIGHTS);
                testflag(flagDontDeactivateChildrenWithZeroWeights, hkbPoseMatchingGenerator::FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS);
                testflag(flagParametricBlend, hkbPoseMatchingGenerator::FLAG_PARAMETRIC_BLEND);
                testflag(flagIsParametricBlendCyclic, hkbPoseMatchingGenerator::FLAG_IS_PARAMETRIC_BLEND_CYCLIC);
                testflag(flagForceDensePose, hkbPoseMatchingGenerator::FLAG_FORCE_DENSE_POSE);
            }else{
                LogFile::writeToLog(QString("PoseMatchingGeneratorUI::loadData(): The flags string is invalid!!!\nString: "+bsData->flags).toLocal8Bit().data());
            }
            subtractLastChild->setChecked(bsData->getSubtractLastChild());
            worldFromModelRotation->setValue(bsData->getWorldFromModelRotation());
            blendSpeed->setValue(bsData->getBlendSpeed());
            minSpeedToSwitch->setValue(bsData->getMinSpeedToSwitch());
            minSwitchTimeNoError->setValue(bsData->getMinSwitchTimeNoError());
            minSwitchTimeFullError->setValue(bsData->getMinSwitchTimeFullError());
            auto eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getStartPlayingEventId());
            auto labeleventnamme = [&](int row, const QString & name){
                (name != "") ? table->item(row, VALUE_COLUMN)->setText(name) : table->item(row, VALUE_COLUMN)->setText("None");
            };
            labeleventnamme(START_PLAYING_EVENT_ID_ROW, eventname);
            eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getStartMatchingEventId());
            labeleventnamme(START_MATCHING_EVENT_ID_ROW, eventname);
            auto fillbonelists = [&](ComboBox *combobox, int boneindex){
                QStringList boneNames("None");
                if (!combobox->count()){
                    boneNames = boneNames + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(boneindex);
            };
            fillbonelists(rootBoneIndex, bsData->getRootBoneIndex() + 1);
            fillbonelists(otherBoneIndex, bsData->getOtherBoneIndex() + 1);
            fillbonelists(anotherBoneIndex, bsData->getAnotherBoneIndex() + 1);
            fillbonelists(pelvisIndex, bsData->getPelvisIndex() + 1);
            (!mode->count()) ? mode->insertItems(0, bsData->Mode) : NULL;
            mode->setCurrentIndex(bsData->Mode.indexOf(bsData->mode));
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, BINDING_COLUMN, varBind, "referencePoseWeightThreshold", table, bsData);
            UIHelper::loadBinding(BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "blendParameter", table, bsData);
            UIHelper::loadBinding(MIN_CYCLIC_BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "minCyclicBlendParameter", table, bsData);
            UIHelper::loadBinding(MAX_CYCLIC_BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "maxCyclicBlendParameter", table, bsData);
            UIHelper::loadBinding(INDEX_OF_SYNC_MASTER_CHILD_ROW, BINDING_COLUMN, varBind, "indexOfSyncMasterChild", table, bsData);
            UIHelper::loadBinding(SUBTRACT_LAST_CHILD_ROW, BINDING_COLUMN, varBind, "subtractLastChild", table, bsData);
            UIHelper::loadBinding(WORLD_FROM_MODEL_ROTATION_ROW, BINDING_COLUMN, varBind, "worldFromModelRotation", table, bsData);
            UIHelper::loadBinding(BLEND_SPEED_ROW, BINDING_COLUMN, varBind, "blendSpeed", table, bsData);
            UIHelper::loadBinding(MIN_SPEED_TO_SWITCH_ROW, BINDING_COLUMN, varBind, "minSpeedToSwitch", table, bsData);
            UIHelper::loadBinding(MIN_SWITCH_TIME_NO_ERROR_ROW, BINDING_COLUMN, varBind, "minSwitchTimeNoError", table, bsData);
            UIHelper::loadBinding(MIN_SWITCH_TIME_FULL_ERROR_ROW, BINDING_COLUMN, varBind, "minSwitchTimeFullError", table, bsData);
            UIHelper::loadBinding(ROOT_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "rootBoneIndex", table, bsData);
            UIHelper::loadBinding(OTHER_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "otherBoneIndex", table, bsData);
            UIHelper::loadBinding(ANOTHER_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "anotherBoneIndex", table, bsData);
            UIHelper::loadBinding(PELVIS_INDEX_ROW, BINDING_COLUMN, varBind, "pelvisIndex", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("PoseMatchingGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void PoseMatchingGeneratorUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_CHILD_ROW + bsData->getNumberOfChildren() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_CHILD_ROW + 1, j = 0; j < bsData->getNumberOfChildren(); i++, j++){
            auto child = static_cast<hkbBlenderGeneratorChild *>(bsData->children.at(j).data());
            if (child){
                UIHelper::setRowItems(i, "Child "+QString::number(j), child->getClassname(), "Remove", "Edit", "Double click to remove this child", "Double click to edit this child", table);
            }else{
                LogFile::writeToLog("PoseMatchingGeneratorUI::loadData(): Null child found!!!");
            }
        }
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case REFERENCE_POSE_WEIGHT_THRESHOLD_ROW:
            checkisproperty(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, "referencePoseWeightThreshold", VARIABLE_TYPE_REAL); break;
        case BLEND_PARAMETER_ROW:
            checkisproperty(BLEND_PARAMETER_ROW, "blendParameter", VARIABLE_TYPE_REAL); break;
        case MIN_CYCLIC_BLEND_PARAMETER_ROW:
            checkisproperty(MIN_CYCLIC_BLEND_PARAMETER_ROW, "minCyclicBlendParameter", VARIABLE_TYPE_REAL); break;
        case MAX_CYCLIC_BLEND_PARAMETER_ROW:
            checkisproperty(MAX_CYCLIC_BLEND_PARAMETER_ROW, "maxCyclicBlendParameter", VARIABLE_TYPE_REAL); break;
        case INDEX_OF_SYNC_MASTER_CHILD_ROW:
            checkisproperty(INDEX_OF_SYNC_MASTER_CHILD_ROW, "indexOfSyncMasterChild", VARIABLE_TYPE_INT32); break;
        case SUBTRACT_LAST_CHILD_ROW:
            checkisproperty(SUBTRACT_LAST_CHILD_ROW, "subtractLastChild", VARIABLE_TYPE_BOOL); break;
        case WORLD_FROM_MODEL_ROTATION_ROW:
            checkisproperty(WORLD_FROM_MODEL_ROTATION_ROW, "worldFromModelRotation", VARIABLE_TYPE_VECTOR4); break;
        case BLEND_SPEED_ROW:
            checkisproperty(BLEND_SPEED_ROW, "blendSpeed", VARIABLE_TYPE_REAL); break;
        case MIN_SPEED_TO_SWITCH_ROW:
            checkisproperty(MIN_SPEED_TO_SWITCH_ROW, "minSpeedToSwitch", VARIABLE_TYPE_REAL); break;
        case MIN_SWITCH_TIME_NO_ERROR_ROW:
            checkisproperty(MIN_SWITCH_TIME_NO_ERROR_ROW, "minSwitchTimeNoError", VARIABLE_TYPE_REAL); break;
        case MIN_SWITCH_TIME_FULL_ERROR_ROW:
            checkisproperty(MIN_SWITCH_TIME_FULL_ERROR_ROW, "minSwitchTimeFullError", VARIABLE_TYPE_REAL); break;
        case ROOT_BONE_INDEX_ROW:
            checkisproperty(ROOT_BONE_INDEX_ROW, "rootBoneIndex", VARIABLE_TYPE_INT32); break;
        case OTHER_BONE_INDEX_ROW:
            checkisproperty(OTHER_BONE_INDEX_ROW, "otherBoneIndex", VARIABLE_TYPE_INT32); break;
        case ANOTHER_BONE_INDEX_ROW:
            checkisproperty(ANOTHER_BONE_INDEX_ROW, "anotherBoneIndex", VARIABLE_TYPE_INT32); break;
        case PELVIS_INDEX_ROW:
            checkisproperty(PELVIS_INDEX_ROW, "pelvisIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::setBindingVariable(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        bsData->updateChildIconNames();
        emit generatorNameChanged(bsData->getName(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::setName(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::setReferencePoseWeightThreshold(){
    (bsData) ? bsData->setReferencePoseWeightThreshold(referencePoseWeightThreshold->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setReferencePoseWeightThreshold(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setBlendParameter(){
    (bsData) ? bsData->setBlendParameter(blendParameter->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setBlendParameter(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setMinCyclicBlendParameter(){
    (bsData) ? bsData->setMinCyclicBlendParameter(minCyclicBlendParameter->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setMinCyclicBlendParameter(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setMaxCyclicBlendParameter(){
    (bsData) ? bsData->setMaxCyclicBlendParameter(maxCyclicBlendParameter->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setMaxCyclicBlendParameter(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setIndexOfSyncMasterChild(){
    (bsData) ? bsData->setIndexOfSyncMasterChild(indexOfSyncMasterChild->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setIndexOfSyncMasterChild(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setFlag(CheckBox *flagcheckbox, hkbPoseMatchingGenerator::BlenderFlag flagtoset){
    if (bsData){
        auto ok = true;
        hkbPoseMatchingGenerator::BlenderFlags flags(bsData->getFlags().toInt(&ok));
        if (ok){
            (flagcheckbox->isChecked()) ? flags |= flagtoset : flags &= ~(flagtoset);
            bsData->setFlags(QString::number(flags));
        }else{
            LogFile::writeToLog(QString("PoseMatchingGeneratorUI::setFlag(): The flags string is invalid!!!\nString: "+bsData->getFlags()).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::setFlag(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::setFlagSync(){
    setFlag(flagSync, hkbPoseMatchingGenerator::FLAG_SYNC);
}

void PoseMatchingGeneratorUI::setFlagSmoothGeneratorWeights(){
    setFlag(flagSmoothGeneratorWeights, hkbPoseMatchingGenerator::FLAG_SMOOTH_GENERATOR_WEIGHTS);
}

void PoseMatchingGeneratorUI::setFlagDontDeactivateChildrenWithZeroWeights(){
    setFlag(flagDontDeactivateChildrenWithZeroWeights, hkbPoseMatchingGenerator::FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS);
}

void PoseMatchingGeneratorUI::setFlagParametricBlend(){
    setFlag(flagParametricBlend, hkbPoseMatchingGenerator::FLAG_PARAMETRIC_BLEND);
}

void PoseMatchingGeneratorUI::setFlagIsParametricBlendCyclic(){
    setFlag(flagIsParametricBlendCyclic, hkbPoseMatchingGenerator::FLAG_IS_PARAMETRIC_BLEND_CYCLIC);
}

void PoseMatchingGeneratorUI::setFlagForceDensePose(){
    setFlag(flagForceDensePose, hkbPoseMatchingGenerator::FLAG_FORCE_DENSE_POSE);
}

void PoseMatchingGeneratorUI::setSubtractLastChild(){
    (bsData) ? bsData->setSubtractLastChild(subtractLastChild->isChecked()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setSubtractLastChild(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setWorldFromModelRotation(){
    (bsData) ? bsData->setWorldFromModelRotation(worldFromModelRotation->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setWorldFromModelRotation(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setBlendSpeed(){
    (bsData) ? bsData->setBlendSpeed(blendSpeed->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setBlendSpeed(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setMinSpeedToSwitch(){
    (bsData) ? bsData->setMinSpeedToSwitch(minSpeedToSwitch->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setMinSpeedToSwitch(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setMinSwitchTimeNoError(){
    (bsData) ? bsData->setMinSwitchTimeNoError(minSwitchTimeNoError->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setMinSwitchTimeNoError(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setMinSwitchTimeFullError(){
    (bsData) ? bsData->setMinSwitchTimeFullError(minSwitchTimeFullError->value()) : LogFile::writeToLog("PoseMatchingGeneratorUI::setMinSwitchTimeFullError(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setStartPlayingEventId(int index, const QString &name){
    (bsData) ? bsData->setStartPlayingEventId(index), table->item(START_PLAYING_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("PoseMatchingGeneratorUI::setStartPlayingEventId(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setStartMatchingEventId(int index, const QString &name){
    (bsData) ? bsData->setStartMatchingEventId(index), table->item(START_MATCHING_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("PoseMatchingGeneratorUI::setStartMatchingEventId(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setRootBoneIndex(int index){
    (bsData) ? bsData->setRootBoneIndex(index - 1) : LogFile::writeToLog("PoseMatchingGeneratorUI::setRootBoneIndex(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setOtherBoneIndex(int index){
    (bsData) ? bsData->setOtherBoneIndex(index - 1) : LogFile::writeToLog("PoseMatchingGeneratorUI::setOtherBoneIndex(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setAnotherBoneIndex(int index){
    (bsData) ? bsData->setAnotherBoneIndex(index - 1) : LogFile::writeToLog("PoseMatchingGeneratorUI::setAnotherBoneIndex(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setPelvisIndex(int index){
    (bsData) ? bsData->setPelvisIndex(index - 1) : LogFile::writeToLog("PoseMatchingGeneratorUI::setPelvisIndex(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::setMode(int index){
    (bsData) ? bsData->setMode(index) : LogFile::writeToLog("PoseMatchingGeneratorUI::setMode(): The data is nullptr!!");
}

void PoseMatchingGeneratorUI::swapGeneratorIndices(int index1, int index2){
    if (bsData){
        index1 = index1 - BASE_NUMBER_OF_ROWS;
        index2 = index2 - BASE_NUMBER_OF_ROWS;
        if (!bsData->swapChildren(index1, index2)){
            WARNING_MESSAGE("Cannot swap these rows!!");
        }else{  //TO DO: check if necessary...
            (behaviorView->getSelectedItem()) ? behaviorView->getSelectedItem()->reorderChildren() : LogFile::writeToLog("BlenderGeneratorUI::swapGeneratorIndices(): No item selected!!");
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::swapGeneratorIndices(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::addChildWithGenerator(){
    if (bsData && behaviorView){
        auto typeEnum = static_cast<Generator_Type>(typeSelectorCB->currentIndex());
        behaviorView->appendBlenderGeneratorChild();
        switch (typeEnum){
        case STATE_MACHINE:
            behaviorView->appendStateMachine(); break;
        case MANUAL_SELECTOR_GENERATOR:
            behaviorView->appendManualSelectorGenerator(); break;
        case BLENDER_GENERATOR:
            behaviorView->appendBlenderGenerator(); break;
        case I_STATE_TAGGING_GENERATOR:
            behaviorView->appendIStateTaggingGenerator(); break;
        case BONE_SWITCH_GENERATOR:
            behaviorView->appendBoneSwitchGenerator(); break;
        case CYCLIC_BLEND_TRANSITION_GENERATOR:
            behaviorView->appendCyclicBlendTransitionGenerator(); break;
        case SYNCHRONIZED_CLIP_GENERATOR:
            behaviorView->appendSynchronizedClipGenerator(); break;
        case MODIFIER_GENERATOR:
            behaviorView->appendModifierGenerator(); break;
        case OFFSET_ANIMATION_GENERATOR:
            behaviorView->appendOffsetAnimationGenerator(); break;
        case POSE_MATCHING_GENERATOR:
            behaviorView->appendPoseMatchingGenerator(); break;
        case CLIP_GENERATOR:
            behaviorView->appendClipGenerator(); break;
        case BEHAVIOR_REFERENCE_GENERATOR:
            behaviorView->appendBehaviorReferenceGenerator(); break;
        case GAMEBYRO_SEQUENCE_GENERATOR:
            behaviorView->appendBGSGamebryoSequenceGenerator(); break;
        default:
            LogFile::writeToLog("PoseMatchingGeneratorUI::addChild(): Invalid typeEnum!!");
            return;
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::addChild(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::removeChild(int index){
    if (bsData && behaviorView){
        auto child = bsData->getChildDataAt(index);
        if (child){
            behaviorView->removeItemFromGraph(behaviorView->getSelectedIconsChildIcon(child->getChildren().first()), index);
            behaviorView->removeObjects();
        }else{
            WARNING_MESSAGE("Invalid index of child to remove!!");
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::removeChild(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_CHILD_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case REFERENCE_POSE_WEIGHT_THRESHOLD_ROW:
                    checkisproperty(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, "referencePoseWeightThreshold"); break;
                case BLEND_PARAMETER_ROW:
                    checkisproperty(BLEND_PARAMETER_ROW, "blendParameter"); break;
                case MIN_CYCLIC_BLEND_PARAMETER_ROW:
                    checkisproperty(MIN_CYCLIC_BLEND_PARAMETER_ROW, "minCyclicBlendParameter"); break;
                case MAX_CYCLIC_BLEND_PARAMETER_ROW:
                    checkisproperty(MAX_CYCLIC_BLEND_PARAMETER_ROW, "maxCyclicBlendParameter"); break;
                case INDEX_OF_SYNC_MASTER_CHILD_ROW:
                    checkisproperty(INDEX_OF_SYNC_MASTER_CHILD_ROW, "indexOfSyncMasterChild"); break;
                case SUBTRACT_LAST_CHILD_ROW:
                    checkisproperty(SUBTRACT_LAST_CHILD_ROW, "subtractLastChild"); break;
                case WORLD_FROM_MODEL_ROTATION_ROW:
                    checkisproperty(WORLD_FROM_MODEL_ROTATION_ROW, "worldFromModelRotation"); break;
                case BLEND_SPEED_ROW:
                    checkisproperty(BLEND_SPEED_ROW, "blendSpeed"); break;
                case MIN_SPEED_TO_SWITCH_ROW:
                    checkisproperty(MIN_SPEED_TO_SWITCH_ROW, "minSpeedToSwitch"); break;
                case MIN_SWITCH_TIME_NO_ERROR_ROW:
                    checkisproperty(MIN_SWITCH_TIME_NO_ERROR_ROW, "minSwitchTimeNoError"); break;
                case MIN_SWITCH_TIME_FULL_ERROR_ROW:
                    checkisproperty(MIN_SWITCH_TIME_FULL_ERROR_ROW, "minSwitchTimeFullError"); break;
                case ROOT_BONE_INDEX_ROW:
                    checkisproperty(ROOT_BONE_INDEX_ROW, "rootBoneIndex"); break;
                case OTHER_BONE_INDEX_ROW:
                    checkisproperty(OTHER_BONE_INDEX_ROW, "otherBoneIndex"); break;
                case ANOTHER_BONE_INDEX_ROW:
                    checkisproperty(ANOTHER_BONE_INDEX_ROW, "anotherBoneIndex"); break;
                case PELVIS_INDEX_ROW:
                    checkisproperty(PELVIS_INDEX_ROW, "pelvisIndex"); break;
                }
            }else if (column == VALUE_COLUMN){
                if (row == START_PLAYING_EVENT_ID_ROW){
                    emit viewEvents(bsData->startPlayingEventId + 1, QString(), QStringList());
                }else if (row == START_MATCHING_EVENT_ID_ROW){
                    emit viewEvents(bsData->startMatchingEventId + 1, QString(), QStringList());
                }
            }
        }else if (row == ADD_CHILD_ROW && column == NAME_COLUMN){
            addChildWithGenerator();
        }else if (row > ADD_CHILD_ROW && row < ADD_CHILD_ROW + bsData->getNumberOfChildren() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->children.size() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    childUI->loadData(static_cast<hkbBlenderGeneratorChild *>(bsData->children.at(result).data()), result);
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the child \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeChild(result);
                    }
                }
            }else{
                LogFile::writeToLog("PoseMatchingGeneratorUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::returnToWidget(bool reloadData){
    (reloadData) ? loadDynamicTableRows() : NULL;
    setCurrentIndex(MAIN_WIDGET);
}

void PoseMatchingGeneratorUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (index == bsData->startPlayingEventId) ? table->item(START_PLAYING_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
        (index == bsData->startMatchingEventId) ? table->item(START_MATCHING_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::eventRenamed(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::eventTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        switch (table->currentRow()){
        case START_PLAYING_EVENT_ID_ROW:
            setStartPlayingEventId(index, name); break;
        case START_MATCHING_EVENT_ID_ROW:
            setStartMatchingEventId(index, name); break;
        default:
            WARNING_MESSAGE("PoseMatchingGeneratorUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
            return;
        }
        break;
    default:
        WARNING_MESSAGE("PoseMatchingGeneratorUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void PoseMatchingGeneratorUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case CHILD_WIDGET:
        childUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("PoseMatchingGeneratorUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void PoseMatchingGeneratorUI::generatorTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case CHILD_WIDGET:
        childUI->setGenerator(index, name); break;
    default:
        WARNING_MESSAGE("PoseMatchingGeneratorUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void PoseMatchingGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (generators && variables && events && properties){
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(eventTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(generatorTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void PoseMatchingGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData){
        if (viewproperties){
            if (bsData->getVariableBindingSetData()){
                emit viewProperties(bsData->getVariableBindingSetData()->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (bsData->getVariableBindingSetData()){
                emit viewVariables(bsData->getVariableBindingSetData()->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        if (name != ""){
            index--;
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto setname = [&](const QString & fieldname, int row){
                    auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                    (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
                };
                setname("referencePoseWeightThreshold", REFERENCE_POSE_WEIGHT_THRESHOLD_ROW);
                setname("blendParameter", BLEND_PARAMETER_ROW);
                setname("minCyclicBlendParameter", MIN_CYCLIC_BLEND_PARAMETER_ROW);
                setname("maxCyclicBlendParameter", MAX_CYCLIC_BLEND_PARAMETER_ROW);
                setname("indexOfSyncMasterChild", INDEX_OF_SYNC_MASTER_CHILD_ROW);
                setname("subtractLastChild", SUBTRACT_LAST_CHILD_ROW);
                setname("worldFromModelRotation", WORLD_FROM_MODEL_ROTATION_ROW);
                setname("blendSpeed", BLEND_SPEED_ROW);
                setname("minSpeedToSwitch", MIN_SPEED_TO_SWITCH_ROW);
                setname("minSwitchTimeNoError", MIN_SWITCH_TIME_NO_ERROR_ROW);
                setname("minSwitchTimeFullError", MIN_SWITCH_TIME_FULL_ERROR_ROW);
                setname("rootBoneIndex", ROOT_BONE_INDEX_ROW);
                setname("otherBoneIndex", OTHER_BONE_INDEX_ROW);
                setname("anotherBoneIndex", ANOTHER_BONE_INDEX_ROW);
                setname("pelvisIndex", PELVIS_INDEX_ROW);
            }
        }
        (currentIndex() == CHILD_WIDGET) ? childUI->variableRenamed(name, index) : NULL;
    }else{
        LogFile::writeToLog("PoseMatchingGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}

void PoseMatchingGeneratorUI::generatorRenamed(const QString &name, int index){
    (currentIndex() == CHILD_WIDGET) ? childUI->generatorRenamed(name, index) : NULL;
}

void PoseMatchingGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
    setCurrentIndex(MAIN_WIDGET);
    childUI->setBehaviorView(view);
}

