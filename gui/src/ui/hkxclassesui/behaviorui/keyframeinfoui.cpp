#include "keyframeinfoui.h"

#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define KEYFRAMED_POSITION_ROW 0
#define KEYFRAMED_ROTATION_ROW 1
#define BONE_INDEX_ROW 2
#define IS_VALID_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList KeyframeInfoUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

KeyframeInfoUI::KeyframeInfoUI()
    : file(nullptr),
      keyframeIndex(-1),
      bsData(nullptr),
      parent(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      keyframedPosition(new QuadVariableWidget),
      keyframedRotation(new QuadVariableWidget),
      boneIndex(new ComboBox),
      isValid(new CheckBox)
{
    setTitle("hkKeyframeInfo");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(KEYFRAMED_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("keyframedPosition"));
    table->setItem(KEYFRAMED_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(KEYFRAMED_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(KEYFRAMED_POSITION_ROW, VALUE_COLUMN, keyframedPosition);
    table->setItem(KEYFRAMED_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("keyframedRotation"));
    table->setItem(KEYFRAMED_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(KEYFRAMED_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(KEYFRAMED_ROTATION_ROW, VALUE_COLUMN, keyframedRotation);
    table->setItem(BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("boneIndex"));
    table->setItem(BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BONE_INDEX_ROW, VALUE_COLUMN, boneIndex);
    table->setItem(IS_VALID_ROW, NAME_COLUMN, new TableWidgetItem("isValid"));
    table->setItem(IS_VALID_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_VALID_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_VALID_ROW, VALUE_COLUMN, isValid);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void KeyframeInfoUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(keyframedPosition, SIGNAL(editingFinished()), this, SLOT(setKeyframedPosition()), Qt::UniqueConnection);
        connect(keyframedRotation, SIGNAL(editingFinished()), this, SLOT(setKeyframedRotation()), Qt::UniqueConnection);
        connect(boneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoneIndex(int)), Qt::UniqueConnection);
        connect(isValid, SIGNAL(released()), this, SLOT(setIsValid()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(keyframedPosition, SIGNAL(editingFinished()), this, SLOT(setKeyframedPosition()));
        disconnect(keyframedRotation, SIGNAL(editingFinished()), this, SLOT(setKeyframedRotation()));
        disconnect(boneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoneIndex(int)));
        disconnect(isValid, SIGNAL(released()), this, SLOT(setIsValid()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void KeyframeInfoUI::loadData(BehaviorFile *parentFile, hkbKeyframeBonesModifier::hkKeyframeInfo *key, hkbKeyframeBonesModifier *par, int index){
    toggleSignals(false);
    if (parentFile && key && par && index > -1){
        parent = par;
        keyframeIndex = index;
        file = parentFile;
        bsData = key;
        keyframedPosition->setValue(bsData->keyframedPosition);
        keyframedRotation->setValue(bsData->keyframedRotation);
        if (!boneIndex->count()){
            auto boneNames = QStringList("None") + file->getRagdollBoneNames();
            boneIndex->insertItems(0, boneNames);
        }
        boneIndex->setCurrentIndex(bsData->boneIndex + 1);
        isValid->setChecked(bsData->isValid);
        auto varBind = parent->getVariableBindingSetData();
        UIHelper::loadBinding(KEYFRAMED_POSITION_ROW, BINDING_COLUMN, varBind, "keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedPosition", table, parent);
        UIHelper::loadBinding(KEYFRAMED_ROTATION_ROW, BINDING_COLUMN, varBind, "keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedRotation", table, parent);
        UIHelper::loadBinding(BONE_INDEX_ROW, BINDING_COLUMN, varBind, "keyframeInfo:"+QString::number(keyframeIndex)+"/boneIndex", table, parent);
        UIHelper::loadBinding(IS_VALID_ROW, BINDING_COLUMN, varBind, "keyframeInfo:"+QString::number(keyframeIndex)+"/isValid", table, parent);
    }else{
        LogFile::writeToLog("KeyframeInfoUI::loadData(): Behavior file, bind or data is null!!!");
    }
    toggleSignals(true);
}

void KeyframeInfoUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, parent);
        };
        switch (row){
        case KEYFRAMED_POSITION_ROW:
            checkisproperty(KEYFRAMED_POSITION_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedPosition", VARIABLE_TYPE_VECTOR4); break;
        case KEYFRAMED_ROTATION_ROW:
            checkisproperty(KEYFRAMED_ROTATION_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedRotation", VARIABLE_TYPE_QUATERNION); break;
        case BONE_INDEX_ROW:
            checkisproperty(BONE_INDEX_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/boneIndex", VARIABLE_TYPE_INT32); break;
        case IS_VALID_ROW:
            checkisproperty(IS_VALID_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/isValid", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("KeyframeInfoUI::setBindingVariable(): The data is nullptr!!");
    }
}

void KeyframeInfoUI::setKeyframedPosition(){
    if (bsData && file){
        (bsData->keyframedPosition != keyframedPosition->value()) ? bsData->keyframedPosition = keyframedPosition->value(), file->setIsChanged(true) : LogFile::writeToLog("KeyframeInfoUI::setKeyframedPosition(): keyframedPosition not set!!");
    }else{
        LogFile::writeToLog("KeyframeInfoUI::setkeyframedPosition(): Behavior file or data is null!!!");
    }
}

void KeyframeInfoUI::setKeyframedRotation(){
    if (bsData && file){
        (bsData->keyframedRotation != keyframedRotation->value()) ? bsData->keyframedRotation = keyframedRotation->value(), file->setIsChanged(true) : LogFile::writeToLog("KeyframeInfoUI::setKeyframedRotation(): keyframedRotation not set!!");
    }else{
        LogFile::writeToLog("KeyframeInfoUI::setkeyframedRotation(): Behavior file or data is null!!!");
    }
}

void KeyframeInfoUI::setBoneIndex(int index){
    auto boneindex = --index;
    (bsData && file && boneindex != bsData->boneIndex) ? bsData->boneIndex = boneindex, file->setIsChanged(true) : LogFile::writeToLog("KeyframeInfoUI::setBoneIndex(): boneIndex was not set!!!");
}

void KeyframeInfoUI::setIsValid(){
    if (bsData && file){
        (bsData->isValid != isValid->isChecked()) ? bsData->isValid = isValid->isChecked(), file->setIsChanged(true) : LogFile::writeToLog("KeyframeInfoUI::setIsValid(): isValid not set!!");
    }else{
        LogFile::writeToLog("KeyframeInfoUI::setIsAnnotation(): Behavior file or data is null!!!");
    }
}

void KeyframeInfoUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case KEYFRAMED_POSITION_ROW:
                checkisproperty(KEYFRAMED_POSITION_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedPosition"); break;
            case KEYFRAMED_ROTATION_ROW:
                checkisproperty(KEYFRAMED_ROTATION_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedRotation"); break;
            case BONE_INDEX_ROW:
                checkisproperty(BONE_INDEX_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/boneIndex"); break;
            case IS_VALID_ROW:
                checkisproperty(IS_VALID_ROW, "keyframeInfo:"+QString::number(keyframeIndex)+"/isValid"); break;
            }
        }
    }else{
        LogFile::writeToLog("KeyframeInfoUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void KeyframeInfoUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData){
        if (viewproperties){
            if (parent->getVariableBindingSetData()){
                emit viewProperties(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (parent->getVariableBindingSetData()){
                emit viewVariables(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("KeyframeInfoUI::selectTableToView(): The data is nullptr!!");
    }
}

void KeyframeInfoUI::variableRenamed(const QString & name, int index){
    if (parent){
        index--;
        auto bind = parent->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedPosition", KEYFRAMED_POSITION_ROW);
            setname("keyframeInfo:"+QString::number(keyframeIndex)+"/keyframedRotation", KEYFRAMED_ROTATION_ROW);
            setname("keyframeInfo:"+QString::number(keyframeIndex)+"/boneIndex", BONE_INDEX_ROW);
            setname("keyframeInfo:"+QString::number(keyframeIndex)+"/isValid", IS_VALID_ROW);
        }
    }else{
        LogFile::writeToLog("KeyframeInfoUI::variableRenamed(): The data is nullptr!!");
    }
}
