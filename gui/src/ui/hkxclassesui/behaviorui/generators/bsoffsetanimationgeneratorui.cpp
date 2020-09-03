#include "bsoffsetanimationgeneratorui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/behavior/generators/bsoffsetanimationgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbgenerator.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/genericdatawidgets.h"

#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define FOFFSET_VARIABLE_ROW 1
#define FOFFSET_RANGE_START_ROW 2
#define FOFFSET_RANGE_END_ROW 3
#define PDEFAULT_GENERATOR_ROW 4
#define POFFSET_CLIP_GENERATOR_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSOffsetAnimationGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSOffsetAnimationGeneratorUI::BSOffsetAnimationGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      fOffsetVariable(new DoubleSpinBox),
      fOffsetRangeStart(new DoubleSpinBox),
      fOffsetRangeEnd(new DoubleSpinBox)
{
    setTitle("BSOffsetAnimationGenerator");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(FOFFSET_VARIABLE_ROW, NAME_COLUMN, new TableWidgetItem("fOffsetVariable"));
    table->setItem(FOFFSET_VARIABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FOFFSET_VARIABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(FOFFSET_VARIABLE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FOFFSET_VARIABLE_ROW, VALUE_COLUMN, fOffsetVariable);
    table->setItem(FOFFSET_RANGE_START_ROW, NAME_COLUMN, new TableWidgetItem("fOffsetRangeStart"));
    table->setItem(FOFFSET_RANGE_START_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FOFFSET_RANGE_START_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(FOFFSET_RANGE_START_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FOFFSET_RANGE_START_ROW, VALUE_COLUMN, fOffsetRangeStart);
    table->setItem(FOFFSET_RANGE_END_ROW, NAME_COLUMN, new TableWidgetItem("fOffsetRangeEnd"));
    table->setItem(FOFFSET_RANGE_END_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FOFFSET_RANGE_END_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(FOFFSET_RANGE_END_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FOFFSET_RANGE_END_ROW, VALUE_COLUMN, fOffsetRangeEnd);
    table->setItem(PDEFAULT_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("pDefaultGenerator"));
    table->setItem(PDEFAULT_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbGenerator", Qt::AlignCenter));
    table->setItem(PDEFAULT_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(PDEFAULT_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    table->setItem(POFFSET_CLIP_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("pOffsetClipGenerator"));
    table->setItem(POFFSET_CLIP_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbClipGenerator", Qt::AlignCenter));
    table->setItem(POFFSET_CLIP_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(POFFSET_CLIP_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSOffsetAnimationGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        if (toggleconnections){
            connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
            connect(fOffsetVariable, SIGNAL(editingFinished()), this, SLOT(setFOffsetVariable()), Qt::UniqueConnection);
            connect(fOffsetRangeStart, SIGNAL(editingFinished()), this, SLOT(setFOffsetRangeStart()), Qt::UniqueConnection);
            connect(fOffsetRangeEnd, SIGNAL(editingFinished()), this, SLOT(setFOffsetRangeEnd()), Qt::UniqueConnection);
            connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
        }else{
            disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName()));
            disconnect(fOffsetVariable, SIGNAL(editingFinished()), this, SLOT(setFOffsetVariable()));
            disconnect(fOffsetRangeStart, SIGNAL(editingFinished()), this, SLOT(setFOffsetRangeStart()));
            disconnect(fOffsetRangeEnd, SIGNAL(editingFinished()), this, SLOT(setFOffsetRangeEnd()));
            disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
        }
    }
}

void BSOffsetAnimationGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties && generators){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(setGenerator(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_OFFSET_ANIMATION_GENERATOR){
            bsData = static_cast<BSOffsetAnimationGenerator *>(data);
            name->setText(bsData->getName());
            fOffsetVariable->setValue(bsData->getFOffsetVariable());
            fOffsetRangeStart->setValue(bsData->getFOffsetRangeStart());
            fOffsetRangeEnd->setValue(bsData->getFOffsetRangeEnd());
            table->item(PDEFAULT_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getDefaultGeneratorName());
            table->item(POFFSET_CLIP_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getOffsetClipGeneratorName());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(FOFFSET_VARIABLE_ROW, BINDING_COLUMN, varBind, "fOffsetVariable", table, bsData);
            UIHelper::loadBinding(FOFFSET_RANGE_START_ROW, BINDING_COLUMN, varBind, "fOffsetRangeStart", table, bsData);
            UIHelper::loadBinding(FOFFSET_RANGE_END_ROW, BINDING_COLUMN, varBind, "fOffsetRangeEnd", table, bsData);
        }else{
            LogFile::writeToLog(QString("BSOffsetAnimationGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BSOffsetAnimationGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::setFOffsetVariable(){
    (bsData) ? bsData->setFOffsetVariable(fOffsetVariable->value()) : LogFile::writeToLog("BSOffsetAnimationGeneratorUI::setFOffsetVariable(): The 'bsData' pointer is nullptr!!");
}

void BSOffsetAnimationGeneratorUI::setFOffsetRangeStart(){
    (bsData) ? bsData->setFOffsetRangeStart(fOffsetRangeStart->value()) : LogFile::writeToLog("BSOffsetAnimationGeneratorUI::setFOffsetRangeStart(): The 'bsData' pointer is nullptr!!");
}

void BSOffsetAnimationGeneratorUI::setFOffsetRangeEnd(){
    (bsData) ? bsData->setFOffsetRangeEnd(fOffsetRangeEnd->value()) : LogFile::writeToLog("BSOffsetAnimationGeneratorUI::setFOffsetRangeEnd(): The 'bsData' pointer is nullptr!!");
}

void BSOffsetAnimationGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case FOFFSET_VARIABLE_ROW:
            checkisproperty(FOFFSET_VARIABLE_ROW, "fOffsetVariable", VARIABLE_TYPE_REAL); break;
        case FOFFSET_RANGE_START_ROW:
            checkisproperty(FOFFSET_RANGE_START_ROW, "fOffsetRangeStart", VARIABLE_TYPE_REAL); break;
        case FOFFSET_RANGE_END_ROW:
            checkisproperty(FOFFSET_RANGE_END_ROW, "fOffsetRangeEnd", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::setBindingVariable(): The data is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::setGenerator(int index, const QString & name){
    if (bsData){
        if (table->currentRow() == PDEFAULT_GENERATOR_ROW){
            UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->pDefaultGenerator.data()), NULL_SIGNATURE, HkxObject::TYPE_GENERATOR, table, behaviorView, PDEFAULT_GENERATOR_ROW, VALUE_COLUMN);
        }else if (table->currentRow() == POFFSET_CLIP_GENERATOR_ROW){
            UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->pOffsetClipGenerator.data()), HKB_CLIP_GENERATOR, HkxObject::TYPE_GENERATOR, table, behaviorView, POFFSET_CLIP_GENERATOR_ROW, VALUE_COLUMN);
        }
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::setGenerator(): The 'bsData' pointer is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case FOFFSET_VARIABLE_ROW:
                checkisproperty(FOFFSET_VARIABLE_ROW, "fOffsetVariable"); break;
            case FOFFSET_RANGE_START_ROW:
                checkisproperty(FOFFSET_RANGE_START_ROW, "fOffsetRangeStart"); break;
            case FOFFSET_RANGE_END_ROW:
                checkisproperty(FOFFSET_RANGE_END_ROW, "fOffsetRangeEnd"); break;
            }
        }else if (column == VALUE_COLUMN){
            if (row == PDEFAULT_GENERATOR_ROW){
                QStringList list = {hkbStateMachineStateInfo::getClassname(), hkbBlenderGeneratorChild::getClassname(), BSBoneSwitchGeneratorBoneData::getClassname()};
                emit viewGenerators(bsData->getIndexOfGenerator(bsData->pDefaultGenerator) + 1, QString(), list);
            }else if (row == POFFSET_CLIP_GENERATOR_ROW){
                emit viewGenerators(bsData->getIndexOfGenerator(bsData->pOffsetClipGenerator) + 1, hkbClipGenerator::getClassname(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("fOffsetVariable", FOFFSET_VARIABLE_ROW);
            setname("fOffsetRangeStart", FOFFSET_RANGE_START_ROW);
            setname("fOffsetRangeEnd", FOFFSET_RANGE_END_ROW);
        }
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::generatorRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->pDefaultGenerator)){
            table->item(PDEFAULT_GENERATOR_ROW, VALUE_COLUMN)->setText(name);
        }
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->pOffsetClipGenerator)){
            table->item(POFFSET_CLIP_GENERATOR_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSOffsetAnimationGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}
