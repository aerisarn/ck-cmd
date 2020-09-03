#include "bslimbikmodifierui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/bslimbikmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 8

#define NAME_ROW 0
#define ENABLE_ROW 1
#define LIMIT_ANGLE_DEGREES_ROW 2
#define START_BONE_INDEX_ROW 3
#define END_BONE_INDEX_ROW 4
#define GAIN_ROW 5
#define BONE_RADIUS_ROW 6
#define CAST_OFFSET_ROW 7

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSLimbIKModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSLimbIKModifierUI::BSLimbIKModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      limitAngleDegrees(new DoubleSpinBox),
      startBoneIndex(new ComboBox),
      endBoneIndex(new ComboBox),
      gain(new DoubleSpinBox),
      boneRadius(new DoubleSpinBox),
      castOffset(new DoubleSpinBox)
{
    setTitle("BSLimbIKModifier");
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
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleDegrees"));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_DEGREES_ROW, VALUE_COLUMN, limitAngleDegrees);
    table->setItem(START_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("startBoneIndex"));
    table->setItem(START_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(START_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(START_BONE_INDEX_ROW, VALUE_COLUMN, startBoneIndex);
    table->setItem(END_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("endBoneIndex"));
    table->setItem(END_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(END_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(END_BONE_INDEX_ROW, VALUE_COLUMN, endBoneIndex);
    table->setItem(GAIN_ROW, NAME_COLUMN, new TableWidgetItem("gain"));
    table->setItem(GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GAIN_ROW, VALUE_COLUMN, gain);
    table->setItem(BONE_RADIUS_ROW, NAME_COLUMN, new TableWidgetItem("boneRadius"));
    table->setItem(BONE_RADIUS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(BONE_RADIUS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BONE_RADIUS_ROW, VALUE_COLUMN, boneRadius);
    table->setItem(CAST_OFFSET_ROW, NAME_COLUMN, new TableWidgetItem("castOffset"));
    table->setItem(CAST_OFFSET_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CAST_OFFSET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CAST_OFFSET_ROW, VALUE_COLUMN, castOffset);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSLimbIKModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()), Qt::UniqueConnection);
        connect(startBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartBoneIndex(int)), Qt::UniqueConnection);
        connect(endBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndBoneIndex(int)), Qt::UniqueConnection);
        connect(gain, SIGNAL(editingFinished()), this, SLOT(setGain()), Qt::UniqueConnection);
        connect(boneRadius, SIGNAL(editingFinished()), this, SLOT(setBoneRadius()), Qt::UniqueConnection);
        connect(castOffset, SIGNAL(editingFinished()), this, SLOT(setCastOffset()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()));
        disconnect(startBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartBoneIndex(int)));
        disconnect(endBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndBoneIndex(int)));
        disconnect(gain, SIGNAL(editingFinished()), this, SLOT(setGain()));
        disconnect(boneRadius, SIGNAL(editingFinished()), this, SLOT(setBoneRadius()));
        disconnect(castOffset, SIGNAL(editingFinished()), this, SLOT(setCastOffset()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSLimbIKModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSLimbIKModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSLimbIKModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_LIMB_IK_MODIFIER){
            bsData = static_cast<BSLimbIKModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            limitAngleDegrees->setValue(bsData->getLimitAngleDegrees());
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(startBoneIndex, bsData->getStartBoneIndex() + 1);
            loadbones(endBoneIndex, bsData->getEndBoneIndex() + 1);
            gain->setValue(bsData->getGain());
            boneRadius->setValue(bsData->getBoneRadius());
            castOffset->setValue(bsData->getCastOffset());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, varBind, "limitAngleDegrees", table, bsData);
            UIHelper::loadBinding(START_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "startBoneIndex", table, bsData);
            UIHelper::loadBinding(END_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "endBoneIndex", table, bsData);
            UIHelper::loadBinding(GAIN_ROW, BINDING_COLUMN, varBind, "gain", table, bsData);
            UIHelper::loadBinding(BONE_RADIUS_ROW, BINDING_COLUMN, varBind, "boneRadius", table, bsData);
            UIHelper::loadBinding(CAST_OFFSET_ROW, BINDING_COLUMN, varBind, "castOffset", table, bsData);
        }else{
            LogFile::writeToLog("BSLimbIKModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSLimbIKModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSLimbIKModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSLimbIKModifierUI::setName(): The data is nullptr!!");
    }
}

void BSLimbIKModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSLimbIKModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::setLimitAngleDegrees(){
    (bsData) ? bsData->setLimitAngleDegrees(limitAngleDegrees->value()) : LogFile::writeToLog("BSLimbIKModifierUI::setLimitAngleDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::setStartBoneIndex(int index){
    (bsData) ? bsData->setStartBoneIndex(index - 1) : LogFile::writeToLog("BSLimbIKModifierUI::setStartBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::setEndBoneIndex(int index){
    (bsData) ? bsData->setEndBoneIndex(index - 1) : LogFile::writeToLog("BSLimbIKModifierUI::setEndBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::setGain(){
    (bsData) ? bsData->setGain(gain->value()) : LogFile::writeToLog("BSLimbIKModifierUI::setGain(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::setBoneRadius(){
    (bsData) ? bsData->setBoneRadius(boneRadius->value()) : LogFile::writeToLog("BSLimbIKModifierUI::setBoneRadius(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::setCastOffset(){
    (bsData) ? bsData->setCastOffset(castOffset->value()) : LogFile::writeToLog("BSLimbIKModifierUI::setCastOffset(): The 'bsData' pointer is nullptr!!");
}

void BSLimbIKModifierUI::viewSelected(int row, int column){
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
            case LIMIT_ANGLE_DEGREES_ROW:
                checkisproperty(LIMIT_ANGLE_DEGREES_ROW, "limitAngleDegrees"); break;
            case START_BONE_INDEX_ROW:
                checkisproperty(START_BONE_INDEX_ROW, "startBoneIndex"); break;
            case END_BONE_INDEX_ROW:
                checkisproperty(END_BONE_INDEX_ROW, "endBoneIndex"); break;
            case GAIN_ROW:
                checkisproperty(GAIN_ROW, "gain"); break;
            case BONE_RADIUS_ROW:
                checkisproperty(BONE_RADIUS_ROW, "boneRadius"); break;
            case CAST_OFFSET_ROW:
                checkisproperty(CAST_OFFSET_ROW, "castOffset"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSLimbIKModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSLimbIKModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSLimbIKModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSLimbIKModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("limitAngleDegrees", LIMIT_ANGLE_DEGREES_ROW);
            setname("startBoneIndex", START_BONE_INDEX_ROW);
            setname("endBoneIndex", END_BONE_INDEX_ROW);
            setname("gain", GAIN_ROW);
            setname("boneRadius", BONE_RADIUS_ROW);
            setname("castOffset", CAST_OFFSET_ROW);
        }
    }else{
        LogFile::writeToLog("BSLimbIKModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSLimbIKModifierUI::setBindingVariable(int index, const QString &name){
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
        case LIMIT_ANGLE_DEGREES_ROW:
            checkisproperty(LIMIT_ANGLE_DEGREES_ROW, "limitAngleDegrees", VARIABLE_TYPE_REAL); break;
        case START_BONE_INDEX_ROW:
            checkisproperty(START_BONE_INDEX_ROW, "startBoneIndex", VARIABLE_TYPE_INT32); break;
        case END_BONE_INDEX_ROW:
            checkisproperty(END_BONE_INDEX_ROW, "endBoneIndex", VARIABLE_TYPE_INT32); break;
        case GAIN_ROW:
            checkisproperty(GAIN_ROW, "gain", VARIABLE_TYPE_REAL); break;
        case BONE_RADIUS_ROW:
            checkisproperty(BONE_RADIUS_ROW, "boneRadius", VARIABLE_TYPE_REAL); break;
        case CAST_OFFSET_ROW:
            checkisproperty(CAST_OFFSET_ROW, "castOffset", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSLimbIKModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
