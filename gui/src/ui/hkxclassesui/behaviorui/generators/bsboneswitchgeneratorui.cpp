#include "bsboneswitchgeneratorui.h"
#include "bsboneswitchgeneratorbonedataui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 3

#define NAME_ROW 0
#define DEFAULT_GENERATOR_ROW 1
#define ADD_CHILD_ROW 2

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSBoneSwitchGeneratorUI::types = {
    "hkbStateMachine",
    "hkbManualSelectorGenerator",
    "hkbBlenderGenerator",
    "BSiStateTaggingGenerator",
    "BSBoneSwitchGenerator",
    "BSCyclicBlendTransitionGenerator",
    "BSSynchronizedClipGenerator",
    "hkbModifierGenerator",
    "BSOffsetAnimationGenerator",
    "hkbPoseMatchingGenerator",
    "hkbClipGenerator",
    "hkbBehaviorReferenceGenerator",
    "BGSGamebryoSequenceGenerator"
};

const QStringList BSBoneSwitchGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSBoneSwitchGeneratorUI::BSBoneSwitchGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      groupBox(new QGroupBox("BSBoneSwitchGenerator")),
      childUI(new BSBoneSwitchGeneratorBoneDataUI),
      topLyt(new QGridLayout),
      typeSelectorCB(new ComboBox),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit)
{
    table->setAcceptDrops(true);
    table->viewport()->setAcceptDrops(true);
    table->setDragDropOverwriteMode(true);
    table->setDropIndicatorShown(true);
    table->setDragDropMode(QAbstractItemView::InternalMove);
    table->setRowSwapRange(BASE_NUMBER_OF_ROWS);
    typeSelectorCB->insertItems(0, types);
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(DEFAULT_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("pDefaultGenerator"));
    table->setItem(DEFAULT_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbGenerator", Qt::AlignCenter));
    table->setItem(DEFAULT_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(DEFAULT_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    table->setItem(ADD_CHILD_ROW, NAME_COLUMN, new TableWidgetItem("Add Bone Data With Generator", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new bone switch generator bone data with a generator of the type specified in the adjacent combo box"));
    table->setCellWidget(ADD_CHILD_ROW, TYPE_COLUMN, typeSelectorCB);
    table->setItem(ADD_CHILD_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Bone Data", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected bone switch generator bone data"));
    table->setItem(ADD_CHILD_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Bone Data", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected bone switch generator bone data"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(childUI);
    childUI->returnPB->setVisible(true);
    toggleSignals(true);
}

void BSBoneSwitchGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(childUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)));
        disconnect(childUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)));
        disconnect(childUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(childUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(childUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)));
    }
}

void BSBoneSwitchGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == BS_BONE_SWITCH_GENERATOR){
            bsData = static_cast<BSBoneSwitchGenerator *>(data);
            name->setText(bsData->getName());
            table->item(DEFAULT_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getDefaultGeneratorName());
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("BSBoneSwitchGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void BSBoneSwitchGeneratorUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_CHILD_ROW + bsData->ChildrenA.size() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        BSBoneSwitchGeneratorBoneData *child = nullptr;
        for (auto i = ADD_CHILD_ROW + 1, j = 0; j < bsData->ChildrenA.size(); i++, j++){
            child = static_cast<BSBoneSwitchGeneratorBoneData *>(bsData->ChildrenA.at(j).data());
            if (child){
                UIHelper::setRowItems(i, "Bone Data "+QString::number(j), child->getClassname(), "Remove", "Edit", "Double click to remove this bone data", "Double click to edit this bone data", table);
            }else{
                LogFile::writeToLog("BSBoneSwitchGeneratorUI::loadData(): Null bone data found!!!");
            }
        }
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        bsData->updateChildIconNames();
        emit generatorNameChanged(bsData->getName(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::swapGeneratorIndices(int index1, int index2){
    if (bsData){
        index1 = index1 - BASE_NUMBER_OF_ROWS;
        index2 = index2 - BASE_NUMBER_OF_ROWS;
        if (!bsData->swapChildren(index1, index2)){
            WARNING_MESSAGE("Cannot swap these rows!!");
        }
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::swapGeneratorIndices(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::addChildWithGenerator(){
    Generator_Type typeEnum;
    if (bsData && behaviorView){
        typeEnum = static_cast<Generator_Type>(typeSelectorCB->currentIndex());
        behaviorView->appendBoneSwitchGeneratorChild();
        switch (typeEnum){
        case STATE_MACHINE:
            behaviorView->appendStateMachine(); break;
        case MANUAL_SELECTOR_GENERATOR:
            behaviorView->appendManualSelectorGenerator(); break;
        case BLENDER_GENERATOR:
            behaviorView->appendBlenderGenerator(); break;
        case I_STATE_TAGGING_GENERATOR:
            behaviorView->appendIStateTaggingGenerator(); break;
        case BONE_SWITCH_GENERATOR:
            behaviorView->appendBoneSwitchGenerator(); break;
        case CYCLIC_BLEND_TRANSITION_GENERATOR:
            behaviorView->appendCyclicBlendTransitionGenerator(); break;
        case SYNCHRONIZED_CLIP_GENERATOR:
            behaviorView->appendSynchronizedClipGenerator(); break;
        case MODIFIER_GENERATOR:
            behaviorView->appendModifierGenerator(); break;
        case OFFSET_ANIMATION_GENERATOR:
            behaviorView->appendOffsetAnimationGenerator(); break;
        case POSE_MATCHING_GENERATOR:
            behaviorView->appendPoseMatchingGenerator(); break;
        case CLIP_GENERATOR:
            behaviorView->appendClipGenerator(); break;
        case BEHAVIOR_REFERENCE_GENERATOR:
            behaviorView->appendBehaviorReferenceGenerator(); break;
        case GAMEBYRO_SEQUENCE_GENERATOR:
            behaviorView->appendBGSGamebryoSequenceGenerator(); break;
        default:
            LogFile::writeToLog("BSBoneSwitchGeneratorUI::addChild(): Invalid typeEnum!!");
            return;
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::addChild(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::removeChild(int index){
    BSBoneSwitchGeneratorBoneData *child = nullptr;
    if (bsData && behaviorView){
        if (index < bsData->ChildrenA.size() && index >= 0){
            child = static_cast<BSBoneSwitchGeneratorBoneData *>(bsData->ChildrenA.at(index).data());
            behaviorView->removeItemFromGraph(behaviorView->getSelectedIconsChildIcon(child->getChildren().first()), index);
            behaviorView->removeObjects();
        }else{
            WARNING_MESSAGE("Invalid index of child to remove!!");
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::removeChild(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row == DEFAULT_GENERATOR_ROW && column == VALUE_COLUMN){
            QStringList list = {hkbStateMachineStateInfo::getClassname(), hkbBlenderGeneratorChild::getClassname(), BSBoneSwitchGeneratorBoneData::getClassname()};
            emit viewGenerators(bsData->getIndexOfGenerator(bsData->pDefaultGenerator) + 1, QString(), list);
        }else if (row == ADD_CHILD_ROW && column == NAME_COLUMN){
            addChildWithGenerator();
        }else if (row > ADD_CHILD_ROW && row < ADD_CHILD_ROW + bsData->ChildrenA.size() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->ChildrenA.size() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    childUI->loadData(static_cast<BSBoneSwitchGeneratorBoneData *>(bsData->ChildrenA.at(result).data()), result);
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the bone data \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeChild(result);
                    }
                }
            }else{
                LogFile::writeToLog("BSBoneSwitchGeneratorUI::viewSelectedChild(): Invalid index of bone data to view!!");
            }
        }
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::setDefaultGenerator(int index, const QString & name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->pDefaultGenerator.data()), NULL_SIGNATURE, HkxObject::TYPE_GENERATOR, table, behaviorView, DEFAULT_GENERATOR_ROW, VALUE_COLUMN);
}

void BSBoneSwitchGeneratorUI::returnToWidget(bool reloadData){
    (reloadData) ? loadDynamicTableRows() : NULL;
    setCurrentIndex(MAIN_WIDGET);
}

void BSBoneSwitchGeneratorUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case CHILD_WIDGET:
        childUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("BSBoneSwitchGeneratorUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BSBoneSwitchGeneratorUI::generatorTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setDefaultGenerator(index, name); break;
    case CHILD_WIDGET:
        childUI->setGenerator(index, name); break;
    default:
        WARNING_MESSAGE("BSBoneSwitchGeneratorUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BSBoneSwitchGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties){
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
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        (currentIndex() == CHILD_WIDGET) ? childUI->variableRenamed(name, --index) : NULL;
    }else{
        LogFile::writeToLog("BSBoneSwitchGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}

void BSBoneSwitchGeneratorUI::generatorRenamed(const QString &name, int index){
    if (--index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->pDefaultGenerator)){
        table->item(DEFAULT_GENERATOR_ROW, VALUE_COLUMN)->setText(name);
    }
    switch (currentIndex()){
    case CHILD_WIDGET:
        childUI->generatorRenamed(name, index); break;
    }
}

void BSBoneSwitchGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
    setCurrentIndex(MAIN_WIDGET);
    childUI->setBehaviorView(view);
}

