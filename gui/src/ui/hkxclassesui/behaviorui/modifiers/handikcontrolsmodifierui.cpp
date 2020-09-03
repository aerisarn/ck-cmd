#include "handikcontrolsmodifierui.h"
#include "src/ui/hkxclassesui/behaviorui/handui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbHandIkControlsModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 3

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ADD_HAND_ROW 2

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList HandIkControlsModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

HandIkControlsModifierUI::HandIkControlsModifierUI()
    : bsData(nullptr),
      handUI(new HandUI),
      groupBox(new QGroupBox),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox)
{
    groupBox->setTitle("hkbHandIkControlsModifier");
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
    table->setItem(ADD_HAND_ROW, NAME_COLUMN, new TableWidgetItem("Add Hand", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new Hand"));
    table->setItem(ADD_HAND_ROW, TYPE_COLUMN, new TableWidgetItem("hkHand", Qt::AlignCenter));
    table->setItem(ADD_HAND_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Hand", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected Hand"));
    table->setItem(ADD_HAND_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Hand", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected Hand"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(handUI);
    toggleSignals(true);
}

void HandIkControlsModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(handUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(handUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(handUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(handUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void HandIkControlsModifierUI::addHand(){
    (bsData) ? bsData->addHand(), loadDynamicTableRows() : LogFile::writeToLog("HandIkControlsModifierUI::addHand(): The data is nullptr!!");
}

void HandIkControlsModifierUI::removeHand(int index){
    (bsData) ? bsData->removeHand(index), loadDynamicTableRows() : LogFile::writeToLog("FootIkControlsModifierUI::removeHand(): The data is nullptr!!");
}

void HandIkControlsModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_HAND_IK_CONTROLS_MODIFIER){
            bsData = static_cast<hkbHandIkControlsModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("HandIkControlsModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void HandIkControlsModifierUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_HAND_ROW + bsData->getNumberOfHands() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_HAND_ROW + 1, j = 0; j < bsData->getNumberOfHands(); i++, j++){
            UIHelper::setRowItems(i, "Hand "+QString::number(j), "hkHand", "Remove", "Edit", "Double click to remove this Hand", "Double click to edit this Hand", table);
        }
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void HandIkControlsModifierUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto isProperty = false;
        auto row = table->currentRow();
        if (row == ENABLE_ROW){
            (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : NULL;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, "enable", VARIABLE_TYPE_BOOL, isProperty, table, bsData);
        }
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void HandIkControlsModifierUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void HandIkControlsModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::setName(): The data is nullptr!!");
    }
}

void HandIkControlsModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("HandIkControlsModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void HandIkControlsModifierUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row < ADD_HAND_ROW && row >= 0){
            auto properties = false;
            if (column == BINDING_COLUMN){
                if (row == ENABLE_ROW){
                    (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : NULL;
                    selectTableToView(properties, "enable");
                }
            }
        }else if (row == ADD_HAND_ROW && column == NAME_COLUMN){
            addHand();
        }else if (row > ADD_HAND_ROW && row < ADD_HAND_ROW + bsData->getNumberOfHands() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfHands() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    handUI->loadData(((BehaviorFile *)(bsData->getParentFile())), &bsData->hands[result], bsData, result);
                    setCurrentIndex(HAND_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the Hand \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeHand(result);
                    }
                }
            }else{
                LogFile::writeToLog("HandIkControlsModifierUI::viewSelectedChild(): Invalid index of range to view!!");
            }
        }
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void HandIkControlsModifierUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case HAND_WIDGET:
        handUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("HandIkControlsModifierUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void HandIkControlsModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void HandIkControlsModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("HandIkControlsModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void HandIkControlsModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (currentIndex() == MAIN_WIDGET){
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto setname = [&](const QString & fieldname, int row){
                    auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                    (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
                };
                setname("enable", ENABLE_ROW);
            }
        }else{
            handUI->variableRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("HandIkControlsModifierUI::variableRenamed(): The data is nullptr!!");
    }
}

