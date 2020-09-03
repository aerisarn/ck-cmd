#include "bsisactivemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/bsisactivemodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 12

#define NAME_ROW 0
#define ENABLE_ROW 1
#define IS_ACTIVE_0_ROW 2
#define INVERT_ACTIVE_0_ROW 3
#define IS_ACTIVE_1_ROW 4
#define INVERT_ACTIVE_1_ROW 5
#define IS_ACTIVE_2_ROW 6
#define INVERT_ACTIVE_2_ROW 7
#define IS_ACTIVE_3_ROW 8
#define INVERT_ACTIVE_3_ROW 9
#define IS_ACTIVE_4_ROW 10
#define INVERT_ACTIVE_4_ROW 11

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSIsActiveModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSIsActiveModifierUI::BSIsActiveModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      bIsActive0(new CheckBox),
      bInvertActive0(new CheckBox),
      bIsActive1(new CheckBox),
      bInvertActive1(new CheckBox),
      bIsActive2(new CheckBox),
      bInvertActive2(new CheckBox),
      bIsActive3(new CheckBox),
      bInvertActive3(new CheckBox),
      bIsActive4(new CheckBox),
      bInvertActive4(new CheckBox)
{
    setTitle("BSIsActiveModifier");
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
    table->setItem(IS_ACTIVE_0_ROW, NAME_COLUMN, new TableWidgetItem("bIsActive0"));
    table->setItem(IS_ACTIVE_0_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ACTIVE_0_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ACTIVE_0_ROW, VALUE_COLUMN, bIsActive0);
    table->setItem(INVERT_ACTIVE_0_ROW, NAME_COLUMN, new TableWidgetItem("bInvertActive0"));
    table->setItem(INVERT_ACTIVE_0_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_ACTIVE_0_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_ACTIVE_0_ROW, VALUE_COLUMN, bInvertActive0);
    table->setItem(IS_ACTIVE_1_ROW, NAME_COLUMN, new TableWidgetItem("bIsActive1"));
    table->setItem(IS_ACTIVE_1_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ACTIVE_1_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ACTIVE_1_ROW, VALUE_COLUMN, bIsActive1);
    table->setItem(INVERT_ACTIVE_1_ROW, NAME_COLUMN, new TableWidgetItem("bInvertActive1"));
    table->setItem(INVERT_ACTIVE_1_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_ACTIVE_1_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_ACTIVE_1_ROW, VALUE_COLUMN, bInvertActive1);
    table->setItem(IS_ACTIVE_2_ROW, NAME_COLUMN, new TableWidgetItem("bIsActive2"));
    table->setItem(IS_ACTIVE_2_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ACTIVE_2_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ACTIVE_2_ROW, VALUE_COLUMN, bIsActive2);
    table->setItem(INVERT_ACTIVE_2_ROW, NAME_COLUMN, new TableWidgetItem("bInvertActive2"));
    table->setItem(INVERT_ACTIVE_2_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_ACTIVE_2_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_ACTIVE_2_ROW, VALUE_COLUMN, bInvertActive2);
    table->setItem(IS_ACTIVE_3_ROW, NAME_COLUMN, new TableWidgetItem("bIsActive3"));
    table->setItem(IS_ACTIVE_3_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ACTIVE_3_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ACTIVE_3_ROW, VALUE_COLUMN, bIsActive3);
    table->setItem(INVERT_ACTIVE_3_ROW, NAME_COLUMN, new TableWidgetItem("bInvertActive3"));
    table->setItem(INVERT_ACTIVE_3_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_ACTIVE_3_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_ACTIVE_3_ROW, VALUE_COLUMN, bInvertActive3);
    table->setItem(IS_ACTIVE_4_ROW, NAME_COLUMN, new TableWidgetItem("bIsActive4"));
    table->setItem(IS_ACTIVE_4_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ACTIVE_4_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ACTIVE_4_ROW, VALUE_COLUMN, bIsActive4);
    table->setItem(INVERT_ACTIVE_4_ROW, NAME_COLUMN, new TableWidgetItem("bInvertActive4"));
    table->setItem(INVERT_ACTIVE_4_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_ACTIVE_4_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_ACTIVE_4_ROW, VALUE_COLUMN, bInvertActive4);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSIsActiveModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(bIsActive0, SIGNAL(released()), this, SLOT(setIsActive0()), Qt::UniqueConnection);
        connect(bInvertActive0, SIGNAL(released()), this, SLOT(setInvertActive0()), Qt::UniqueConnection);
        connect(bIsActive1, SIGNAL(released()), this, SLOT(setIsActive1()), Qt::UniqueConnection);
        connect(bInvertActive1, SIGNAL(released()), this, SLOT(setInvertActive1()), Qt::UniqueConnection);
        connect(bIsActive2, SIGNAL(released()), this, SLOT(setIsActive2()), Qt::UniqueConnection);
        connect(bInvertActive2, SIGNAL(released()), this, SLOT(setInvertActive2()), Qt::UniqueConnection);
        connect(bIsActive3, SIGNAL(released()), this, SLOT(setIsActive3()), Qt::UniqueConnection);
        connect(bInvertActive3, SIGNAL(released()), this, SLOT(setInvertActive3()), Qt::UniqueConnection);
        connect(bIsActive4, SIGNAL(released()), this, SLOT(setIsActive4()), Qt::UniqueConnection);
        connect(bInvertActive4, SIGNAL(released()), this, SLOT(setInvertActive4()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(bIsActive0, SIGNAL(released()), this, SLOT(setIsActive0()));
        disconnect(bInvertActive0, SIGNAL(released()), this, SLOT(setInvertActive0()));
        disconnect(bIsActive1, SIGNAL(released()), this, SLOT(setIsActive1()));
        disconnect(bInvertActive1, SIGNAL(released()), this, SLOT(setInvertActive1()));
        disconnect(bIsActive2, SIGNAL(released()), this, SLOT(setIsActive2()));
        disconnect(bInvertActive2, SIGNAL(released()), this, SLOT(setInvertActive2()));
        disconnect(bIsActive3, SIGNAL(released()), this, SLOT(setIsActive3()));
        disconnect(bInvertActive3, SIGNAL(released()), this, SLOT(setInvertActive3()));
        disconnect(bIsActive4, SIGNAL(released()), this, SLOT(setIsActive4()));
        disconnect(bInvertActive4, SIGNAL(released()), this, SLOT(setInvertActive4()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSIsActiveModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSIsActiveModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSIsActiveModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_IS_ACTIVE_MODIFIER){
            bsData = static_cast<BSIsActiveModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            bIsActive0->setChecked(bsData->getBIsActive0());
            bInvertActive0->setChecked(bsData->getBInvertActive0());
            bIsActive1->setChecked(bsData->getBIsActive1());
            bInvertActive1->setChecked(bsData->getBInvertActive1());
            bIsActive2->setChecked(bsData->getBIsActive2());
            bInvertActive2->setChecked(bsData->getBInvertActive2());
            bIsActive3->setChecked(bsData->getBIsActive3());
            bInvertActive3->setChecked(bsData->getBInvertActive3());
            bIsActive4->setChecked(bsData->getBIsActive4());
            bInvertActive4->setChecked(bsData->getBInvertActive4());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(IS_ACTIVE_0_ROW, BINDING_COLUMN, varBind, "bIsActive0", table, bsData);
            UIHelper::loadBinding(INVERT_ACTIVE_0_ROW, BINDING_COLUMN, varBind, "bInvertActive0", table, bsData);
            UIHelper::loadBinding(IS_ACTIVE_1_ROW, BINDING_COLUMN, varBind, "bIsActive1", table, bsData);
            UIHelper::loadBinding(INVERT_ACTIVE_1_ROW, BINDING_COLUMN, varBind, "bInvertActive1", table, bsData);
            UIHelper::loadBinding(IS_ACTIVE_2_ROW, BINDING_COLUMN, varBind, "bIsActive2", table, bsData);
            UIHelper::loadBinding(INVERT_ACTIVE_2_ROW, BINDING_COLUMN, varBind, "bInvertActive2", table, bsData);
            UIHelper::loadBinding(IS_ACTIVE_3_ROW, BINDING_COLUMN, varBind, "bIsActive3", table, bsData);
            UIHelper::loadBinding(INVERT_ACTIVE_3_ROW, BINDING_COLUMN, varBind, "bInvertActive3", table, bsData);
            UIHelper::loadBinding(IS_ACTIVE_4_ROW, BINDING_COLUMN, varBind, "bIsActive4", table, bsData);
            UIHelper::loadBinding(INVERT_ACTIVE_4_ROW, BINDING_COLUMN, varBind, "bInvertActive4", table, bsData);
        }else{
            LogFile::writeToLog("BSIsActiveModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSIsActiveModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSIsActiveModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSIsActiveModifierUI::setName(): The data is nullptr!!");
    }
}

void BSIsActiveModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setEnable(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setIsActive0(){
    (bsData) ? bsData->setBIsActive0(bIsActive0->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setIsActive0(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setInvertActive0(){
    (bsData) ? bsData->setBInvertActive0(bInvertActive0->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setInvertActive0(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setIsActive1(){
    (bsData) ? bsData->setBIsActive1(bIsActive1->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setIsActive1(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setInvertActive1(){
    (bsData) ? bsData->setBInvertActive1(bInvertActive1->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setInvertActive1(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setIsActive2(){
    (bsData) ? bsData->setBIsActive2(bIsActive2->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setIsActive2(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setInvertActive2(){
    (bsData) ? bsData->setBInvertActive2(bInvertActive2->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setInvertActive2(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setIsActive3(){
    (bsData) ? bsData->setBIsActive3(bIsActive3->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setIsActive3(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setInvertActive3(){
    (bsData) ? bsData->setBInvertActive3(bInvertActive3->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setInvertActive3(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setIsActive4(){
    (bsData) ? bsData->setBIsActive4(bIsActive4->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setIsActive4(): The data is nullptr!!");
}

void BSIsActiveModifierUI::setInvertActive4(){
    (bsData) ? bsData->setBInvertActive4(bInvertActive4->isChecked()) : LogFile::writeToLog("BSIsActiveModifierUI::setInvertActive4(): The data is nullptr!!");
}

void BSIsActiveModifierUI::viewSelected(int row, int column){
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
            case IS_ACTIVE_0_ROW:
                checkisproperty(IS_ACTIVE_0_ROW, "bIsActive0"); break;
            case INVERT_ACTIVE_0_ROW:
                checkisproperty(INVERT_ACTIVE_0_ROW, "bInvertActive0"); break;
            case IS_ACTIVE_1_ROW:
                checkisproperty(IS_ACTIVE_1_ROW, "bIsActive1"); break;
            case INVERT_ACTIVE_1_ROW:
                checkisproperty(INVERT_ACTIVE_1_ROW, "bInvertActive1"); break;
            case IS_ACTIVE_2_ROW:
                checkisproperty(IS_ACTIVE_2_ROW, "bIsActive2"); break;
            case INVERT_ACTIVE_2_ROW:
                checkisproperty(INVERT_ACTIVE_2_ROW, "bInvertActive2"); break;
            case IS_ACTIVE_3_ROW:
                checkisproperty(IS_ACTIVE_3_ROW, "bIsActive3"); break;
            case INVERT_ACTIVE_3_ROW:
                checkisproperty(INVERT_ACTIVE_3_ROW, "bInvertActive3"); break;
            case IS_ACTIVE_4_ROW:
                checkisproperty(IS_ACTIVE_4_ROW, "bIsActive4"); break;
            case INVERT_ACTIVE_4_ROW:
                checkisproperty(INVERT_ACTIVE_4_ROW, "bInvertActive4"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSIsActiveModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSIsActiveModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSIsActiveModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSIsActiveModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("bIsActive0", IS_ACTIVE_0_ROW);
            setname("bInvertActive0", INVERT_ACTIVE_0_ROW);
            setname("bIsActive1", IS_ACTIVE_1_ROW);
            setname("bInvertActive1", INVERT_ACTIVE_1_ROW);
            setname("bIsActive2", IS_ACTIVE_2_ROW);
            setname("bInvertActive2", INVERT_ACTIVE_2_ROW);
            setname("bIsActive3", IS_ACTIVE_3_ROW);
            setname("bInvertActive3", INVERT_ACTIVE_3_ROW);
            setname("bIsActive4", IS_ACTIVE_4_ROW);
            setname("bInvertActive4", INVERT_ACTIVE_4_ROW);
        }
    }else{
        LogFile::writeToLog("BSIsActiveModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSIsActiveModifierUI::setBindingVariable(int index, const QString &name){
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
        case IS_ACTIVE_0_ROW:
            checkisproperty(IS_ACTIVE_0_ROW, "bIsActive0", VARIABLE_TYPE_BOOL); break;
        case INVERT_ACTIVE_0_ROW:
            checkisproperty(INVERT_ACTIVE_0_ROW, "bInvertActive0", VARIABLE_TYPE_BOOL); break;
        case IS_ACTIVE_1_ROW:
            checkisproperty(IS_ACTIVE_1_ROW, "bIsActive1", VARIABLE_TYPE_BOOL); break;
        case INVERT_ACTIVE_1_ROW:
            checkisproperty(INVERT_ACTIVE_1_ROW, "bInvertActive1", VARIABLE_TYPE_BOOL); break;
        case IS_ACTIVE_2_ROW:
            checkisproperty(IS_ACTIVE_2_ROW, "bIsActive2", VARIABLE_TYPE_BOOL); break;
        case INVERT_ACTIVE_2_ROW:
            checkisproperty(INVERT_ACTIVE_2_ROW, "bInvertActive2", VARIABLE_TYPE_BOOL); break;
        case IS_ACTIVE_3_ROW:
            checkisproperty(IS_ACTIVE_3_ROW, "bIsActive3", VARIABLE_TYPE_BOOL); break;
        case INVERT_ACTIVE_3_ROW:
            checkisproperty(INVERT_ACTIVE_3_ROW, "bInvertActive3", VARIABLE_TYPE_BOOL); break;
        case IS_ACTIVE_4_ROW:
            checkisproperty(IS_ACTIVE_4_ROW, "bIsActive4", VARIABLE_TYPE_BOOL); break;
        case INVERT_ACTIVE_4_ROW:
            checkisproperty(INVERT_ACTIVE_4_ROW, "bInvertActive4", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("BSIsActiveModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
