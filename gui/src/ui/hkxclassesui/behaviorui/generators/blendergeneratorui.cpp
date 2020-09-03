#include "blendergeneratorui.h"
#include "blendergeneratorchildui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
//#include "src/hkxclasses/behavior/generators/hkbblendergenerator.h"
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

#define BASE_NUMBER_OF_ROWS 14

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
#define ADD_CHILD_ROW 13

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BlenderGeneratorUI::types = {
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

const QStringList BlenderGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BlenderGeneratorUI::BlenderGeneratorUI()
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
      subtractLastChild(new CheckBox)
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
    table->setItem(ADD_CHILD_ROW, NAME_COLUMN, new TableWidgetItem("Add Child With Generator", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new blender generator child with a generator of the type specified in the adjacent combo box"));
    table->setCellWidget(ADD_CHILD_ROW, TYPE_COLUMN, typeSelectorCB);
    table->setItem(ADD_CHILD_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Child", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected blender generator child"));
    table->setItem(ADD_CHILD_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Child", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected blender generator child"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(childUI);
    childUI->returnPB->setVisible(true);
    toggleSignals(true);
}

void BlenderGeneratorUI::toggleSignals(bool toggleconnections){
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
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)));
        disconnect(childUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)));
        disconnect(childUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(childUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(childUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)));
    }
}

void BlenderGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_BLENDER_GENERATOR){
            bsData = static_cast<hkbBlenderGenerator *>(data);
            name->setText(bsData->getName());
            referencePoseWeightThreshold->setValue(bsData->getReferencePoseWeightThreshold());
            blendParameter->setValue(bsData->getBlendParameter());
            minCyclicBlendParameter->setValue(bsData->getMinCyclicBlendParameter());
            maxCyclicBlendParameter->setValue(bsData->getMaxCyclicBlendParameter());
            indexOfSyncMasterChild->setValue(bsData->getIndexOfSyncMasterChild());
            auto ok = true;
            hkbBlenderGenerator::BlenderFlags flags(bsData->flags.toInt(&ok));
            auto testflag = [&](CheckBox * checkbox, hkbBlenderGenerator::BlenderFlag flagtotest){
                (flags.testFlag(flagtotest)) ? checkbox->setChecked(true) : checkbox->setChecked(false);
            };
            if (ok){
                testflag(flagSync, hkbBlenderGenerator::FLAG_SYNC);
                testflag(flagSmoothGeneratorWeights, hkbBlenderGenerator::FLAG_SMOOTH_GENERATOR_WEIGHTS);
                testflag(flagDontDeactivateChildrenWithZeroWeights, hkbBlenderGenerator::FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS);
                testflag(flagParametricBlend, hkbBlenderGenerator::FLAG_PARAMETRIC_BLEND);
                testflag(flagIsParametricBlendCyclic, hkbBlenderGenerator::FLAG_IS_PARAMETRIC_BLEND_CYCLIC);
                testflag(flagForceDensePose, hkbBlenderGenerator::FLAG_FORCE_DENSE_POSE);
            }else{
                LogFile::writeToLog(QString("BlenderGeneratorUI::loadData(): The flags string is invalid!!!\nString: "+bsData->flags).toLocal8Bit().data());
            }
            subtractLastChild->setChecked(bsData->getSubtractLastChild());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(REFERENCE_POSE_WEIGHT_THRESHOLD_ROW, BINDING_COLUMN, varBind, "referencePoseWeightThreshold", table, bsData);
            UIHelper::loadBinding(BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "blendParameter", table, bsData);
            UIHelper::loadBinding(MIN_CYCLIC_BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "minCyclicBlendParameter", table, bsData);
            UIHelper::loadBinding(MAX_CYCLIC_BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "maxCyclicBlendParameter", table, bsData);
            UIHelper::loadBinding(INDEX_OF_SYNC_MASTER_CHILD_ROW, BINDING_COLUMN, varBind, "indexOfSyncMasterChild", table, bsData);
            UIHelper::loadBinding(SUBTRACT_LAST_CHILD_ROW, BINDING_COLUMN, varBind, "subtractLastChild", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("BlenderGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void BlenderGeneratorUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_CHILD_ROW + bsData->getNumberOfChildren() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_CHILD_ROW + 1, j = 0; j < bsData->getNumberOfChildren(); i++, j++){
            auto child = bsData->getChildDataAt(j);
            if (child){
                UIHelper::setRowItems(i, "Child "+QString::number(j), child->getClassname(), "Remove", "Edit", "Double click to remove this child", "Double click to edit this child", table);
            }else{
                LogFile::writeToLog("BlenderGeneratorUI::loadData(): Null child found!!!");
            }
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::setBindingVariable(int index, const QString & name){
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
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::setBindingVariable(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        bsData->updateChildIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::setReferencePoseWeightThreshold(){
    (bsData) ? bsData->setReferencePoseWeightThreshold(referencePoseWeightThreshold->value()) : LogFile::writeToLog("BlenderGeneratorUI::setReferencePoseWeightThreshold(): The data is nullptr!!");
}

void BlenderGeneratorUI::setBlendParameter(){
    (bsData) ? bsData->setBlendParameter(blendParameter->value()) : LogFile::writeToLog("BlenderGeneratorUI::setBlendParameter(): The data is nullptr!!");
}

void BlenderGeneratorUI::setMinCyclicBlendParameter(){
    (bsData) ? bsData->setMinCyclicBlendParameter(minCyclicBlendParameter->value()) : LogFile::writeToLog("BlenderGeneratorUI::setMinCyclicBlendParameter(): The data is nullptr!!");
}

void BlenderGeneratorUI::setMaxCyclicBlendParameter(){
    (bsData) ? bsData->setMaxCyclicBlendParameter(maxCyclicBlendParameter->value()) : LogFile::writeToLog("BlenderGeneratorUI::setMaxCyclicBlendParameter(): The data is nullptr!!");
}

void BlenderGeneratorUI::setIndexOfSyncMasterChild(){
    (bsData) ? bsData->setIndexOfSyncMasterChild(indexOfSyncMasterChild->value()) : LogFile::writeToLog("BlenderGeneratorUI::setIndexOfSyncMasterChild(): The data is nullptr!!");
}

void BlenderGeneratorUI::setFlag(CheckBox *flagcheckbox, hkbBlenderGenerator::BlenderFlag flagtoset){
    if (bsData){
        auto ok = true;
        hkbBlenderGenerator::BlenderFlags flags(bsData->getFlags().toInt(&ok));
        if (ok){
            (flagcheckbox->isChecked()) ? flags |= flagtoset : flags &= ~(flagtoset);
            bsData->setFlags(QString::number(flags));
        }else{
            LogFile::writeToLog(QString("BlenderGeneratorUI::setFlag(): The flags string is invalid!!!\nString: "+bsData->getFlags()).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::setFlag(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::setFlagSync(){
    setFlag(flagSync, hkbBlenderGenerator::FLAG_SYNC);
}

void BlenderGeneratorUI::setFlagSmoothGeneratorWeights(){
    setFlag(flagSmoothGeneratorWeights, hkbBlenderGenerator::FLAG_SMOOTH_GENERATOR_WEIGHTS);
}

void BlenderGeneratorUI::setFlagDontDeactivateChildrenWithZeroWeights(){
    setFlag(flagDontDeactivateChildrenWithZeroWeights, hkbBlenderGenerator::FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS);
}

void BlenderGeneratorUI::setFlagParametricBlend(){
    setFlag(flagParametricBlend, hkbBlenderGenerator::FLAG_PARAMETRIC_BLEND);
}

void BlenderGeneratorUI::setFlagIsParametricBlendCyclic(){
    setFlag(flagIsParametricBlendCyclic, hkbBlenderGenerator::FLAG_IS_PARAMETRIC_BLEND_CYCLIC);
}

void BlenderGeneratorUI::setFlagForceDensePose(){
    setFlag(flagForceDensePose, hkbBlenderGenerator::FLAG_FORCE_DENSE_POSE);
}

void BlenderGeneratorUI::setSubtractLastChild(){
    (bsData) ? bsData->setSubtractLastChild(subtractLastChild->isChecked()) : LogFile::writeToLog("BlenderGeneratorUI::setSubtractLastChild(): The data is nullptr!!");
}

void BlenderGeneratorUI::swapGeneratorIndices(int index1, int index2){
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

void BlenderGeneratorUI::addChildWithGenerator(){
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
            LogFile::writeToLog("BlenderGeneratorUI::addChild(): Invalid typeEnum!!");
            return;
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::addChild(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::removeChild(int index){
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
        LogFile::writeToLog("BlenderGeneratorUI::removeChild(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::viewSelectedChild(int row, int column){
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
                }
            }
        }else if (row == ADD_CHILD_ROW && column == NAME_COLUMN){
            addChildWithGenerator();
        }else if (row > ADD_CHILD_ROW && row < ADD_CHILD_ROW + bsData->getNumberOfChildren() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (column == VALUE_COLUMN){
                childUI->loadData(bsData->getChildDataAt(result), result);
                setCurrentIndex(CHILD_WIDGET);
            }else if (column == BINDING_COLUMN){
                if (MainWindow::yesNoDialogue("Are you sure you want to remove the child \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                    removeChild(result);
                }
            }
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::returnToWidget(bool reloadData){
    (reloadData) ? loadDynamicTableRows() : NULL;
    setCurrentIndex(MAIN_WIDGET);
}

void BlenderGeneratorUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case CHILD_WIDGET:
        childUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("BlenderGeneratorUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BlenderGeneratorUI::generatorTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case CHILD_WIDGET:
        childUI->setGenerator(index, name); break;
    default:
        WARNING_MESSAGE("BlenderGeneratorUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BlenderGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties && generators){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(generatorTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BlenderGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BlenderGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::variableRenamed(const QString & name, int index){
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
            }
            (currentIndex() == CHILD_WIDGET) ? childUI->variableRenamed(name, index) : NULL;
        }else{
            WARNING_MESSAGE("BlenderGeneratorUI::variableRenamed(): The new variable name is the empty string!!");
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}

void BlenderGeneratorUI::generatorRenamed(const QString &name, int index){
    switch (currentIndex()){
    case CHILD_WIDGET:
        childUI->generatorRenamed(name, index); break;
    }
}

void BlenderGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
    setCurrentIndex(MAIN_WIDGET);
    childUI->setBehaviorView(view);
}

