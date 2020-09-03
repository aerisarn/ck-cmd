#include "bsmodifyoncemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/behavior/modifiers/BSModifyOnceModifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbmodifier.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/genericdatawidgets.h"

#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ON_ACTIVATE_MODIFIER_ROW 2
#define ON_DEACTIVATE_MODIFIER_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSModifyOnceModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSModifyOnceModifierUI::BSModifyOnceModifierUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox)
{
    setTitle("BSModifyOnceModifier");
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
    table->setItem(ON_ACTIVATE_MODIFIER_ROW, NAME_COLUMN, new TableWidgetItem("pOnActivateModifier"));
    table->setItem(ON_ACTIVATE_MODIFIER_ROW, TYPE_COLUMN, new TableWidgetItem("hkbModifier", Qt::AlignCenter));
    table->setItem(ON_ACTIVATE_MODIFIER_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(ON_ACTIVATE_MODIFIER_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_MODIFIERS_TABLE_TIP));
    table->setItem(ON_DEACTIVATE_MODIFIER_ROW, NAME_COLUMN, new TableWidgetItem("pOnDeactivateModifier"));
    table->setItem(ON_DEACTIVATE_MODIFIER_ROW, TYPE_COLUMN, new TableWidgetItem("hkbModifier", Qt::AlignCenter));
    table->setItem(ON_DEACTIVATE_MODIFIER_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(ON_DEACTIVATE_MODIFIER_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSModifyOnceModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSModifyOnceModifierUI::connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties && modifiers){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(modifiers, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(modifiers, SIGNAL(elementSelected(int,QString)), this, SLOT(setModifier(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewModifiers(int,QString,QStringList)), modifiers, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSModifyOnceModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_MODIFY_ONCE_MODIFIER){
            bsData = static_cast<BSModifyOnceModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto settext = [&](hkbModifier *mod, int row){
                (mod) ? table->item(row, VALUE_COLUMN)->setText(mod->getName()) : table->item(row, VALUE_COLUMN)->setText("NONE");
            };
            settext(bsData->getPOnActivateModifier(), ON_ACTIVATE_MODIFIER_ROW);
            settext(bsData->getPOnDeactivateModifier(), ON_DEACTIVATE_MODIFIER_ROW);
        }else{
            LogFile::writeToLog(QString("BSModifyOnceModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BSModifyOnceModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::setName(): The data is nullptr!!");
    }
}

void BSModifyOnceModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSModifyOnceModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void BSModifyOnceModifierUI::setModifier(int index, const QString &name){
    if (bsData){
        if (table->currentRow() == ON_ACTIVATE_MODIFIER_ROW){
            UIHelper::setModifier(index, name, bsData, static_cast<hkbModifier *>(bsData->pOnActivateModifier.data()), NULL_SIGNATURE, HkxObject::TYPE_MODIFIER, table, behaviorView, ON_ACTIVATE_MODIFIER_ROW, VALUE_COLUMN);
        }else if (table->currentRow() == ON_DEACTIVATE_MODIFIER_ROW){
            UIHelper::setModifier(index, name, bsData, static_cast<hkbModifier *>(bsData->pOnDeactivateModifier.data()), NULL_SIGNATURE, HkxObject::TYPE_MODIFIER, table, behaviorView, ON_DEACTIVATE_MODIFIER_ROW, VALUE_COLUMN);
        }
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::setModifier(): The 'bsData' pointer is nullptr!!");
    }
}

void BSModifyOnceModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BSModifyOnceModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSModifyOnceModifierUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto isProperty = false;
        auto row = table->currentRow();
        switch (row){
        case ENABLE_ROW:
            (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : NULL;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, "enable", VARIABLE_TYPE_BOOL, isProperty, table, bsData);
        }
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void BSModifyOnceModifierUI::viewSelected(int row, int column){
    if (bsData){
        if (column == VALUE_COLUMN){
            if (row == ON_ACTIVATE_MODIFIER_ROW){
                emit viewModifiers(static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getPOnActivateModifier()) + 1, QString(), QStringList());
            }else if (row == ON_DEACTIVATE_MODIFIER_ROW){
                emit viewModifiers(static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getPOnDeactivateModifier()) + 1, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSModifyOnceModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto bindIndex = bind->getVariableIndexOfBinding("enable");
            (bindIndex == index) ? table->item(ENABLE_ROW, BINDING_COLUMN)->setText(name) : NULL;
        }
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSModifyOnceModifierUI::modifierRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getPOnActivateModifier())){
            table->item(ON_ACTIVATE_MODIFIER_ROW, VALUE_COLUMN)->setText(name);
        }
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getPOnDeactivateModifier())){
            table->item(ON_DEACTIVATE_MODIFIER_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("BSModifyOnceModifierUI::modifierRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSModifyOnceModifierUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}
