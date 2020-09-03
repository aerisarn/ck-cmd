#include "blendergeneratorchildui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/generators/hkbblendergenerator.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/boneweightarrayui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>
#include <QGroupBox>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define BONE_WEIGHTS_ROW 0
#define WEIGHT_ROW 1
#define WORLD_FROM_MODEL_WEIGHT_ROW 2
#define GENERATOR_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BlenderGeneratorChildUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BlenderGeneratorChildUI::BlenderGeneratorChildUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      childIndex(0),
      topLyt(new QGridLayout),
      groupBox(new QGroupBox("hkbBlenderGeneratorChild")),
      returnPB(new QPushButton("Return")),
      table(new TableWidget(QColor(Qt::white))),
      boneWeights(new CheckButtonCombo("Edit")),
      boneWeightArrayUI(new BoneWeightArrayUI),
      weight(new DoubleSpinBox),
      worldFromModelWeight(new DoubleSpinBox)
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(BONE_WEIGHTS_ROW, NAME_COLUMN, new TableWidgetItem("boneWeights"));
    table->setItem(BONE_WEIGHTS_ROW, TYPE_COLUMN, new TableWidgetItem("hkbBoneWeightArray", Qt::AlignCenter));
    table->setItem(BONE_WEIGHTS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    //table->setItem(BONE_WEIGHTS_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(BONE_WEIGHTS_ROW, VALUE_COLUMN, boneWeights);
    table->setItem(WEIGHT_ROW, NAME_COLUMN, new TableWidgetItem("weight"));
    table->setItem(WEIGHT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(WEIGHT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(WEIGHT_ROW, VALUE_COLUMN, weight);
    table->setItem(WORLD_FROM_MODEL_WEIGHT_ROW, NAME_COLUMN, new TableWidgetItem("worldFromModelWeight"));
    table->setItem(WORLD_FROM_MODEL_WEIGHT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(WORLD_FROM_MODEL_WEIGHT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(WORLD_FROM_MODEL_WEIGHT_ROW, VALUE_COLUMN, worldFromModelWeight);
    table->setItem(GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("generator"));
    table->setItem(GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbGenerator", Qt::AlignCenter));
    table->setItem(GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(boneWeightArrayUI);
    returnPB->setVisible(false);
    toggleSignals(true);
}

void BlenderGeneratorChildUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(boneWeightArrayUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(weight, SIGNAL(editingFinished()), this, SLOT(setWeight()), Qt::UniqueConnection);
        connect(worldFromModelWeight, SIGNAL(editingFinished()), this, SLOT(setWorldFromModelWeight()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
        connect(boneWeights, SIGNAL(pressed()), this, SLOT(viewBoneWeights()), Qt::UniqueConnection);
        connect(boneWeights, SIGNAL(enabled(bool)), this, SLOT(toggleBoneWeights(bool)), Qt::UniqueConnection);
        connect(returnPB, SIGNAL(clicked(bool)), this, SIGNAL(returnToParent(bool)), Qt::UniqueConnection);
    }else{
        disconnect(boneWeightArrayUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(weight, SIGNAL(editingFinished()), this, SLOT(setWeight()));
        disconnect(worldFromModelWeight, SIGNAL(editingFinished()), this, SLOT(setWorldFromModelWeight()));
        disconnect(boneWeights, SIGNAL(pressed()), this, SLOT(viewBoneWeights()));
        disconnect(boneWeights, SIGNAL(enabled(bool)), this, SLOT(toggleBoneWeights(bool)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
        disconnect(returnPB, SIGNAL(clicked(bool)), this, SIGNAL(returnToParent(bool)));
    }
}

void BlenderGeneratorChildUI::loadData(HkxObject *data, int childindex){
    hkbVariableBindingSet *varBind = nullptr;
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
            childIndex = childindex;
            bsData = static_cast<hkbBlenderGeneratorChild *>(data);
            (bsData->boneWeights.data()) ? boneWeights->setChecked(true) : boneWeights->setChecked(false);
            weight->setValue(bsData->getWeight());
            worldFromModelWeight->setValue(bsData->getWorldFromModelWeight());
            table->item(GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getGeneratorName());
            varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(BONE_WEIGHTS_ROW, BINDING_COLUMN, varBind, "boneWeights", table, bsData);
            UIHelper::loadBinding(WEIGHT_ROW, BINDING_COLUMN, varBind, "weight", table, bsData);
            UIHelper::loadBinding(WORLD_FROM_MODEL_WEIGHT_ROW, BINDING_COLUMN, varBind, "worldFromModelWeight", table, bsData);
            if (bsData->isParametricBlend()){
                weight->setMaximum(1);
                weight->setMinimum(0);
            }else{
                weight->setMaximum(std::numeric_limits<int>::max());
                weight->setMinimum(std::numeric_limits<int>::min());
            }
        }else{
            LogFile::writeToLog(QString("BlenderGeneratorChildUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BlenderGeneratorChildUI::setGenerator(int index, const QString & name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->generator.data()), NULL_SIGNATURE, HkxObject::TYPE_GENERATOR, table, behaviorView, GENERATOR_ROW, VALUE_COLUMN);
}

void BlenderGeneratorChildUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case BONE_WEIGHTS_ROW:
            checkisproperty(BONE_WEIGHTS_ROW, "boneWeights", VARIABLE_TYPE_POINTER); break;
        case WEIGHT_ROW:
            checkisproperty(WEIGHT_ROW, "weight", VARIABLE_TYPE_REAL); break;
        case WORLD_FROM_MODEL_WEIGHT_ROW:
            checkisproperty(WORLD_FROM_MODEL_WEIGHT_ROW, "worldFromModelWeight", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void BlenderGeneratorChildUI::setWeight(){
    (bsData) ? bsData->setWeight(weight->value()) : LogFile::writeToLog("BlenderGeneratorChildUI::setWeight(): The 'bsData' pointer is nullptr!!");
}

void BlenderGeneratorChildUI::setWorldFromModelWeight(){
    (bsData) ? bsData->setWorldFromModelWeight(worldFromModelWeight->value()) : LogFile::writeToLog("BlenderGeneratorChildUI::setWorldFromModelWeight(): The 'bsData' pointer is nullptr!!");
}

void BlenderGeneratorChildUI::viewBoneWeights(){
    if (bsData){
        boneWeightArrayUI->loadData(bsData->getBoneWeightsData());
        setCurrentIndex(BONE_WEIGHT_ARRAY_WIDGET);
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::viewBoneWeights(): The data is nullptr!!");
    }
}

void BlenderGeneratorChildUI::toggleBoneWeights(bool enable){
    if (bsData){
        if (!enable){
            bsData->setBoneWeights(HkxSharedPtr());
            static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
        }else if (enable && !bsData->getBoneWeightsData()){
            bsData->setBoneWeights(HkxSharedPtr(new hkbBoneWeightArray(bsData->getParentFile(), -1, static_cast<BehaviorFile *>(bsData->getParentFile())->getNumberOfBones())));
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::toggleBoneWeights(): The data is nullptr!!");
    }
}

void BlenderGeneratorChildUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BlenderGeneratorChildUI::selectTableToView(): The data is nullptr!!");
    }
}

void BlenderGeneratorChildUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case BONE_WEIGHTS_ROW:
                checkisproperty(BONE_WEIGHTS_ROW, "boneWeights"); break;
            case WEIGHT_ROW:
                checkisproperty(WEIGHT_ROW, "weight"); break;
            case WORLD_FROM_MODEL_WEIGHT_ROW:
                checkisproperty(WORLD_FROM_MODEL_WEIGHT_ROW, "worldFromModelWeight"); break;
            }
        }else if (row == GENERATOR_ROW && column == VALUE_COLUMN){
            QStringList list = {hkbStateMachineStateInfo::getClassname(), hkbBlenderGeneratorChild::getClassname(), BSBoneSwitchGeneratorBoneData::getClassname()};
            emit viewGenerators(bsData->getIndexOfGenerator(bsData->getGenerator()) + 1, QString(), list);
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BlenderGeneratorChildUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}

void BlenderGeneratorChildUI::generatorTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setGenerator(index, name); break;
    default:
        WARNING_MESSAGE("BlenderGeneratorChildUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BlenderGeneratorChildUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("BlenderGeneratorChildUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BlenderGeneratorChildUI::variableRenamed(const QString & name, int index){
    if (bsData){
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("boneWeights", BONE_WEIGHTS_ROW);
            setname("weight", WEIGHT_ROW);
            setname("worldFromModelWeight", WORLD_FROM_MODEL_WEIGHT_ROW);
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BlenderGeneratorChildUI::generatorRenamed(const QString &name, int index){
    if (bsData){
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->getGenerator())){
            table->item(GENERATOR_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BlenderGeneratorChildUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}

void BlenderGeneratorChildUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties && generators){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(generatorTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BlenderGeneratorChildUI::connectToTables(): One or more arguments are nullptr!!");
    }
}
