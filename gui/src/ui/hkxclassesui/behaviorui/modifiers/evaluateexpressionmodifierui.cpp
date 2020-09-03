#include "evaluateexpressionmodifierui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbevaluateexpressionmodifier.h"
#include "src/hkxclasses/behavior/hkbexpressiondataarray.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/expressiondataarrayui.h"
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
#define ADD_EXPRESSION_ROW 2

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList EvaluateExpressionModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

EvaluateExpressionModifierUI::EvaluateExpressionModifierUI()
    : bsData(nullptr),
      expressionUI(new ExpressionDataArrayUI),
      groupBox(new QGroupBox),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox)
{
    groupBox->setTitle("hkbEvaluateExpressionModifier");
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
    table->setItem(ADD_EXPRESSION_ROW, NAME_COLUMN, new TableWidgetItem("Add Expression", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new expression"));
    table->setItem(ADD_EXPRESSION_ROW, BINDING_COLUMN, new TableWidgetItem("hkExpression", Qt::AlignCenter));
    table->setItem(ADD_EXPRESSION_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Expression", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected expression"));
    table->setItem(ADD_EXPRESSION_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Expression", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected expression"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(expressionUI);
    toggleSignals(true);
}

void EvaluateExpressionModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(expressionUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(expressionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(expressionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(expressionUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(expressionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(expressionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void EvaluateExpressionModifierUI::addExpression(){
    if (bsData){
        auto exps = bsData->getExpressions();
        (!exps) ? bsData->setExpressions(new hkbExpressionDataArray(bsData->getParentFile())) : NULL;
        exps->addExpression();
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::addExpression(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::removeExpression(int index){
    if (bsData){
        auto exps = bsData->getExpressions();
        if (exps){
            exps->removeExpression(index);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog("EvaluateExpressionModifierUI::removeExpression(): Event data is nullptr!!");
        }
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::removeExpression(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_EVALUATE_EXPRESSION_MODIFIER){
            bsData = static_cast<hkbEvaluateExpressionModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("EvaluateExpressionModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void EvaluateExpressionModifierUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_EXPRESSION_ROW + bsData->getNumberOfExpressions() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        auto exps = bsData->getExpressions();
        if (exps){
            for (auto i = ADD_EXPRESSION_ROW + 1, j = 0; j < bsData->getNumberOfExpressions(); i++, j++){
                UIHelper::setRowItems(i, exps->getExpressionAt(j), exps->getClassname(), "Remove", "Edit", "Double click to remove this expression", "Double click to edit this expression", table);
            }
        }
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        bool isProperty;
        auto row = table->currentRow();
        if (row == ENABLE_ROW){
            (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, "enable", VARIABLE_TYPE_BOOL, isProperty, table, bsData);
        }
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void EvaluateExpressionModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::setName(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("EvaluateExpressionModifierUI::setEnable(): The data is nullptr!!");
}

void EvaluateExpressionModifierUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row < ADD_EXPRESSION_ROW && row >= 0){
            auto properties = false;
            if (column == BINDING_COLUMN){
                if (row == ENABLE_ROW){
                    (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : NULL;
                    selectTableToView(properties, "enable");
                }
            }
        }else if (row == ADD_EXPRESSION_ROW && column == NAME_COLUMN){
            addExpression();
        }else if (row > ADD_EXPRESSION_ROW && row < ADD_EXPRESSION_ROW + bsData->getNumberOfExpressions() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfExpressions() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    expressionUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &static_cast<hkbExpressionDataArray *>(bsData->expressions.data())->expressionsData[result]);
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the expression \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeExpression(result);
                    }
                }
            }else{
                LogFile::writeToLog("EvaluateExpressionModifierUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case CHILD_WIDGET:
        expressionUI->setAssignmentVariableIndex(index, name); break;
    default:
        WARNING_MESSAGE("EvaluateExpressionModifierUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void EvaluateExpressionModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && events && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), expressionUI, SLOT(setAssignmentEventIndex(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void EvaluateExpressionModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("EvaluateExpressionModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (currentIndex() == CHILD_WIDGET) ? expressionUI->eventRenamed(name, index) : NULL;
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void EvaluateExpressionModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (currentIndex() == MAIN_WIDGET){
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto bindIndex = bind->getVariableIndexOfBinding("enable");
                (bindIndex == index) ? table->item(ENABLE_ROW, BINDING_COLUMN)->setText(name) : NULL;
            }
        }else{
            expressionUI->variableRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("EvaluateExpressionModifierUI::variableRenamed(): The data is nullptr!!");
    }
}

