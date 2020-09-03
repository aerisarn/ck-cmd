#include "bscyclicblendtransitiongeneratorui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbblendergenerator.h"
#include "src/hkxclasses/behavior/generators/bscyclicblendtransitiongenerator.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/eventui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 7

#define NAME_ROW 0
#define EVENT_TO_FREEZE_BLEND_VALUE_ROW 1
#define EVENT_TO_CROSS_BLEND_ROW 2
#define BLEND_PARAMETER_ROW 3
#define TRANSITION_DURATION_ROW 4
#define BLEND_CURVE_ROW 5
#define BLENDER_GENERATOR_ROW 6

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSCyclicBlendTransitionGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSCyclicBlendTransitionGeneratorUI::BSCyclicBlendTransitionGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      groupBox(new QGroupBox("BSCyclicBlendTransitionGenerator")),
      eventUI(new EventUI),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      eventToFreezeBlendValue(new CheckButtonCombo("Edit")),
      eventToCrossBlend(new CheckButtonCombo("Edit")),
      fBlendParameter(new DoubleSpinBox),
      fTransitionDuration(new DoubleSpinBox),
      eBlendCurve(new ComboBox)
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(EVENT_TO_FREEZE_BLEND_VALUE_ROW, NAME_COLUMN, new TableWidgetItem("eventToFreezeBlendValue"));
    table->setItem(EVENT_TO_FREEZE_BLEND_VALUE_ROW, TYPE_COLUMN, new TableWidgetItem("hkEvent", Qt::AlignCenter));
    table->setItem(EVENT_TO_FREEZE_BLEND_VALUE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_FREEZE_BLEND_VALUE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(EVENT_TO_FREEZE_BLEND_VALUE_ROW, VALUE_COLUMN, eventToFreezeBlendValue);
    table->setItem(EVENT_TO_CROSS_BLEND_ROW, NAME_COLUMN, new TableWidgetItem("eventToCrossBlend"));
    table->setItem(EVENT_TO_CROSS_BLEND_ROW, TYPE_COLUMN, new TableWidgetItem("hkEvent", Qt::AlignCenter));
    table->setItem(EVENT_TO_CROSS_BLEND_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_CROSS_BLEND_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(EVENT_TO_CROSS_BLEND_ROW, VALUE_COLUMN, eventToCrossBlend);
    table->setItem(BLEND_PARAMETER_ROW, NAME_COLUMN, new TableWidgetItem("fBlendParameter"));
    table->setItem(BLEND_PARAMETER_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(BLEND_PARAMETER_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BLEND_PARAMETER_ROW, VALUE_COLUMN, fBlendParameter);
    table->setItem(TRANSITION_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("fTransitionDuration"));
    table->setItem(TRANSITION_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TRANSITION_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TRANSITION_DURATION_ROW, VALUE_COLUMN, fTransitionDuration);
    table->setItem(BLEND_CURVE_ROW, NAME_COLUMN, new TableWidgetItem("eBlendCurve"));
    table->setItem(BLEND_CURVE_ROW, TYPE_COLUMN, new TableWidgetItem("BlendCurve", Qt::AlignCenter));
    table->setItem(BLEND_CURVE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(BLEND_CURVE_ROW, VALUE_COLUMN, eBlendCurve);
    table->setItem(BLENDER_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("pBlenderGenerator"));
    table->setItem(BLENDER_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbBlenderGenerator", Qt::AlignCenter));
    table->setItem(BLENDER_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(BLENDER_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(eventUI);
    toggleSignals(true);
}

void BSCyclicBlendTransitionGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(fBlendParameter, SIGNAL(editingFinished()), this, SLOT(setBlendParameter()), Qt::UniqueConnection);
        connect(fTransitionDuration, SIGNAL(editingFinished()), this, SLOT(setTransitionDuration()), Qt::UniqueConnection);
        connect(eventToFreezeBlendValue, SIGNAL(pressed()), this, SLOT(viewEventToFreezeBlendValue()), Qt::UniqueConnection);
        connect(eventToFreezeBlendValue, SIGNAL(enabled(bool)), this, SLOT(toggleEventToFreezeBlendValue(bool)), Qt::UniqueConnection);
        connect(eventToCrossBlend, SIGNAL(pressed()), this, SLOT(viewEventToCrossBlend()), Qt::UniqueConnection);
        connect(eventToCrossBlend, SIGNAL(enabled(bool)), this, SLOT(toggleEventToCrossBlend(bool)), Qt::UniqueConnection);
        connect(eBlendCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(setBlendCurve(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(eventUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(eventUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(fBlendParameter, SIGNAL(editingFinished()), this, SLOT(setBlendParameter()));
        disconnect(fTransitionDuration, SIGNAL(editingFinished()), this, SLOT(setTransitionDuration()));
        disconnect(eventToFreezeBlendValue, SIGNAL(pressed()), this, SLOT(viewEventToFreezeBlendValue()));
        disconnect(eventToFreezeBlendValue, SIGNAL(enabled(bool)), this, SLOT(toggleEventToFreezeBlendValue(bool)));
        disconnect(eventToCrossBlend, SIGNAL(pressed()), this, SLOT(viewEventToCrossBlend()));
        disconnect(eventToCrossBlend, SIGNAL(enabled(bool)), this, SLOT(toggleEventToCrossBlend(bool)));
        disconnect(eBlendCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(setBlendCurve(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(eventUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(eventUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
    }
}

void BSCyclicBlendTransitionGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == BS_CYCLIC_BLEND_TRANSITION_GENERATOR){
            bsData = static_cast<BSCyclicBlendTransitionGenerator *>(data);
            name->setText(bsData->getName());
            (bsData->getEventToFreezeBlendValueId() > -1) ? eventToFreezeBlendValue->setChecked(true) : eventToFreezeBlendValue->setChecked(false);
            (bsData->getEventToCrossBlendId() > -1) ? eventToCrossBlend->setChecked(true) : eventToCrossBlend->setChecked(false);
            (!eBlendCurve->count()) ? eBlendCurve->insertItems(0, bsData->BlendCurve) : NULL;
            fTransitionDuration->setValue(bsData->getFTransitionDuration());
            eBlendCurve->setCurrentIndex(bsData->BlendCurve.indexOf(bsData->getEBlendCurve()));
            table->item(BLENDER_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getBlenderGeneratorName());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(BLEND_PARAMETER_ROW, BINDING_COLUMN, varBind, "fBlendParameter", table, bsData);
            UIHelper::loadBinding(TRANSITION_DURATION_ROW, BINDING_COLUMN, varBind, "fTransitionDuration", table, bsData);
        }else{
            LogFile::writeToLog(QString("BSCyclicBlendTransitionGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void BSCyclicBlendTransitionGeneratorUI::setName(const QString & newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(bsData->getName(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::setBlendParameter(){
    (bsData) ? bsData->setFBlendParameter(fBlendParameter->value()) : LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::setBlendParameter(): The data is nullptr!!");
}

void BSCyclicBlendTransitionGeneratorUI::setTransitionDuration(){
    (bsData) ? bsData->setFTransitionDuration(fTransitionDuration->value()) : LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::setTransitionDuration(): The data is nullptr!!");
}

void BSCyclicBlendTransitionGeneratorUI::setBlendCurve(int index){
    (bsData) ? bsData->setEBlendCurve(index) : LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::setBlendCurve(): The data is nullptr!!");
}

void BSCyclicBlendTransitionGeneratorUI::viewEventToFreezeBlendValue(){
    if (bsData){
        eventUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &bsData->eventToFreezeBlendValue);
        setCurrentIndex(EVENT_WIDGET);
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::viewEventToFreezeBlendValue(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::toggleEventToFreezeBlendValue(bool enable){
    if (bsData){
        (!enable) ? bsData->nullEventToFreezeBlendValue() : NULL;
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::toggleEventToFreezeBlendValue(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::viewEventToCrossBlend(){
    if (bsData){
        eventUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &bsData->eventToCrossBlend);
        setCurrentIndex(EVENT_WIDGET);
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::viewEventToCrossBlend(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::toggleEventToCrossBlend(bool enable){
    if (bsData){
        (!enable) ? bsData->nullEventToCrossBlend() : NULL;
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::toggleEventToCrossBlend(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
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

void BSCyclicBlendTransitionGeneratorUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case BLEND_PARAMETER_ROW:
                checkisproperty(BLEND_PARAMETER_ROW, "fBlendParameter"); break;
            case TRANSITION_DURATION_ROW:
                checkisproperty(TRANSITION_DURATION_ROW, "fTransitionDuration"); break;
            }
        }else if (column == VALUE_COLUMN){
            if (row == BLENDER_GENERATOR_ROW){
                emit viewGenerators(bsData->getIndexOfGenerator(bsData->pBlenderGenerator) + 1, hkbBlenderGenerator::getClassname(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::setBlenderGenerator(int index, const QString & name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->pBlenderGenerator.data()), HKB_BLENDER_GENERATOR, HkxObject::TYPE_GENERATOR, table, behaviorView, BLENDER_GENERATOR_ROW, VALUE_COLUMN);
}

void BSCyclicBlendTransitionGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case BLEND_PARAMETER_ROW:
            checkisproperty(BLEND_PARAMETER_ROW, "fBlendParameter", VARIABLE_TYPE_REAL); break;
        case TRANSITION_DURATION_ROW:
            checkisproperty(TRANSITION_DURATION_ROW, "fTransitionDuration", VARIABLE_TYPE_REAL); break;
        default:
            return;
        }
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}

void BSCyclicBlendTransitionGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (generators && variables && events && properties){
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(events, SIGNAL(elementSelected(int,QString)), eventUI, SLOT(setEvent(int,QString)), Qt::UniqueConnection);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(setBlenderGenerator(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::eventRenamed(const QString & name, int index){
    if (bsData){
        if (currentIndex() == EVENT_WIDGET){
            eventUI->eventRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("fBlendParameter", BLEND_PARAMETER_ROW);
            setname("fTransitionDuration", TRANSITION_DURATION_ROW);
        }
    }else{
        LogFile::writeToLog("BSCyclicBlendTransitionGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}

void BSCyclicBlendTransitionGeneratorUI::generatorRenamed(const QString &name, int index){
    if (--index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->pBlenderGenerator)){
        table->item(BLENDER_GENERATOR_ROW, VALUE_COLUMN)->setText(name);
    }
}

void BSCyclicBlendTransitionGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
    setCurrentIndex(MAIN_WIDGET);
}

