#include "bsistatetagginggeneratorui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/generators/bsistatetagginggenerator.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/TreeGraphicsView.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

#include <QHeaderView>
#include <QGroupBox>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define I_STATE_TO_SET_AS_ROW 1
#define I_PRIORITY_ROW 2
#define P_DEFAULT_GENERATOR_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSiStateTaggingGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSiStateTaggingGeneratorUI::BSiStateTaggingGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      iStateToSetAs(new SpinBox),
      iPriority(new SpinBox)
{
    setTitle("BSiStateTaggingGenerator");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignVCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(I_STATE_TO_SET_AS_ROW, NAME_COLUMN, new TableWidgetItem("iStateToSetAs"));
    table->setItem(I_STATE_TO_SET_AS_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(I_STATE_TO_SET_AS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(I_STATE_TO_SET_AS_ROW, VALUE_COLUMN, iStateToSetAs);
    table->setItem(I_PRIORITY_ROW, NAME_COLUMN, new TableWidgetItem("iPriority"));
    table->setItem(I_PRIORITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(I_PRIORITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(I_PRIORITY_ROW, VALUE_COLUMN, iPriority);
    table->setItem(P_DEFAULT_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("pDefaultGenerator"));
    table->setItem(P_DEFAULT_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbGenerator", Qt::AlignCenter));
    table->setItem(P_DEFAULT_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(P_DEFAULT_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSiStateTaggingGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        if (toggleconnections){
            connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
            connect(iStateToSetAs, SIGNAL(editingFinished()), this, SLOT(setIStateToSetAs()), Qt::UniqueConnection);
            connect(iPriority, SIGNAL(editingFinished()), this, SLOT(setIPriority()), Qt::UniqueConnection);
            connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
        }else{
            disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName()));
            disconnect(iStateToSetAs, SIGNAL(editingFinished()), this, SLOT(setIStateToSetAs()));
            disconnect(iPriority, SIGNAL(editingFinished()), this, SLOT(setIPriority()));
            disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
        }
    }
}

void BSiStateTaggingGeneratorUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *generators){
    if (variables && properties && generators){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(setDefaultGenerator(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_I_STATE_TAGGING_GENERATOR){
            bsData = static_cast<BSiStateTaggingGenerator *>(data);
            name->setText(bsData->getName());
            iStateToSetAs->setValue(bsData->getIStateToSetAs());
            iPriority->setValue(bsData->getIPriority());
            table->item(P_DEFAULT_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getDefaultGeneratorName());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(I_STATE_TO_SET_AS_ROW, BINDING_COLUMN, varBind, "iStateToSetAs", table, bsData);
            UIHelper::loadBinding(I_PRIORITY_ROW, BINDING_COLUMN, varBind, "iPriority", table, bsData);
        }else{
            LogFile::writeToLog(QString("BSiStateTaggingGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BSiStateTaggingGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::setIStateToSetAs(){
    (bsData) ? bsData->setIStateToSetAs(iStateToSetAs->value()) : LogFile::writeToLog("BSiStateTaggingGeneratorUI::iStateToSetAs(): The 'bsData' pointer is nullptr!!");
}

void BSiStateTaggingGeneratorUI::setIPriority(){
    (bsData) ? bsData->setIPriority(iPriority->value()) : LogFile::writeToLog("BSiStateTaggingGeneratorUI::setIPriority(): The 'bsData' pointer is nullptr!!");
}

void BSiStateTaggingGeneratorUI::setDefaultGenerator(int index, const QString & name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->pDefaultGenerator.data()), NULL_SIGNATURE, HkxObject::TYPE_GENERATOR, table, behaviorView, P_DEFAULT_GENERATOR_ROW, VALUE_COLUMN);
}

void BSiStateTaggingGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case I_STATE_TO_SET_AS_ROW:
            checkisproperty(I_STATE_TO_SET_AS_ROW, "iStateToSetAs", VARIABLE_TYPE_INT32); break;
        case I_PRIORITY_ROW:
            checkisproperty(I_PRIORITY_ROW, "iPriority", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case I_STATE_TO_SET_AS_ROW:
                checkisproperty(I_STATE_TO_SET_AS_ROW, "iStateToSetAs"); break;
            case I_PRIORITY_ROW:
                checkisproperty(I_PRIORITY_ROW, "iPriority"); break;
            }
        }else if (row == P_DEFAULT_GENERATOR_ROW && column == VALUE_COLUMN){
            QStringList list = {hkbStateMachineStateInfo::getClassname(), hkbBlenderGeneratorChild::getClassname(), BSBoneSwitchGeneratorBoneData::getClassname()};
            emit viewGenerators(bsData->getIndexOfGenerator(bsData->pDefaultGenerator) + 1, QString(), list);
        }
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("iStateToSetAs", I_STATE_TO_SET_AS_ROW);
            setname("iPriority", I_PRIORITY_ROW);
        }
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::generatorRenamed(const QString &name, int index){
    if (bsData){
        --index;
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->pDefaultGenerator)){
            table->item(P_DEFAULT_GENERATOR_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("BSiStateTaggingGeneratorUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSiStateTaggingGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}
