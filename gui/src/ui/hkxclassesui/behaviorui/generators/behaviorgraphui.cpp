#include "behaviorgraphui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorgraph.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 3

#define NAME_ROW 0
#define VARIABLE_MODE_ROW 1
#define ROOT_GENERATOR_ROW 2

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList BehaviorGraphUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

BehaviorGraphUI::BehaviorGraphUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      variableMode(new ComboBox)
{
    setTitle("hkbBehaviorGraph");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(VARIABLE_MODE_ROW, NAME_COLUMN, new TableWidgetItem("variableMode"));
    table->setItem(VARIABLE_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("VariableMode", Qt::AlignCenter));
    table->setCellWidget(VARIABLE_MODE_ROW, VALUE_COLUMN, variableMode);
    table->setItem(ROOT_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("rootGenerator"));
    table->setItem(ROOT_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ROOT_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(219, 219, 219), QBrush(Qt::black), "Click to view the list of generators"));
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BehaviorGraphUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(variableMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setVariableMode(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName()));
        disconnect(variableMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setVariableMode(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void BehaviorGraphUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_BEHAVIOR_GRAPH){
            bsData = static_cast<hkbBehaviorGraph *>(data);
            name->setText(bsData->getName());
            (!variableMode->count()) ? variableMode->insertItems(0, bsData->VariableMode) : NULL;
            variableMode->setCurrentIndex(bsData->VariableMode.indexOf(bsData->getVariableMode()));
            table->item(ROOT_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getRootGeneratorName());
        }else{
            LogFile::writeToLog("BehaviorGraphUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BehaviorGraphUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BehaviorGraphUI::setName(const QString & newname){
    (bsData) ? bsData->setName(newname), bsData->updateIconNames() : LogFile::writeToLog("BehaviorGraphUI::setName(): The data is nullptr!!");
}

void BehaviorGraphUI::setVariableMode(int index){
    (bsData) ? bsData->setVariableMode(bsData->VariableMode.at(index)) : LogFile::writeToLog("BehaviorGraphUI::setVariableMode(): The data is nullptr!!");
}

void BehaviorGraphUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row == ROOT_GENERATOR_ROW && column == VALUE_COLUMN){
            emit viewGenerators(bsData->getIndexOfGenerator(bsData->rootGenerator) + 1, hkbStateMachine::getClassname(), QStringList());
        }
    }else{
        LogFile::writeToLog("BehaviorGraphUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BehaviorGraphUI::setRootGenerator(int index, const QString &name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->rootGenerator.data()), HKB_STATE_MACHINE, HkxObject::TYPE_GENERATOR, table, behaviorView, ROOT_GENERATOR_ROW, VALUE_COLUMN);
}

void BehaviorGraphUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}

void BehaviorGraphUI::connectToTables(GenericTableWidget *generators){
    if (generators){
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(setRootGenerator(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BehaviorGraphUI::connectToTables(): The argument is nullptr!!");
    }
}
