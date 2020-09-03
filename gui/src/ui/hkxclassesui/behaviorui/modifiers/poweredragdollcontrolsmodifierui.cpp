#include "poweredragdollcontrolsmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbPoweredRagdollControlsModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/boneindexarrayui.h"
#include "src/ui/hkxclassesui/behaviorui/boneweightarrayui.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 13

#define NAME_ROW 0
#define ENABLE_ROW 1
#define MAX_FORCE_ROW 2
#define TAU_ROW 3
#define DAMPING_ROW 4
#define PROPERTIONAL_RECOVERY_VELOCITY_ROW 5
#define CONSTANT_RECOVERY_VELOCITY_ROW 6
#define BONES_ROW 7
#define POSE_MATCHING_BONE_0_ROW 8
#define POSE_MATCHING_BONE_1_ROW 9
#define POSE_MATCHING_BONE_2_ROW 10
#define MODE_ROW 11
#define BONE_WEIGHTS_ROW 12

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList PoweredRagdollControlsModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

PoweredRagdollControlsModifierUI::PoweredRagdollControlsModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      groupBox(new QGroupBox("hkbPoweredRagdollControlsModifier")),
      table(new TableWidget(QColor(Qt::white))),
      boneIndexUI(new BoneIndexArrayUI),
      boneWeightsUI(new BoneWeightArrayUI),
      name(new LineEdit),
      enable(new CheckBox),
      maxForce(new DoubleSpinBox),
      tau(new DoubleSpinBox),
      damping(new DoubleSpinBox),
      proportionalRecoveryVelocity(new DoubleSpinBox),
      constantRecoveryVelocity(new DoubleSpinBox),
      bones(new CheckButtonCombo("Edit")),
      poseMatchingBone0(new ComboBox),
      poseMatchingBone1(new ComboBox),
      poseMatchingBone2(new ComboBox),
      mode(new ComboBox),
      boneWeights(new CheckButtonCombo("Edit"))
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(MAX_FORCE_ROW, NAME_COLUMN, new TableWidgetItem("maxForce"));
    table->setItem(MAX_FORCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MAX_FORCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MAX_FORCE_ROW, VALUE_COLUMN, maxForce);
    table->setItem(TAU_ROW, NAME_COLUMN, new TableWidgetItem("tau"));
    table->setItem(TAU_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TAU_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TAU_ROW, VALUE_COLUMN, tau);
    table->setItem(DAMPING_ROW, NAME_COLUMN, new TableWidgetItem("damping"));
    table->setItem(DAMPING_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DAMPING_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DAMPING_ROW, VALUE_COLUMN, damping);
    table->setItem(PROPERTIONAL_RECOVERY_VELOCITY_ROW, NAME_COLUMN, new TableWidgetItem("proportionalRecoveryVelocity"));
    table->setItem(PROPERTIONAL_RECOVERY_VELOCITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(PROPERTIONAL_RECOVERY_VELOCITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(PROPERTIONAL_RECOVERY_VELOCITY_ROW, VALUE_COLUMN, proportionalRecoveryVelocity);
    table->setItem(CONSTANT_RECOVERY_VELOCITY_ROW, NAME_COLUMN, new TableWidgetItem("constantRecoveryVelocity"));
    table->setItem(CONSTANT_RECOVERY_VELOCITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CONSTANT_RECOVERY_VELOCITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CONSTANT_RECOVERY_VELOCITY_ROW, VALUE_COLUMN, constantRecoveryVelocity);
    table->setItem(BONES_ROW, NAME_COLUMN, new TableWidgetItem("bones"));
    table->setItem(BONES_ROW, TYPE_COLUMN, new TableWidgetItem("hkBoneIndexArray", Qt::AlignCenter));
    table->setItem(BONES_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(BONES_ROW, VALUE_COLUMN, bones);
    table->setItem(POSE_MATCHING_BONE_0_ROW, NAME_COLUMN, new TableWidgetItem("poseMatchingBone0"));
    table->setItem(POSE_MATCHING_BONE_0_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(POSE_MATCHING_BONE_0_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSE_MATCHING_BONE_0_ROW, VALUE_COLUMN, poseMatchingBone0);
    table->setItem(POSE_MATCHING_BONE_1_ROW, NAME_COLUMN, new TableWidgetItem("poseMatchingBone1"));
    table->setItem(POSE_MATCHING_BONE_1_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(POSE_MATCHING_BONE_1_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSE_MATCHING_BONE_1_ROW, VALUE_COLUMN, poseMatchingBone1);
    table->setItem(POSE_MATCHING_BONE_2_ROW, NAME_COLUMN, new TableWidgetItem("poseMatchingBone2"));
    table->setItem(POSE_MATCHING_BONE_2_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(POSE_MATCHING_BONE_2_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSE_MATCHING_BONE_2_ROW, VALUE_COLUMN, poseMatchingBone2);
    table->setItem(MODE_ROW, NAME_COLUMN, new TableWidgetItem("mode"));
    table->setItem(MODE_ROW, TYPE_COLUMN, new TableWidgetItem("Mode", Qt::AlignCenter));
    table->setItem(MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(MODE_ROW, VALUE_COLUMN, mode);
    table->setItem(BONE_WEIGHTS_ROW, NAME_COLUMN, new TableWidgetItem("boneWeights"));
    table->setItem(BONE_WEIGHTS_ROW, TYPE_COLUMN, new TableWidgetItem("hkbBoneWeightArray", Qt::AlignCenter));
    table->setItem(BONE_WEIGHTS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BONE_WEIGHTS_ROW, VALUE_COLUMN, boneWeights);
    topLyt->addWidget(table, 0, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(boneIndexUI);
    addWidget(boneWeightsUI);
    toggleSignals(true);
}

void PoweredRagdollControlsModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(maxForce, SIGNAL(editingFinished()), this, SLOT(setMaxForce()), Qt::UniqueConnection);
        connect(tau, SIGNAL(editingFinished()), this, SLOT(setTau()), Qt::UniqueConnection);
        connect(damping, SIGNAL(editingFinished()), this, SLOT(setDamping()), Qt::UniqueConnection);
        connect(proportionalRecoveryVelocity, SIGNAL(released()), this, SLOT(setProportionalRecoveryVelocity()), Qt::UniqueConnection);
        connect(constantRecoveryVelocity, SIGNAL(released()), this, SLOT(setConstantRecoveryVelocity()), Qt::UniqueConnection);
        connect(poseMatchingBone0, SIGNAL(editingFinished()), this, SLOT(setPoseMatchingBone0()), Qt::UniqueConnection);
        connect(poseMatchingBone1, SIGNAL(editingFinished()), this, SLOT(setPoseMatchingBone1()), Qt::UniqueConnection);
        connect(poseMatchingBone2, SIGNAL(editingFinished()), this, SLOT(setPoseMatchingBone2()), Qt::UniqueConnection);
        connect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(setMode(int)), Qt::UniqueConnection);
        connect(bones, SIGNAL(pressed()), this, SLOT(viewBones()), Qt::UniqueConnection);
        connect(bones, SIGNAL(enabled(bool)), this, SLOT(toggleBones(bool)), Qt::UniqueConnection);
        connect(boneIndexUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(boneWeights, SIGNAL(pressed()), this, SLOT(viewBoneWeights()), Qt::UniqueConnection);
        connect(boneWeights, SIGNAL(enabled(bool)), this, SLOT(toggleBoneWeights(bool)), Qt::UniqueConnection);
        connect(boneWeightsUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(maxForce, SIGNAL(editingFinished()), this, SLOT(setMaxForce()));
        disconnect(tau, SIGNAL(editingFinished()), this, SLOT(setTau()));
        disconnect(damping, SIGNAL(editingFinished()), this, SLOT(setDamping()));
        disconnect(proportionalRecoveryVelocity, SIGNAL(released()), this, SLOT(setProportionalRecoveryVelocity()));
        disconnect(constantRecoveryVelocity, SIGNAL(released()), this, SLOT(setConstantRecoveryVelocity()));
        disconnect(poseMatchingBone0, SIGNAL(editingFinished()), this, SLOT(setPoseMatchingBone0()));
        disconnect(poseMatchingBone1, SIGNAL(editingFinished()), this, SLOT(setPoseMatchingBone1()));
        disconnect(poseMatchingBone2, SIGNAL(editingFinished()), this, SLOT(setPoseMatchingBone2()));
        disconnect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(setMode(int)));
        disconnect(bones, SIGNAL(pressed()), this, SLOT(viewBones()));
        disconnect(bones, SIGNAL(enabled(bool)), this, SLOT(toggleBones(bool)));
        disconnect(boneIndexUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(boneWeights, SIGNAL(pressed()), this, SLOT(viewBoneWeights()));
        disconnect(boneWeights, SIGNAL(enabled(bool)), this, SLOT(toggleBoneWeights(bool)));
        disconnect(boneWeightsUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void PoweredRagdollControlsModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *ragdollBones){
    if (variables && properties && ragdollBones){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(ragdollBones, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(ragdollBones, SIGNAL(elementSelected(int,QString)), boneIndexUI, SLOT(setRagdollBone(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(boneIndexUI, SIGNAL(viewRagdollBones(int)), ragdollBones, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER){
            bsData = static_cast<hkbPoweredRagdollControlsModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            maxForce->setValue(bsData->getMaxForce());
            tau->setValue(bsData->getTau());
            damping->setValue(bsData->getDamping());
            proportionalRecoveryVelocity->setValue(bsData->getProportionalRecoveryVelocity());
            constantRecoveryVelocity->setValue(bsData->getConstantRecoveryVelocity());
            (bsData->getBones()) ? bones->setChecked(true), bones->setText("Edit") : bones->setChecked(false), bones->setText("nullptr");
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRagdollBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(poseMatchingBone0, bsData->getPoseMatchingBone0() + 1);
            loadbones(poseMatchingBone1, bsData->getPoseMatchingBone1() + 1);
            loadbones(poseMatchingBone2, bsData->getPoseMatchingBone2() + 1);
            (!mode->count()) ? mode->insertItems(0, bsData->Mode) : NULL;
            mode->setCurrentIndex(bsData->Mode.indexOf(bsData->getMode()));
            (bsData->getBoneWeights()) ? boneWeights->setChecked(true), boneWeights->setText("Edit") : boneWeights->setChecked(false), boneWeights->setText("nullptr");
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(MAX_FORCE_ROW, BINDING_COLUMN, varBind, "maxForce", table, bsData);
            UIHelper::loadBinding(TAU_ROW, BINDING_COLUMN, varBind, "tau", table, bsData);
            UIHelper::loadBinding(DAMPING_ROW, BINDING_COLUMN, varBind, "damping", table, bsData);
            UIHelper::loadBinding(PROPERTIONAL_RECOVERY_VELOCITY_ROW, BINDING_COLUMN, varBind, "proportionalRecoveryVelocity", table, bsData);
            UIHelper::loadBinding(CONSTANT_RECOVERY_VELOCITY_ROW, BINDING_COLUMN, varBind, "constantRecoveryVelocity", table, bsData);
            UIHelper::loadBinding(POSE_MATCHING_BONE_0_ROW, BINDING_COLUMN, varBind, "poseMatchingBone0", table, bsData);
            UIHelper::loadBinding(POSE_MATCHING_BONE_1_ROW, BINDING_COLUMN, varBind, "poseMatchingBone1", table, bsData);
            UIHelper::loadBinding(POSE_MATCHING_BONE_2_ROW, BINDING_COLUMN, varBind, "poseMatchingBone2", table, bsData);
            UIHelper::loadBinding(BONE_WEIGHTS_ROW, BINDING_COLUMN, varBind, "boneWeights", table, bsData);
        }else{
            LogFile::writeToLog("PoweredRagdollControlsModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void PoweredRagdollControlsModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::setName(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("PoweredRagdollControlsModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setMaxForce(){
    (bsData) ? bsData->setMaxForce(maxForce->value()) : LogFile::writeToLog("PoweredRagdollControlsModifierUI::setMaxForce(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setTau(){
    (bsData) ? bsData->setTau(tau->value()) : LogFile::writeToLog("PoweredRagdollControlsModifierUI::setTau(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setDamping(){
    (bsData) ? bsData->setDamping(damping->value()) : LogFile::writeToLog("PoweredRagdollControlsModifierUI::setDamping(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setProportionalRecoveryVelocity(){
    (bsData) ? bsData->setProportionalRecoveryVelocity(proportionalRecoveryVelocity->value()) : LogFile::writeToLog("PoweredRagdollControlsModifierUI::setProportionalRecoveryVelocity(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setConstantRecoveryVelocity(){
    (bsData) ? bsData->setConstantRecoveryVelocity(constantRecoveryVelocity->value()) : LogFile::writeToLog("PoweredRagdollControlsModifierUI::setConstantRecoveryVelocity(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setPoseMatchingBone0(int index){
    (bsData) ? bsData->setPoseMatchingBone0(index - 1) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setPoseMatchingBone0(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setPoseMatchingBone1(int index){
    (bsData) ? bsData->setPoseMatchingBone1(index - 1) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setPoseMatchingBone1(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setPoseMatchingBone2(int index){
    (bsData) ? bsData->setPoseMatchingBone2(index - 1) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setPoseMatchingBone2(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::setMode(int index){
    (bsData) ? bsData->setMode(index) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setMode(): The 'bsData' pointer is nullptr!!");
}

void PoweredRagdollControlsModifierUI::toggleBones(bool enable){
    if (bsData){
        if (!enable){
            bsData->setBones(nullptr);
            static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
        }else if (!bsData->getBones()){
            bsData->setBones(new hkbBoneIndexArray(bsData->getParentFile()));
        }
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::toggleBones(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::viewBones(){
    if (bsData){
        boneIndexUI->loadData(bsData->getBones());
        setCurrentIndex(BONE_INDEX_WIDGET);
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::viewBones(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::toggleBoneWeights(bool enable){
    if (bsData){
        if (!enable){
            bsData->setBoneWeights(nullptr);
            static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
        }else if (enable && !bsData->getBoneWeights()){
            bsData->setBoneWeights(new hkbBoneWeightArray(bsData->getParentFile(), -1, static_cast<BehaviorFile *>(bsData->getParentFile())->getNumberOfBones()));
            boneWeights->setText("Edit");
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::toggleBoneWeights(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::viewBoneWeights(){
    if (bsData){
        boneWeightsUI->loadData(bsData->getBoneWeights());
        setCurrentIndex(BONE_WEIGHT_WIDGET);
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::viewBoneWeights(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case ENABLE_ROW:
                checkisproperty(ENABLE_ROW, "enable"); break;
            case MAX_FORCE_ROW:
                checkisproperty(MAX_FORCE_ROW, "maxForce"); break;
            case TAU_ROW:
                checkisproperty(TAU_ROW, "tau"); break;
            case DAMPING_ROW:
                checkisproperty(DAMPING_ROW, "damping"); break;
            case PROPERTIONAL_RECOVERY_VELOCITY_ROW:
                checkisproperty(PROPERTIONAL_RECOVERY_VELOCITY_ROW, "proportionalRecoveryVelocity"); break;
            case CONSTANT_RECOVERY_VELOCITY_ROW:
                checkisproperty(CONSTANT_RECOVERY_VELOCITY_ROW, "constantRecoveryVelocity"); break;
            case POSE_MATCHING_BONE_0_ROW:
                checkisproperty(POSE_MATCHING_BONE_0_ROW, "poseMatchingBone0"); break;
            case POSE_MATCHING_BONE_1_ROW:
                checkisproperty(POSE_MATCHING_BONE_1_ROW, "poseMatchingBone1"); break;
            case POSE_MATCHING_BONE_2_ROW:
                checkisproperty(POSE_MATCHING_BONE_2_ROW, "poseMatchingBone2"); break;
            case BONE_WEIGHTS_ROW:
                checkisproperty(BONE_WEIGHTS_ROW, "boneWeights"); break;
            }
        }
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::selectTableToView(bool viewisProperty, const QString & path){
    if (bsData){
        if (viewisProperty){
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
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("maxForce", MAX_FORCE_ROW);
            setname("tau", TAU_ROW);
            setname("damping", DAMPING_ROW);
            setname("proportionalRecoveryVelocity", PROPERTIONAL_RECOVERY_VELOCITY_ROW);
            setname("constantRecoveryVelocity", CONSTANT_RECOVERY_VELOCITY_ROW);
            setname("poseMatchingBone0", POSE_MATCHING_BONE_0_ROW);
            setname("poseMatchingBone1", POSE_MATCHING_BONE_1_ROW);
            setname("poseMatchingBone2", POSE_MATCHING_BONE_2_ROW);
            setname("boneWeights", BONE_WEIGHTS_ROW);
        }
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case ENABLE_ROW:
            checkisproperty(ENABLE_ROW, "enable", VARIABLE_TYPE_BOOL); break;
        case MAX_FORCE_ROW:
            checkisproperty(MAX_FORCE_ROW, "maxForce", VARIABLE_TYPE_REAL); break;
        case TAU_ROW:
            checkisproperty(TAU_ROW, "tau", VARIABLE_TYPE_REAL); break;
        case DAMPING_ROW:
            checkisproperty(DAMPING_ROW, "damping", VARIABLE_TYPE_REAL); break;
        case PROPERTIONAL_RECOVERY_VELOCITY_ROW:
            checkisproperty(PROPERTIONAL_RECOVERY_VELOCITY_ROW, "proportionalRecoveryVelocity", VARIABLE_TYPE_REAL); break;
        case CONSTANT_RECOVERY_VELOCITY_ROW:
            checkisproperty(CONSTANT_RECOVERY_VELOCITY_ROW, "constantRecoveryVelocity", VARIABLE_TYPE_REAL); break;
        case POSE_MATCHING_BONE_0_ROW:
            checkisproperty(POSE_MATCHING_BONE_0_ROW, "poseMatchingBone0", VARIABLE_TYPE_INT32); break;
        case POSE_MATCHING_BONE_1_ROW:
            checkisproperty(POSE_MATCHING_BONE_1_ROW, "poseMatchingBone1", VARIABLE_TYPE_INT32); break;
        case POSE_MATCHING_BONE_2_ROW:
            checkisproperty(POSE_MATCHING_BONE_2_ROW, "poseMatchingBone2", VARIABLE_TYPE_INT32); break;
        case BONE_WEIGHTS_ROW:
            checkisproperty(BONE_WEIGHTS_ROW, "boneWeights", VARIABLE_TYPE_POINTER); break;
        }
    }else{
        LogFile::writeToLog("PoweredRagdollControlsModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void PoweredRagdollControlsModifierUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}
