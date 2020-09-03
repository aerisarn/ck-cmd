#include "eventsfromrangemodifierui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbEventsFromRangeModifier.h"
#include "src/hkxclasses/behavior/hkbEventRangeDataArray.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/eventrangedataui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 5

#define NAME_ROW 0
#define ENABLE_ROW 1
#define INPUT_VALUE_ROW 2
#define LOWER_BOUND_ROW 3
#define ADD_RANGE_ROW 4

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList EventsFromRangeModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

EventsFromRangeModifierUI::EventsFromRangeModifierUI()
    : bsData(nullptr),
      rangeUI(new EventRangeDataUI),
      groupBox(new QGroupBox),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      inputValue(new DoubleSpinBox),
      lowerBound(new DoubleSpinBox)
{
    groupBox->setTitle("hkbEventsFromRangeModifier");
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
    table->setItem(INPUT_VALUE_ROW, NAME_COLUMN, new TableWidgetItem("inputValue"));
    table->setItem(INPUT_VALUE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(INPUT_VALUE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INPUT_VALUE_ROW, VALUE_COLUMN, inputValue);
    table->setItem(LOWER_BOUND_ROW, NAME_COLUMN, new TableWidgetItem("lowerBound"));
    table->setItem(LOWER_BOUND_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LOWER_BOUND_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOWER_BOUND_ROW, VALUE_COLUMN, lowerBound);
    table->setItem(ADD_RANGE_ROW, NAME_COLUMN, new TableWidgetItem("Add Event Range", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new Event Range"));
    table->setItem(ADD_RANGE_ROW, TYPE_COLUMN, new TableWidgetItem("hkEventRangeData", Qt::AlignCenter));
    table->setItem(ADD_RANGE_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Event Range", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected Event Range"));
    table->setItem(ADD_RANGE_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Event Range", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected Event Range"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(rangeUI);
    toggleSignals(true);
}

void EventsFromRangeModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(inputValue, SIGNAL(editingFinished()), this, SLOT(setInputValue()), Qt::UniqueConnection);
        connect(lowerBound, SIGNAL(editingFinished()), this, SLOT(setLowerBound()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(rangeUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        //connect(rangeUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        //connect(rangeUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(rangeUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(inputValue, SIGNAL(editingFinished()), this, SLOT(setInputValue()));
        disconnect(lowerBound, SIGNAL(editingFinished()), this, SLOT(setLowerBound()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(rangeUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        //disconnect(rangeUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        //disconnect(rangeUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(rangeUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void EventsFromRangeModifierUI::addRange(){
    if (bsData){
        auto exps = bsData->getEventRanges();
        (!exps) ? bsData->setEventRanges(new hkbEventRangeDataArray(bsData->getParentFile())) : NULL;
        exps->addEventRange();
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::addExpression(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::removeRange(int index){
    if (bsData){
        auto exps = bsData->getEventRanges();
        if (exps){
            exps->removeEventRange(index);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog("EventsFromRangeModifierUI::removeExpression(): Event data is nullptr!!");
        }
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::removeRange(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_EVENTS_FROM_RANGE_MODIFIER){
            bsData = static_cast<hkbEventsFromRangeModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            inputValue->setValue(bsData->getInputValue());
            lowerBound->setValue(bsData->getLowerBound());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(INPUT_VALUE_ROW, BINDING_COLUMN, varBind, "inputValue", table, bsData);
            UIHelper::loadBinding(LOWER_BOUND_ROW, BINDING_COLUMN, varBind, "lowerBound", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("EventsFromRangeModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void EventsFromRangeModifierUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_RANGE_ROW + bsData->getNumberOfRanges() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        auto exps = bsData->getEventRanges();
        if (exps){
            for (auto i = ADD_RANGE_ROW + 1, j = 0; j < bsData->getNumberOfRanges(); i++, j++){
                auto eventName = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(exps->getEventRangeIdAt(j));
                UIHelper::setRowItems(i, eventName, exps->getClassname(), "Remove", "Edit", "Double click to remove this Event Range", "Double click to edit this Event Range", table);
            }
        }
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::setBindingVariable(int index, const QString & name){
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
        case INPUT_VALUE_ROW:
            checkisproperty(INPUT_VALUE_ROW, "inputValue", VARIABLE_TYPE_REAL); break;
        case LOWER_BOUND_ROW:
            checkisproperty(LOWER_BOUND_ROW, "lowerBound", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void EventsFromRangeModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::setName(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("EventsFromRangeModifierUI::setEnable(): The data is nullptr!!");
}

void EventsFromRangeModifierUI::setInputValue(){
    (bsData) ? bsData->setInputValue(inputValue->value()) : LogFile::writeToLog("EventsFromRangeModifierUI::setInputValue(): The 'bsData' pointer is nullptr!!");
}

void EventsFromRangeModifierUI::setLowerBound(){
    (bsData) ? bsData->setLowerBound(inputValue->value()) : LogFile::writeToLog("EventsFromRangeModifierUI::setLowerBound(): The 'bsData' pointer is nullptr!!");
}

void EventsFromRangeModifierUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_RANGE_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case ENABLE_ROW:
                    checkisproperty(ENABLE_ROW, "enable"); break;
                case INPUT_VALUE_ROW:
                    checkisproperty(INPUT_VALUE_ROW, "inputValue"); break;
                case LOWER_BOUND_ROW:
                    checkisproperty(LOWER_BOUND_ROW, "lowerBound"); break;
                }
            }
        }else if (row == ADD_RANGE_ROW && column == NAME_COLUMN){
            addRange();
        }else if (row > ADD_RANGE_ROW && row < ADD_RANGE_ROW + bsData->getNumberOfRanges() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfRanges() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    rangeUI->loadData((BehaviorFile *)(bsData->getParentFile()), static_cast<hkbEventRangeDataArray::hkbEventRangeData *>(&static_cast<hkbEventRangeDataArray *>(bsData->eventRanges.data())->eventData[result]), static_cast<hkbEventRangeDataArray *>(bsData->eventRanges.data()), result);
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the expression \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeRange(result);
                    }
                }
            }else{
                LogFile::writeToLog("EventsFromRangeModifierUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    /*case CHILD_WIDGET:
        rangeUI->setAssignmentVariableIndex(index, name); break;*/
    default:
        WARNING_MESSAGE("EventsFromRangeModifierUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void EventsFromRangeModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && events && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), rangeUI, SLOT(setEventId(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void EventsFromRangeModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("EventsFromRangeModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (currentIndex() == CHILD_WIDGET) ? rangeUI->eventRenamed(name, index) : NULL;
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void EventsFromRangeModifierUI::variableRenamed(const QString & name, int index){
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
                setname("inputValue", INPUT_VALUE_ROW);
                setname("lowerBound", LOWER_BOUND_ROW);
            }
        }else{
            //rangeUI->variableRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("EventsFromRangeModifierUI::variableRenamed(): The data is nullptr!!");
    }
}

