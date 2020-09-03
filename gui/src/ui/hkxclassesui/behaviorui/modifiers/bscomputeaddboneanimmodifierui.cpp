#include "bscomputeaddboneanimmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/bscomputeaddboneanimmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define BONE_INDEX_ROW 2
#define TRANSLATION_LS_OUT_ROW 3
#define ROTATION_LS_OUT_ROW 4
#define SCALE_LS_OUT_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSComputeAddBoneAnimModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSComputeAddBoneAnimModifierUI::BSComputeAddBoneAnimModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      boneIndex(new ComboBox),
      translationLSOut(new QuadVariableWidget),
      rotationLSOut(new QuadVariableWidget),
      scaleLSOut(new QuadVariableWidget)
{
    setTitle("BSComputeAddBoneAnimModifier");
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
    table->setItem(BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("boneIndex"));
    table->setItem(BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BONE_INDEX_ROW, VALUE_COLUMN, boneIndex);
    table->setItem(TRANSLATION_LS_OUT_ROW, NAME_COLUMN, new TableWidgetItem("translationLSOut"));
    table->setItem(TRANSLATION_LS_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TRANSLATION_LS_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TRANSLATION_LS_OUT_ROW, VALUE_COLUMN, translationLSOut);
    table->setItem(ROTATION_LS_OUT_ROW, NAME_COLUMN, new TableWidgetItem("rotationLSOut"));
    table->setItem(ROTATION_LS_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ROTATION_LS_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROTATION_LS_OUT_ROW, VALUE_COLUMN, rotationLSOut);
    table->setItem(SCALE_LS_OUT_ROW, NAME_COLUMN, new TableWidgetItem("scaleLSOut"));
    table->setItem(SCALE_LS_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SCALE_LS_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SCALE_LS_OUT_ROW, VALUE_COLUMN, scaleLSOut);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSComputeAddBoneAnimModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(boneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoneIndex(int)), Qt::UniqueConnection);
        connect(translationLSOut, SIGNAL(editingFinished()), this, SLOT(setTranslationLSOut()), Qt::UniqueConnection);
        connect(rotationLSOut, SIGNAL(editingFinished()), this, SLOT(setRotationLSOut()), Qt::UniqueConnection);
        connect(scaleLSOut, SIGNAL(editingFinished()), this, SLOT(setScaleLSOut()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(boneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoneIndex(int)));
        disconnect(translationLSOut, SIGNAL(editingFinished()), this, SLOT(setTranslationLSOut()));
        disconnect(rotationLSOut, SIGNAL(editingFinished()), this, SLOT(setRotationLSOut()));
        disconnect(scaleLSOut, SIGNAL(editingFinished()), this, SLOT(setScaleLSOut()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSComputeAddBoneAnimModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSComputeAddBoneAnimModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_COMPUTE_ADD_BONE_ANIM_MODIFIER){
            bsData = static_cast<BSComputeAddBoneAnimModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->enable);
            if (!boneIndex->count()){
                auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                boneIndex->insertItems(0, boneNames);
            }
            boneIndex->setCurrentIndex(bsData->getBoneIndex() + 1);
            translationLSOut->setValue(bsData->getTranslationLSOut());
            rotationLSOut->setValue(bsData->getRotationLSOut());
            scaleLSOut->setValue(bsData->getScaleLSOut());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(BONE_INDEX_ROW, BINDING_COLUMN, varBind, "boneIndex", table, bsData);
            UIHelper::loadBinding(TRANSLATION_LS_OUT_ROW, BINDING_COLUMN, varBind, "translationLSOut", table, bsData);
            UIHelper::loadBinding(ROTATION_LS_OUT_ROW, BINDING_COLUMN, varBind, "rotationLSOut", table, bsData);
            UIHelper::loadBinding(SCALE_LS_OUT_ROW, BINDING_COLUMN, varBind, "scaleLSOut", table, bsData);
        }else{
            LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSComputeAddBoneAnimModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setName(): The data is nullptr!!");
    }
}

void BSComputeAddBoneAnimModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void BSComputeAddBoneAnimModifierUI::setBoneIndex(int index){
    (bsData) ? bsData->setBoneIndex(index - 1) : LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void BSComputeAddBoneAnimModifierUI::setTranslationLSOut(){
    (bsData) ? bsData->setTranslationLSOut(translationLSOut->value()) : LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setTranslationLSOut(): The 'bsData' pointer is nullptr!!");
}

void BSComputeAddBoneAnimModifierUI::setRotationLSOut(){
    (bsData) ? bsData->setRotationLSOut(rotationLSOut->value()) : LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setRotationLSOut(): The 'bsData' pointer is nullptr!!");
}

void BSComputeAddBoneAnimModifierUI::setScaleLSOut(){
    (bsData) ? bsData->setScaleLSOut(scaleLSOut->value()) : LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setScaleLSOut(): The 'bsData' pointer is nullptr!!");
}

void BSComputeAddBoneAnimModifierUI::viewSelected(int row, int column){
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
            case BONE_INDEX_ROW:
                checkisproperty(BONE_INDEX_ROW, "boneIndex"); break;
            case TRANSLATION_LS_OUT_ROW:
                checkisproperty(TRANSLATION_LS_OUT_ROW, "translationLSOut"); break;
            case ROTATION_LS_OUT_ROW:
                checkisproperty(ROTATION_LS_OUT_ROW, "rotationLSOut"); break;
            case SCALE_LS_OUT_ROW:
                checkisproperty(SCALE_LS_OUT_ROW, "scaleLSOut"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSComputeAddBoneAnimModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSComputeAddBoneAnimModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("boneIndex", BONE_INDEX_ROW);
            setname("translationLSOut", TRANSLATION_LS_OUT_ROW);
            setname("rotationLSOut", ROTATION_LS_OUT_ROW);
            setname("scaleLSOut", SCALE_LS_OUT_ROW);
        }
    }else{
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSComputeAddBoneAnimModifierUI::setBindingVariable(int index, const QString &name){
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
        case BONE_INDEX_ROW:
            checkisproperty(BONE_INDEX_ROW, "boneIndex", VARIABLE_TYPE_INT32); break;
        case TRANSLATION_LS_OUT_ROW:
            checkisproperty(TRANSLATION_LS_OUT_ROW, "translationLSOut", VARIABLE_TYPE_VECTOR4); break;
        case ROTATION_LS_OUT_ROW:
            checkisproperty(ROTATION_LS_OUT_ROW, "rotationLSOut", VARIABLE_TYPE_QUATERNION); break;
        case SCALE_LS_OUT_ROW:
            checkisproperty(SCALE_LS_OUT_ROW, "scaleLSOut", VARIABLE_TYPE_VECTOR4); break;
        }
    }else{
        LogFile::writeToLog("BSComputeAddBoneAnimModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
