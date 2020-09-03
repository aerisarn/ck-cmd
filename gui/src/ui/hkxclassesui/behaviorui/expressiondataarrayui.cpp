#include "expressiondataarrayui.h"

#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbexpressiondataarray.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define EXPRESSION_ROW 0
#define ASSIGNMENT_VARIABLE_INDEX_ROW 1
#define ASSIGNMENT_EVENT_INDEX_ROW 2
#define EVENT_MODE_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList ExpressionDataArrayUI::EventModeUI = {
    "EVENT_MODE_SEND_ONCE",
    "EVENT_MODE_SEND_ON_TRUE",
    "EVENT_MODE_SEND_ON_FALSE_TO_TRUE",
    "EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE"
};

const QStringList ExpressionDataArrayUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

ExpressionDataArrayUI::ExpressionDataArrayUI()
    : file(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      expression(new QLineEdit),
      eventMode(new ComboBox)
{
    setTitle("Expression Editor");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(EXPRESSION_ROW, NAME_COLUMN, new TableWidgetItem("expression"));
    table->setItem(EXPRESSION_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setCellWidget(EXPRESSION_ROW, VALUE_COLUMN, expression);
    table->setItem(ASSIGNMENT_VARIABLE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("assignmentVariableIndex"));
    table->setItem(ASSIGNMENT_VARIABLE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ASSIGNMENT_VARIABLE_INDEX_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP));
    table->setItem(ASSIGNMENT_EVENT_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("assignmentEventIndex"));
    table->setItem(ASSIGNMENT_EVENT_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ASSIGNMENT_EVENT_INDEX_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_MODE_ROW, NAME_COLUMN, new TableWidgetItem("eventMode"));
    table->setItem(EVENT_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("EventMode", Qt::AlignCenter));
    table->setCellWidget(EVENT_MODE_ROW, VALUE_COLUMN, eventMode);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 2);
    setLayout(topLyt);
    toggleSignals(true);
}

void ExpressionDataArrayUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(expression, SIGNAL(editingFinished()), this, SLOT(setExpression()), Qt::UniqueConnection);
        connect(eventMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEventMode(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(expression, SIGNAL(editingFinished()), this, SLOT(setExpression()));
        disconnect(eventMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEventMode(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void ExpressionDataArrayUI::loadData(BehaviorFile *parentFile, hkbExpressionDataArray::hkExpression *exp){
    toggleSignals(false);
    if (parentFile && bsData){
        file = parentFile;
        bsData = exp;
        expression->setText(bsData->expression);
        auto varName = file->getVariableNameAt(bsData->assignmentVariableIndex);
        auto item = table->item(ASSIGNMENT_VARIABLE_INDEX_ROW, VALUE_COLUMN);
        (varName != "") ? item->setText(varName) : item->setText("NONE");
        varName = file->getEventNameAt(bsData->assignmentEventIndex);
        item = table->item(ASSIGNMENT_EVENT_INDEX_ROW, VALUE_COLUMN);
        (varName != "") ? item->setText(varName) : item->setText("NONE");
        (!eventMode->count()) ? eventMode->insertItems(0, EventModeUI) : NULL;
        eventMode->setCurrentIndex(EventModeUI.indexOf(bsData->eventMode));
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::loadData(): Behavior file or data is null!!!");
    }
    toggleSignals(true);
}

void ExpressionDataArrayUI::setExpression(){
    if (bsData && file){
        (bsData->expression != expression->text()) ? bsData->expression = expression->text(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::setExpression(): Behavior file or data is null!!!");
    }
}

void ExpressionDataArrayUI::setEventMode(int index){
    (bsData) ? bsData->eventMode = EventModeUI.at(index), file->setIsChanged(true) : LogFile::writeToLog("ExpressionDataArrayUI::setEventMode(): The data is nullptr!!");
}

void ExpressionDataArrayUI::setAssignmentVariableIndex(int index, const QString & name){
    if (bsData && file){
        if (bsData->assignmentVariableIndex != --index){
            bsData->assignmentVariableIndex = index;
            table->item(ASSIGNMENT_VARIABLE_INDEX_ROW, VALUE_COLUMN)->setText(name);
            file->setIsChanged(true);
        }
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::setAssignmentVariableIndex(): Behavior file or data is null!!!");
    }
}

void ExpressionDataArrayUI::setAssignmentEventIndex(int index, const QString & name){
    if (bsData && file){
        if (bsData->assignmentEventIndex != --index){
            bsData->assignmentEventIndex = index;
            table->item(ASSIGNMENT_EVENT_INDEX_ROW, VALUE_COLUMN)->setText(name);
            file->setIsChanged(true);
        }
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::setAssignmentVariableIndex(): Behavior file or data is null!!!");
    }
}

void ExpressionDataArrayUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row == ASSIGNMENT_VARIABLE_INDEX_ROW && column == VALUE_COLUMN){
            emit viewVariables(bsData->assignmentVariableIndex + 1, QString(), QStringList());
        }else if (row == ASSIGNMENT_EVENT_INDEX_ROW && column == VALUE_COLUMN){
            emit viewEvents(bsData->assignmentEventIndex + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void ExpressionDataArrayUI::eventRenamed(const QString & name, int index){
    if (bsData){
        if (--index == bsData->assignmentEventIndex){
            table->item(ASSIGNMENT_EVENT_INDEX_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::eventRenamed(): The data is nullptr!!");
    }
}

void ExpressionDataArrayUI::variableRenamed(const QString & name, int index){
    if (bsData){
        if (--index == bsData->assignmentVariableIndex){
            table->item(ASSIGNMENT_VARIABLE_INDEX_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("ExpressionDataArrayUI::variableRenamed(): The data is nullptr!!");
    }
}
