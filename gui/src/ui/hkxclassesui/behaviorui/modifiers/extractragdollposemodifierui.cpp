#include "extractragdollposemodifierui.h"

#include "src/hkxclasses/behavior/modifiers/hkbExtractRagdollPoseModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define POSE_MATCHING_BONE_0_ROW 2
#define POSE_MATCHING_BONE_1_ROW 3
#define POSE_MATCHING_BONE_2_ROW 4
#define ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ExtractRagdollPoseModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ExtractRagdollPoseModifierUI::ExtractRagdollPoseModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      poseMatchingBone0(new ComboBox),
      poseMatchingBone1(new ComboBox),
      poseMatchingBone2(new ComboBox),
      enableComputeWorldFromModel(new CheckBox)
{
    setTitle("hkbExtractRagdollPoseModifier");
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
    table->setItem(POSE_MATCHING_BONE_0_ROW, NAME_COLUMN, new TableWidgetItem("poseMatchingBone0"));
    table->setItem(POSE_MATCHING_BONE_0_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(POSE_MATCHING_BONE_0_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSE_MATCHING_BONE_0_ROW, VALUE_COLUMN, poseMatchingBone0);
    table->setItem(POSE_MATCHING_BONE_1_ROW, NAME_COLUMN, new TableWidgetItem("poseMatchingBone1"));
    table->setItem(POSE_MATCHING_BONE_1_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(POSE_MATCHING_BONE_1_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSE_MATCHING_BONE_1_ROW, VALUE_COLUMN, poseMatchingBone1);
    table->setItem(POSE_MATCHING_BONE_2_ROW, NAME_COLUMN, new TableWidgetItem("poseMatchingBone2"));
    table->setItem(POSE_MATCHING_BONE_2_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(POSE_MATCHING_BONE_2_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSE_MATCHING_BONE_2_ROW, VALUE_COLUMN, poseMatchingBone2);
    table->setItem(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, NAME_COLUMN, new TableWidgetItem("enableComputeWorldFromModel"));
    table->setItem(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, VALUE_COLUMN, enableComputeWorldFromModel);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ExtractRagdollPoseModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(poseMatchingBone0, SIGNAL(currentIndexChanged(int)), this, SLOT(setPoseMatchingBone0(int)), Qt::UniqueConnection);
        connect(poseMatchingBone1, SIGNAL(currentIndexChanged(int)), this, SLOT(setPoseMatchingBone1(int)), Qt::UniqueConnection);
        connect(poseMatchingBone2, SIGNAL(currentIndexChanged(int)), this, SLOT(setPoseMatchingBone2(int)), Qt::UniqueConnection);
        connect(enableComputeWorldFromModel, SIGNAL(released()), this, SLOT(setEnableComputeWorldFromModel()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(poseMatchingBone0, SIGNAL(currentIndexChanged(int)), this, SLOT(setPoseMatchingBone0(int)));
        disconnect(poseMatchingBone1, SIGNAL(currentIndexChanged(int)), this, SLOT(setPoseMatchingBone1(int)));
        disconnect(poseMatchingBone2, SIGNAL(currentIndexChanged(int)), this, SLOT(setPoseMatchingBone2(int)));
        disconnect(enableComputeWorldFromModel, SIGNAL(released()), this, SLOT(setEnableComputeWorldFromModel()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void ExtractRagdollPoseModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ExtractRagdollPoseModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_EXTRACT_RAGDOLL_POSE_MODIFIER){
            bsData = static_cast<hkbExtractRagdollPoseModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(poseMatchingBone0, bsData->getPoseMatchingBone0() + 1);
            loadbones(poseMatchingBone1, bsData->getPoseMatchingBone1() + 1);
            loadbones(poseMatchingBone2, bsData->getPoseMatchingBone2() + 1);
            enableComputeWorldFromModel->setChecked(bsData->getEnableComputeWorldFromModel());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(POSE_MATCHING_BONE_0_ROW, BINDING_COLUMN, varBind, "poseMatchingBone0", table, bsData);
            UIHelper::loadBinding(POSE_MATCHING_BONE_1_ROW, BINDING_COLUMN, varBind, "poseMatchingBone1", table, bsData);
            UIHelper::loadBinding(POSE_MATCHING_BONE_2_ROW, BINDING_COLUMN, varBind, "poseMatchingBone2", table, bsData);
            UIHelper::loadBinding(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, BINDING_COLUMN, varBind, "enableComputeWorldFromModel", table, bsData);
        }else{
            LogFile::writeToLog("ExtractRagdollPoseModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void ExtractRagdollPoseModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::setName(): The data is nullptr!!");
    }
}

void ExtractRagdollPoseModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("ExtractRagdollPoseModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void ExtractRagdollPoseModifierUI::setPoseMatchingBone0(int index){
    (bsData) ? bsData->setPoseMatchingBone0(index - 1) : LogFile::writeToLog("ExtractRagdollPoseModifierUI::setPoseMatchingBone0(): The 'bsData' pointer is nullptr!!");
}

void ExtractRagdollPoseModifierUI::setPoseMatchingBone1(int index){
    (bsData) ? bsData->setPoseMatchingBone1(index - 1) : LogFile::writeToLog("ExtractRagdollPoseModifierUI::setPoseMatchingBone1(): The 'bsData' pointer is nullptr!!");
}

void ExtractRagdollPoseModifierUI::setPoseMatchingBone2(int index){
    (bsData) ? bsData->setPoseMatchingBone2(index - 1) : LogFile::writeToLog("ExtractRagdollPoseModifierUI::setPoseMatchingBone2(): The 'bsData' pointer is nullptr!!");
}

void ExtractRagdollPoseModifierUI::setEnableComputeWorldFromModel(){
    (bsData) ? bsData->setEnableComputeWorldFromModel(enable->isChecked()) : LogFile::writeToLog("ExtractRagdollPoseModifierUI::setEnableComputeWorldFromModel(): The 'bsData' pointer is nullptr!!");
}

void ExtractRagdollPoseModifierUI::viewSelected(int row, int column){
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
            case POSE_MATCHING_BONE_0_ROW:
                checkisproperty(POSE_MATCHING_BONE_0_ROW, "poseMatchingBone0"); break;
            case POSE_MATCHING_BONE_1_ROW:
                checkisproperty(POSE_MATCHING_BONE_1_ROW, "poseMatchingBone1"); break;
            case POSE_MATCHING_BONE_2_ROW:
                checkisproperty(POSE_MATCHING_BONE_2_ROW, "poseMatchingBone2"); break;
            case ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW:
                checkisproperty(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, "enableComputeWorldFromModel"); break;
            }
        }
    }else{
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void ExtractRagdollPoseModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void ExtractRagdollPoseModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("poseMatchingBone0", POSE_MATCHING_BONE_0_ROW);
            setname("poseMatchingBone1", POSE_MATCHING_BONE_1_ROW);
            setname("poseMatchingBone2", POSE_MATCHING_BONE_2_ROW);
            setname("enableComputeWorldFromModel", ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW);
        }
    }else{
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void ExtractRagdollPoseModifierUI::setBindingVariable(int index, const QString &name){
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
        case POSE_MATCHING_BONE_0_ROW:
            checkisproperty(POSE_MATCHING_BONE_0_ROW, "poseMatchingBone0", VARIABLE_TYPE_INT32); break;
        case POSE_MATCHING_BONE_1_ROW:
            checkisproperty(POSE_MATCHING_BONE_1_ROW, "poseMatchingBone1", VARIABLE_TYPE_INT32); break;
        case POSE_MATCHING_BONE_2_ROW:
            checkisproperty(POSE_MATCHING_BONE_2_ROW, "poseMatchingBone2", VARIABLE_TYPE_INT32); break;
        case ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW:
            checkisproperty(ENABLE_COMPUTE_WORLD_FROM_MODEL_ROW, "enableComputeWorldFromModel", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("ExtractRagdollPoseModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
