#include "gethandleonbonemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbgethandleonbonemodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define HANDLE_OUT_ROW 2
#define LOCAL_FRAME_NAME_ROW 3
#define RAGDOLL_BONE_INDEX_ROW 4
#define ANIMATION_BONE_INDEX_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList GetHandleOnBoneModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

GetHandleOnBoneModifierUI::GetHandleOnBoneModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      localFrameName(new ComboBox),
      ragdollBoneIndex(new ComboBox),
      animationBoneIndex(new ComboBox)
{
    setTitle("hkbGetHandleOnBoneModifier");
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
    table->setItem(HANDLE_OUT_ROW, NAME_COLUMN, new TableWidgetItem("handleOut"));
    table->setItem(HANDLE_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkHandle", Qt::AlignCenter));
    table->setItem(HANDLE_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(HANDLE_OUT_ROW, VALUE_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(LOCAL_FRAME_NAME_ROW, NAME_COLUMN, new TableWidgetItem("localFrameName"));
    table->setItem(LOCAL_FRAME_NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(LOCAL_FRAME_NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(LOCAL_FRAME_NAME_ROW, VALUE_COLUMN, localFrameName);
    table->setItem(RAGDOLL_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("ragdollBoneIndex"));
    table->setItem(RAGDOLL_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(RAGDOLL_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RAGDOLL_BONE_INDEX_ROW, VALUE_COLUMN, ragdollBoneIndex);
    table->setItem(ANIMATION_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("animationBoneIndex"));
    table->setItem(ANIMATION_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ANIMATION_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANIMATION_BONE_INDEX_ROW, VALUE_COLUMN, animationBoneIndex);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void GetHandleOnBoneModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(localFrameName, SIGNAL(currentTextChanged(QString)), this, SLOT(setLocalFrameName(QString)), Qt::UniqueConnection);
        connect(ragdollBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setRagdollBoneIndex(int)), Qt::UniqueConnection);
        connect(animationBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnimationBoneIndex(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(localFrameName, SIGNAL(currentTextChanged(QString)), this, SLOT(setLocalFrameName(QString)));
        disconnect(ragdollBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setRagdollBoneIndex(int)));
        disconnect(animationBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnimationBoneIndex(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void GetHandleOnBoneModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("GetHandleOnBoneModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void GetHandleOnBoneModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_GET_HANDLE_ON_BONE_MODIFIER){
            bsData = static_cast<hkbGetHandleOnBoneModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            if (!localFrameName->count()){
                auto localFrames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getLocalFrameNames();
                localFrameName->insertItems(0, localFrames);
            }
            auto index = localFrameName->findText(bsData->getLocalFrameName());
            if (index < 0 || index >= localFrameName->count()){
                LogFile::writeToLog("The local frame name was not loaded correctly!!!");
            }else{
                localFrameName->setCurrentIndex(index);
            }
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(ragdollBoneIndex, bsData->getRagdollBoneIndex() + 1);
            loadbones(animationBoneIndex, bsData->getAnimationBoneIndex() + 1);
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(HANDLE_OUT_ROW, BINDING_COLUMN, varBind, "handleOut", table, bsData);
            UIHelper::loadBinding(RAGDOLL_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "ragdollBoneIndex", table, bsData);
            UIHelper::loadBinding(ANIMATION_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "animationBoneIndex", table, bsData);
        }else{
            LogFile::writeToLog("GetHandleOnBoneModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("GetHandleOnBoneModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void GetHandleOnBoneModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("GetHandleOnBoneModifierUI::setName(): The data is nullptr!!");
    }
}

void GetHandleOnBoneModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("GetHandleOnBoneModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void GetHandleOnBoneModifierUI::setLocalFrameName(int index){
    (bsData) ? bsData->setLocalFrameName(index - 1) : LogFile::writeToLog("GetHandleOnBoneModifierUI::setLocalFrameName(): The 'bsData' pointer is nullptr!!");
}

void GetHandleOnBoneModifierUI::setRagdollBoneIndex(int index){
    (bsData) ? bsData->setRagdollBoneIndex(index - 1) : LogFile::writeToLog("GetHandleOnBoneModifierUI::setRagdollBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void GetHandleOnBoneModifierUI::setAnimationBoneIndex(int index){
    (bsData) ? bsData->setAnimationBoneIndex(index - 1) : LogFile::writeToLog("GetHandleOnBoneModifierUI::setAnimationBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void GetHandleOnBoneModifierUI::viewSelected(int row, int column){
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
            case HANDLE_OUT_ROW:
                checkisproperty(HANDLE_OUT_ROW, "handleOut"); break;
            case RAGDOLL_BONE_INDEX_ROW:
                checkisproperty(RAGDOLL_BONE_INDEX_ROW, "ragdollBoneIndex"); break;
            case ANIMATION_BONE_INDEX_ROW:
                checkisproperty(ANIMATION_BONE_INDEX_ROW, "animationBoneIndex"); break;
            }
        }
    }else{
        LogFile::writeToLog("GetHandleOnBoneModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void GetHandleOnBoneModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("GetHandleOnBoneModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void GetHandleOnBoneModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("handleOut", HANDLE_OUT_ROW);
            setname("ragdollBoneIndex", RAGDOLL_BONE_INDEX_ROW);
            setname("animationBoneIndex", ANIMATION_BONE_INDEX_ROW);
        }
    }else{
        LogFile::writeToLog("GetHandleOnBoneModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void GetHandleOnBoneModifierUI::setBindingVariable(int index, const QString &name){
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
        case HANDLE_OUT_ROW:
            checkisproperty(HANDLE_OUT_ROW, "handleOut", VARIABLE_TYPE_POINTER); break;
        case RAGDOLL_BONE_INDEX_ROW:
            checkisproperty(RAGDOLL_BONE_INDEX_ROW, "ragdollBoneIndex", VARIABLE_TYPE_INT32); break;
        case ANIMATION_BONE_INDEX_ROW:
            checkisproperty(ANIMATION_BONE_INDEX_ROW, "animationBoneIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("GetHandleOnBoneModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
