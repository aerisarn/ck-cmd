#include "ManualSelectorGeneratorUI.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/generators/hkbmanualselectorgenerator.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/mainwindow.h"

#include <QHeaderView>

#include "src/ui/genericdatawidgets.h"

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define SELECTED_GENERATOR_INDEX_ROW 1
#define CURRENT_GENERATOR_INDEX_ROW 2
#define ADD_GENERATOR_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ManualSelectorGeneratorUI::types = {
    "None",
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

const QStringList ManualSelectorGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ManualSelectorGeneratorUI::ManualSelectorGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      typeSelectorCB(new ComboBox),
      selectedGeneratorIndex(new SpinBox),
      currentGeneratorIndex(new SpinBox)
{
    table->setAcceptDrops(true);
    table->viewport()->setAcceptDrops(true);
    table->setDragDropOverwriteMode(true);
    table->setDropIndicatorShown(true);
    table->setDragDropMode(QAbstractItemView::InternalMove);
    table->setRowSwapRange(BASE_NUMBER_OF_ROWS);
    setTitle("hkbManualSelectorGenerator");
    typeSelectorCB->insertItems(0, types);
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(SELECTED_GENERATOR_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("selectedGeneratorIndex"));
    table->setItem(SELECTED_GENERATOR_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(SELECTED_GENERATOR_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(SELECTED_GENERATOR_INDEX_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(SELECTED_GENERATOR_INDEX_ROW, VALUE_COLUMN, selectedGeneratorIndex);
    table->setItem(CURRENT_GENERATOR_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("currentGeneratorIndex"));
    table->setItem(CURRENT_GENERATOR_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(CURRENT_GENERATOR_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(CURRENT_GENERATOR_INDEX_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(CURRENT_GENERATOR_INDEX_ROW, VALUE_COLUMN, currentGeneratorIndex);
    table->setItem(ADD_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("Add Generator", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new generator of the type specified in the adjacent combo box"));
    table->setCellWidget(ADD_GENERATOR_ROW, TYPE_COLUMN, typeSelectorCB);
    table->setItem(ADD_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Generator", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected generator"));
    table->setItem(ADD_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("Change Selected Generator", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to change the selected generator"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ManualSelectorGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(selectedGeneratorIndex, SIGNAL(editingFinished()), this, SLOT(setSelectedGeneratorIndex()), Qt::UniqueConnection);
        connect(currentGeneratorIndex, SIGNAL(editingFinished()), this, SLOT(setCurrentGeneratorIndex()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(selectedGeneratorIndex, SIGNAL(editingFinished()), this, SLOT(setSelectedGeneratorIndex()));
        disconnect(currentGeneratorIndex, SIGNAL(editingFinished()), this, SLOT(setCurrentGeneratorIndex()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)));
    }
}

void ManualSelectorGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_MANUAL_SELECTOR_GENERATOR){
            bsData = static_cast<hkbManualSelectorGenerator *>(data);
            name->setText(bsData->getName());
            selectedGeneratorIndex->setValue(bsData->getSelectedGeneratorIndex());
            currentGeneratorIndex->setValue(bsData->getCurrentGeneratorIndex());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(SELECTED_GENERATOR_INDEX_ROW, BINDING_COLUMN, varBind, "selectedGeneratorIndex", table, bsData);
            UIHelper::loadBinding(CURRENT_GENERATOR_INDEX_ROW, BINDING_COLUMN, varBind, "currentGeneratorIndex", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("ManualSelectorGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void ManualSelectorGeneratorUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_GENERATOR_ROW + bsData->generators.size() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_GENERATOR_ROW + 1, j = 0; j < bsData->generators.size(); i++, j++){
            auto child = static_cast<hkbGenerator *>(bsData->generators.at(j).data());  //TO DO..
            if (child){
                UIHelper::setRowItems(i, child->getName(), child->getClassname(), "Remove", "Edit", "Double click to remove this generator", VIEW_GENERATORS_TABLE_TIP, table);
            }else{
                UIHelper::setRowItems(i, "NONE", "NONE", "Remove", "Edit", "Double click to remove this generator", VIEW_GENERATORS_TABLE_TIP, table);
            }
        }
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties){
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
        LogFile::writeToLog("ManualSelectorGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ManualSelectorGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case SELECTED_GENERATOR_INDEX_ROW:
            checkisproperty(SELECTED_GENERATOR_INDEX_ROW, "selectedGeneratorIndex", VARIABLE_TYPE_INT32); break;
        case CURRENT_GENERATOR_INDEX_ROW:
            checkisproperty(CURRENT_GENERATOR_INDEX_ROW, "currentGeneratorIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::setBindingVariable(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::variableRenamed(const QString &name, int index){
    if (bsData){
        if (name != ""){
            index--;
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto setname = [&](const QString & fieldname, int row){
                    auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                    (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
                };
                setname("selectedGeneratorIndex", SELECTED_GENERATOR_INDEX_ROW);
                setname("currentGeneratorIndex", CURRENT_GENERATOR_INDEX_ROW);
            }
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::generatorRenamed(const QString &name, int index){
    auto generatorIndex = table->currentRow() - BASE_NUMBER_OF_ROWS;
    if (bsData){
        if (generatorIndex >= 0 && generatorIndex < bsData->generators.size()){
            index--;
            if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->generators.at(generatorIndex))){
                table->item(table->currentRow(), VALUE_COLUMN)->setText(name);
            }
        }else{
            WARNING_MESSAGE("ManualSelectorGeneratorUI::generatorRenamed(): Invalid generator index selected!!");
        }
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}

void ManualSelectorGeneratorUI::selectTableToView(bool viewproperties, const QString &path){
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
        LogFile::writeToLog("ManualSelectorGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::setName(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::setSelectedGeneratorIndex(){
    (bsData) ? bsData->setSelectedGeneratorIndex(selectedGeneratorIndex->value()) : LogFile::writeToLog("ManualSelectorGeneratorUI::setSelectedGeneratorIndex(): The data is nullptr!!");
}

void ManualSelectorGeneratorUI::setCurrentGeneratorIndex(){
    (bsData) ? bsData->setCurrentGeneratorIndex(currentGeneratorIndex->value()) : LogFile::writeToLog("ManualSelectorGeneratorUI::setCurrentGeneratorIndex(): The data is nullptr!!");
}

void ManualSelectorGeneratorUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_GENERATOR_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case SELECTED_GENERATOR_INDEX_ROW:
                    checkisproperty(SELECTED_GENERATOR_INDEX_ROW, "selectedGeneratorIndex"); break;
                case CURRENT_GENERATOR_INDEX_ROW:
                    checkisproperty(CURRENT_GENERATOR_INDEX_ROW, "currentGeneratorIndex"); break;
                }
            }
        }else if (row == ADD_GENERATOR_ROW && column == NAME_COLUMN){
            addGenerator();
        }else if (row > ADD_GENERATOR_ROW && row < ADD_GENERATOR_ROW + bsData->generators.size() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->generators.size() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    QStringList list = {hkbStateMachineStateInfo::getClassname(), hkbBlenderGeneratorChild::getClassname(), BSBoneSwitchGeneratorBoneData::getClassname()};
                    emit viewGenerators(bsData->getIndexOfGenerator(bsData->generators.at(result)) + 1, QString(), list);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the generator \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeGenerator(result);
                    }
                }
            }else{
                WARNING_MESSAGE("ManualSelectorGeneratorUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::swapGeneratorIndices(int index1, int index2){
    if (bsData){
        index1 = index1 - BASE_NUMBER_OF_ROWS;
        index2 = index2 - BASE_NUMBER_OF_ROWS;
        if (!bsData->swapChildren(index1, index2)){
            WARNING_MESSAGE("Cannot swap these rows!!");
        }else{  //TO DO: check if necessary...
            (behaviorView->getSelectedItem()) ? behaviorView->getSelectedItem()->reorderChildren() : LogFile::writeToLog("ManualSelectorGeneratorUI::swapGeneratorIndices(): No item selected!!");
        }
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::swapGeneratorIndices(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::setGenerator(int index, const QString &name){
    if (index < bsData->generators.size() && index >= 0){
        UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->generators.at(index).data()), NULL_SIGNATURE, HkxObject::TYPE_GENERATOR, table, behaviorView, (ADD_GENERATOR_ROW + index + 1), VALUE_COLUMN);
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::setGenerator(): Invalid generator index selected!!");
    }
}

void ManualSelectorGeneratorUI::addGenerator(){
    if (bsData && behaviorView){
        auto typeEnum = static_cast<Generator_Type>(typeSelectorCB->currentIndex());
        switch (typeEnum){
        case NONE:
            bsData->generators.append(HkxSharedPtr()); break;
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
            LogFile::writeToLog("ManualSelectorGeneratorUI::addGenerator(): Invalid typeEnum!!");
            return;
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::addGenerator(): The data is nullptr!!");
    }
}

void ManualSelectorGeneratorUI::removeGenerator(int index){
    if (bsData && behaviorView){
        if (index < bsData->generators.size() && index >= 0){
            if (!bsData->generators.at(index).data()){
                bsData->generators.removeAt(index);
            }else{
                behaviorView->removeItemFromGraph(behaviorView->getSelectedIconsChildIcon(bsData->generators.at(index).data()), index);
                behaviorView->removeObjects();
            }
        }else{
            WARNING_MESSAGE("Invalid index of generator to remove!!");
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("ManualSelectorGeneratorUI::removeGenerator(): The data is nullptr!!");
    }
}
