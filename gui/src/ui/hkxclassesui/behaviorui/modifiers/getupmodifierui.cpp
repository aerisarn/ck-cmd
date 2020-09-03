#include "getupmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbgetupmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 8

#define NAME_ROW 0
#define ENABLE_ROW 1
#define GROUND_NORMAL_ROW 2
#define DURATION_ROW 3
#define ALIGN_WITH_GROUND_DURATION_ROW 4
#define ROOT_BONE_INDEX_ROW 5
#define OTHER_BONE_INDEX_ROW 6
#define ANOTHER_BONE_INDEX_ROW 7

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList GetUpModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

GetUpModifierUI::GetUpModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      groundNormal(new QuadVariableWidget),
      duration(new DoubleSpinBox),
      alignWithGroundDuration(new DoubleSpinBox),
      rootBoneIndex(new ComboBox),
      otherBoneIndex(new ComboBox),
      anotherBoneIndex(new ComboBox)
{
    setTitle("hkbGetUpModifier");
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
    table->setItem(GROUND_NORMAL_ROW, NAME_COLUMN, new TableWidgetItem("groundNormal"));
    table->setItem(GROUND_NORMAL_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(GROUND_NORMAL_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GROUND_NORMAL_ROW, VALUE_COLUMN, groundNormal);
    table->setItem(DURATION_ROW, NAME_COLUMN, new TableWidgetItem("duration"));
    table->setItem(DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DURATION_ROW, VALUE_COLUMN, duration);
    table->setItem(ALIGN_WITH_GROUND_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("alignWithGroundDuration"));
    table->setItem(ALIGN_WITH_GROUND_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ALIGN_WITH_GROUND_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ALIGN_WITH_GROUND_DURATION_ROW, VALUE_COLUMN, alignWithGroundDuration);
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
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void GetUpModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(groundNormal, SIGNAL(editingFinished()), this, SLOT(setGroundNormal()), Qt::UniqueConnection);
        connect(duration, SIGNAL(editingFinished()), this, SLOT(setDuration()), Qt::UniqueConnection);
        connect(alignWithGroundDuration, SIGNAL(editingFinished()), this, SLOT(setAlignWithGroundDuration()), Qt::UniqueConnection);
        connect(rootBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setRootBoneIndex(int)), Qt::UniqueConnection);
        connect(otherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setOtherBoneIndex(int)), Qt::UniqueConnection);
        connect(anotherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnotherBoneIndex(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(groundNormal, SIGNAL(editingFinished()), this, SLOT(setGroundNormal()));
        disconnect(duration, SIGNAL(editingFinished()), this, SLOT(setDuration()));
        disconnect(alignWithGroundDuration, SIGNAL(editingFinished()), this, SLOT(setAlignWithGroundDuration()));
        disconnect(rootBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setRootBoneIndex()));
        disconnect(otherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setOtherBoneIndex()));
        disconnect(anotherBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnotherBoneIndex()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void GetUpModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("GetUpModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void GetUpModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_GET_UP_MODIFIER){
            bsData = static_cast<hkbGetUpModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            groundNormal->setValue(bsData->getGroundNormal());
            duration->setValue(bsData->getDuration());
            alignWithGroundDuration->setValue(bsData->getAlignWithGroundDuration());
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRagdollBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(rootBoneIndex, bsData->getRootBoneIndex() + 1);
            loadbones(otherBoneIndex, bsData->getOtherBoneIndex() + 1);
            loadbones(anotherBoneIndex, bsData->getAnotherBoneIndex() + 1);
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(GROUND_NORMAL_ROW, BINDING_COLUMN, varBind, "groundNormal", table, bsData);
            UIHelper::loadBinding(DURATION_ROW, BINDING_COLUMN, varBind, "duration", table, bsData);
            UIHelper::loadBinding(ALIGN_WITH_GROUND_DURATION_ROW, BINDING_COLUMN, varBind, "alignWithGroundDuration", table, bsData);
            UIHelper::loadBinding(ROOT_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "rootBoneIndex", table, bsData);
            UIHelper::loadBinding(OTHER_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "otherBoneIndex", table, bsData);
            UIHelper::loadBinding(ANOTHER_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "anotherBoneIndex", table, bsData);
        }else{
            LogFile::writeToLog("GetUpModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("GetUpModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void GetUpModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("GetUpModifierUI::setName(): The data is nullptr!!");
    }
}

void GetUpModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("GetUpModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void GetUpModifierUI::setGroundNormal(){
    (bsData) ? bsData->setGroundNormal(groundNormal->value()) : LogFile::writeToLog("GetUpModifierUI::setGroundNormal(): The data is nullptr!!");
}

void GetUpModifierUI::setDuration(){
    (bsData) ? bsData->setDuration(duration->value()) : LogFile::writeToLog("GetUpModifierUI::setDuration(): The data is nullptr!!");
}

void GetUpModifierUI::setAlignWithGroundDuration(){
    (bsData) ? bsData->setAlignWithGroundDuration(alignWithGroundDuration->value()) : LogFile::writeToLog("GetUpModifierUI::setAlignWithGroundDuration(): The data is nullptr!!");
}

void GetUpModifierUI::setRootBoneIndex(int index){
    (bsData) ? bsData->setRootBoneIndex(index - 1) : LogFile::writeToLog("GetUpModifierUI::setRootBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void GetUpModifierUI::setOtherBoneIndex(int index){
    (bsData) ? bsData->setOtherBoneIndex(index - 1) : LogFile::writeToLog("GetUpModifierUI::setOtherBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void GetUpModifierUI::setAnotherBoneIndex(int index){
    (bsData) ? bsData->setAnotherBoneIndex(index - 1) : LogFile::writeToLog("GetUpModifierUI::setAnotherBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void GetUpModifierUI::viewSelected(int row, int column){
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
            case GROUND_NORMAL_ROW:
                checkisproperty(GROUND_NORMAL_ROW, "groundNormal"); break;
            case DURATION_ROW:
                checkisproperty(DURATION_ROW, "duration"); break;
            case ALIGN_WITH_GROUND_DURATION_ROW:
                checkisproperty(ALIGN_WITH_GROUND_DURATION_ROW, "alignWithGroundDuration"); break;
            case ROOT_BONE_INDEX_ROW:
                checkisproperty(ROOT_BONE_INDEX_ROW, "rootBoneIndex"); break;
            case OTHER_BONE_INDEX_ROW:
                checkisproperty(OTHER_BONE_INDEX_ROW, "otherBoneIndex"); break;
            case ANOTHER_BONE_INDEX_ROW:
                checkisproperty(ANOTHER_BONE_INDEX_ROW, "anotherBoneIndex"); break;
            }
        }
    }else{
        LogFile::writeToLog("GetUpModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void GetUpModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("GetUpModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void GetUpModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("groundNormal", GROUND_NORMAL_ROW);
            setname("duration", DURATION_ROW);
            setname("alignWithGroundDuration", ALIGN_WITH_GROUND_DURATION_ROW);
            setname("rootBoneIndex", ROOT_BONE_INDEX_ROW);
            setname("otherBoneIndex", OTHER_BONE_INDEX_ROW);
            setname("anotherBoneIndex", ANOTHER_BONE_INDEX_ROW);
        }
    }else{
        LogFile::writeToLog("GetUpModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void GetUpModifierUI::setBindingVariable(int index, const QString &name){
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
        case GROUND_NORMAL_ROW:
            checkisproperty(GROUND_NORMAL_ROW, "groundNormal", VARIABLE_TYPE_VECTOR4); break;
        case DURATION_ROW:
            checkisproperty(DURATION_ROW, "duration", VARIABLE_TYPE_REAL); break;
        case ALIGN_WITH_GROUND_DURATION_ROW:
            checkisproperty(ALIGN_WITH_GROUND_DURATION_ROW, "alignWithGroundDuration", VARIABLE_TYPE_REAL); break;
        case ROOT_BONE_INDEX_ROW:
            checkisproperty(ROOT_BONE_INDEX_ROW, "rootBoneIndex", VARIABLE_TYPE_INT32); break;
        case OTHER_BONE_INDEX_ROW:
            checkisproperty(OTHER_BONE_INDEX_ROW, "otherBoneIndex", VARIABLE_TYPE_INT32); break;
        case ANOTHER_BONE_INDEX_ROW:
            checkisproperty(ANOTHER_BONE_INDEX_ROW, "anotherBoneIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("GetUpModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
