#include "modifierlistui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbmodifierlist.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/mainwindow.h"

#include <QHeaderView>

#include "src/ui/genericdatawidgets.h"

using namespace UI;

#define BASE_NUMBER_OF_ROWS 3

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ADD_MODIFIER_ROW 2

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ModifierListUI::types = {
    "hkbModifierList",
    "hkbTwistModifier",
    "hkbEventDrivenModifier",
    "BSIsActiveModifier",
    "BSLimbIKModifier",
    "BSInterpValueModifier",
    "BSGetTimeStepModifier",
    "hkbFootIkControlsModifier",
    "hkbGetHandleOnBoneModifier",
    "hkbTransformVectorModifier",
    "hkbProxyModifier",
    "hkbLookAtModifier",
    "hkbMirrorModifier",
    "hkbGetWorldFromModelModifier",
    "hkbSenseHandleModifier",
    "hkbEvaluateExpressionModifier",
    "hkbEvaluateHandleModifier",
    "hkbAttachmentModifier",
    "hkbAttributeModifier",
    "hkbCombineTransformsModifier",
    "hkbComputeRotationFromAxisAngleModifier",
    "hkbComputeRotationToTargetModifier",
    "hkbEventsFromRangeModifier",
    "hkbMoveCharacterModifier",
    "hkbExtractRagdollPoseModifier",
    "BSModifyOnceModifier",
    "BSEventOnDeactivateModifier",
    "BSEventEveryNEventsModifier",
    "BSRagdollContactListenerModifier",
    "hkbPoweredRagdollControlsModifier",
    "BSEventOnFalseToTrueModifier",
    "BSDirectAtModifier",
    "BSDistTriggerModifier",
    "BSDecomposeVectorModifier",
    "BSComputeAddBoneAnimModifier",
    "BSTweenerModifier",
    "BSIStateManagerModifier",
    "hkbTimerModifier",
    "hkbRotateCharacterModifier",
    "hkbDampingModifier",
    "hkbDelayedModifier",
    "hkbGetUpModifier",
    "hkbKeyframeBonesModifier",
    "hkbComputeDirectionModifier",
    "hkbRigidBodyRagdollControlsModifier",
    "BSSpeedSamplerModifier",
    "hkbDetectCloseToGroundModifier",
    "BSLookAtModifier",
    "BSTimerModifier",
    "BSPassByTargetTriggerModifier",
    "hkbHandIKControlsModifier"
};

const QStringList ModifierListUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ModifierListUI::ModifierListUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      typeSelectorCB(new ComboBox)
{
    table->setAcceptDrops(true);
    table->viewport()->setAcceptDrops(true);
    table->setDragDropOverwriteMode(true);
    table->setDropIndicatorShown(true);
    table->setDragDropMode(QAbstractItemView::InternalMove);
    table->setRowSwapRange(BASE_NUMBER_OF_ROWS);
    setTitle("hkbModifierList");
    typeSelectorCB->insertItems(0, types);
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
    //table->setItem(ENABLE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(ADD_MODIFIER_ROW, NAME_COLUMN, new TableWidgetItem("Add Modifier", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new modifier of the type specified in the adjacent combo box"));
    table->setCellWidget(ADD_MODIFIER_ROW, TYPE_COLUMN, typeSelectorCB);
    table->setItem(ADD_MODIFIER_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Modifier", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected modifier"));
    table->setItem(ADD_MODIFIER_ROW, VALUE_COLUMN, new TableWidgetItem("Change Selected Modifier", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to change the selected modifier"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ModifierListUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)));
    }
}

void ModifierListUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_MODIFIER_LIST){
            bsData = static_cast<hkbModifierList *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("ModifierListUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("ModifierListUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void ModifierListUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_MODIFIER_ROW + bsData->modifiers.size() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_MODIFIER_ROW + 1, j = 0; j < bsData->modifiers.size(); i++, j++){
            auto child = bsData->getModifierAt(j);
            if (child){
                UIHelper::setRowItems(i, child->getName(), child->getClassname(), "Remove", "Edit", "Double click to remove this modifier", VIEW_MODIFIERS_TABLE_TIP, table);
            }else{
                LogFile::writeToLog("ModifierListUI::loadData(): Null state found!!!");
            }
        }
    }else{
        LogFile::writeToLog("ModifierListUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void ModifierListUI::connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties && modifiers){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(modifiers, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(modifiers, SIGNAL(elementSelected(int,QString)), this, SLOT(setModifier(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewModifiers(int,QString,QStringList)), modifiers, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("ModifierListUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ModifierListUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto isProperty = false;
        auto row = table->currentRow();
        if (row == ENABLE_ROW){
            (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : NULL;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, "enable", VARIABLE_TYPE_BOOL, isProperty, table, bsData);
        }
    }else{
        LogFile::writeToLog("ModifierListUI::setBindingVariable(): The data is nullptr!!");
    }
}

void ModifierListUI::variableRenamed(const QString &name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto bindIndex = bind->getVariableIndexOfBinding("enable");
            (bindIndex == index) ? table->item(ENABLE_ROW, BINDING_COLUMN)->setText(name) : NULL;
        }
    }else{
        LogFile::writeToLog("ModifierListUI::variableRenamed(): The data is nullptr!!");
    }
}

void ModifierListUI::modifierRenamed(const QString &name, int index){
    auto modifierIndex = table->currentRow() - BASE_NUMBER_OF_ROWS;
    if (bsData){
        if (modifierIndex >= 0 && modifierIndex < bsData->modifiers.size()){
            index--;
            if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getModifierAt(modifierIndex))){
                table->item(table->currentRow(), VALUE_COLUMN)->setText(name);
            }
        }else{
            WARNING_MESSAGE("ModifierListUI::generatorRenamed(): Invalid modifier index selected!!");
        }
    }else{
        LogFile::writeToLog("ModifierListUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void ModifierListUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}

void ModifierListUI::selectTableToView(bool viewproperties, const QString &path){
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
        LogFile::writeToLog("ModifierListUI::selectTableToView(): The data is nullptr!!");
    }
}

void ModifierListUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("ModifierListUI::setName(): The data is nullptr!!");
    }
}

void ModifierListUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("ModifierListUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void ModifierListUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row < ADD_MODIFIER_ROW && row >= 0){
            auto properties = false;
            if (column == BINDING_COLUMN && row == ENABLE_ROW){
                (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : NULL;
                selectTableToView(properties, "enable");
            }
        }else if (row == ADD_MODIFIER_ROW && column == NAME_COLUMN){
            addModifier();
        }else if (row > ADD_MODIFIER_ROW && row < ADD_MODIFIER_ROW + bsData->getNumberOfModifiers() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfModifiers() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    emit viewModifiers(static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getModifierAt(result)) + 1, QString(), QStringList());
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the modifier \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeModifier(result);
                    }
                }
            }else{
                LogFile::writeToLog("ModifierListUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("ModifierListUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void ModifierListUI::swapGeneratorIndices(int index1, int index2){
    HkxObject *gen1;
    HkxObject *gen2;
    if (bsData){
        index1 = index1 - BASE_NUMBER_OF_ROWS;
        index2 = index2 - BASE_NUMBER_OF_ROWS;
        if (bsData->modifiers.size() > index1 && bsData->modifiers.size() > index2 && index1 != index2 && index1 >= 0 && index2 >= 0){
            gen1 = bsData->modifiers.at(index1).data();
            gen2 = bsData->modifiers.at(index2).data();
            bsData->modifiers[index1] = HkxSharedPtr(gen2);
            bsData->modifiers[index2] = HkxSharedPtr(gen1);
            behaviorView->getSelectedItem()->reorderChildren();
            bsData->setIsFileChanged(true);
        }else{
            WARNING_MESSAGE("ModifierListUI::swapGeneratorIndices(): Cannot swap these rows!!");
        }
    }else{
        LogFile::writeToLog("ModifierListUI::swapGeneratorIndices(): The data is nullptr!!");
    }
}

void ModifierListUI::setModifier(int index, const QString &name){
    auto modifierIndex = table->currentRow() - BASE_NUMBER_OF_ROWS;
    if (bsData){
        if (modifierIndex >= 0 && modifierIndex < bsData->getNumberOfModifiers()){
            UIHelper::setModifier(index, name, bsData, bsData->getModifierAt(modifierIndex), NULL_SIGNATURE, HkxObject::TYPE_MODIFIER, table, behaviorView, modifierIndex, VALUE_COLUMN);
        }else{
            LogFile::writeToLog("ModifierListUI::setModifier(): Invalid modifier index selected!!");
        }
    }else{
        LogFile::writeToLog("ModifierListUI::setModifier(): The 'bsData' pointer is nullptr!!");
    }
}

void ModifierListUI::addModifier(){
    if (bsData && behaviorView){
        auto text = typeSelectorCB->currentText();
        if (text == "hkbModifierList"){
            behaviorView->appendModifierList();
        }else if (text == "hkbTwistModifier"){
            behaviorView->appendTwistModifier();
        }else if (text == "hkbEventDrivenModifier"){
            behaviorView->appendEventDrivenModifier();
        }else if (text == "BSIsActiveModifier"){
            behaviorView->appendIsActiveModifier();
        }else if (text == "BSLimbIKModifier"){
            behaviorView->appendLimbIKModifier();
        }else if (text == "BSInterpValueModifier"){
            behaviorView->appendInterpValueModifier();
        }else if (text == "BSGetTimeStepModifier"){
            behaviorView->appendGetTimeStepModifier();
        }else if (text == "hkbFootIkControlsModifier"){
            behaviorView->appendFootIkControlsModifier();
        }else if (text == "hkbGetHandleOnBoneModifier"){
            behaviorView->appendGetHandleOnBoneModifier();
        }else if (text == "hkbTransformVectorModifier"){
            behaviorView->appendTransformVectorModifier();
        }else if (text == "hkbProxyModifier"){
            behaviorView->appendProxyModifier();
        }else if (text == "hkbLookAtModifier"){
            behaviorView->appendLookAtModifier();
        }else if (text == "hkbMirrorModifier"){
            behaviorView->appendMirrorModifier();
        }else if (text == "hkbGetWorldFromModelModifier"){
            behaviorView->appendGetWorldFromModelModifier();
        }else if (text == "hkbSenseHandleModifier"){
            behaviorView->appendSenseHandleModifier();
        }else if (text == "hkbEvaluateExpressionModifier"){
            behaviorView->appendEvaluateExpressionModifier();
        }else if (text == "hkbEvaluateHandleModifier"){
            behaviorView->appendEvaluateHandleModifier();
        }else if (text == "hkbAttachmentModifier"){
            behaviorView->appendAttachmentModifier();
        }else if (text == "hkbAttributeModifier"){
            behaviorView->appendAttributeModifier();
        }else if (text == "hkbCombineTransformsModifier"){
            behaviorView->appendCombineTransformsModifier();
        }else if (text == "hkbComputeRotationFromAxisAngleModifier"){
            behaviorView->appendComputeRotationFromAxisAngleModifier();
        }else if (text == "hkbComputeRotationToTargetModifier"){
            behaviorView->appendComputeRotationToTargetModifier();
        }else if (text == "hkbEventsFromRangeModifier"){
            behaviorView->appendEventsFromRangeModifier();
        }else if (text == "hkbMoveCharacterModifier"){
            behaviorView->appendMoveCharacterModifier();
        }else if (text == "hkbExtractRagdollPoseModifier"){
            behaviorView->appendExtractRagdollPoseModifier();
        }else if (text == "BSModifyOnceModifier"){
            behaviorView->appendModifyOnceModifier();
        }else if (text == "BSEventOnDeactivateModifier"){
            behaviorView->appendEventOnDeactivateModifier();
        }else if (text == "BSEventEveryNEventsModifier"){
            behaviorView->appendEventEveryNEventsModifier();
        }else if (text == "BSRagdollContactListenerModifier"){
            behaviorView->appendRagdollContactListenerModifier();
        }else if (text == "hkbPoweredRagdollControlsModifier"){
            behaviorView->appendPoweredRagdollControlsModifier();
        }else if (text == "BSEventOnFalseToTrueModifier"){
            behaviorView->appendEventOnFalseToTrueModifier();
        }else if (text == "BSDirectAtModifier"){
            behaviorView->appendDirectAtModifier();
        }else if (text == "BSDistTriggerModifier"){
            behaviorView->appendDistTriggerModifier();
        }else if (text == "BSDecomposeVectorModifier"){
            behaviorView->appendDecomposeVectorModifier();
        }else if (text == "BSComputeAddBoneAnimModifier"){
            behaviorView->appendComputeAddBoneAnimModifier();
        }else if (text == "BSTweenerModifier"){
            behaviorView->appendTweenerModifier();
        }else if (text == "BSIStateManagerModifier"){
            behaviorView->appendBSIStateManagerModifier();
        }else if (text == "hkbTimerModifier"){
            behaviorView->appendTimerModifier();
        }else if (text == "hkbRotateCharacterModifier"){
            behaviorView->appendRotateCharacterModifier();
        }else if (text == "hkbDampingModifier"){
            behaviorView->appendDampingModifier();
        }else if (text == "hkbDelayedModifier"){
            behaviorView->appendDelayedModifier();
        }else if (text == "hkbGetUpModifier"){
            behaviorView->appendGetUpModifier();
        }else if (text == "hkbKeyframeBonesModifier"){
            behaviorView->appendKeyframeBonesModifier();
        }else if (text == "hkbComputeDirectionModifier"){
            behaviorView->appendComputeDirectionModifier();
        }else if (text == "hkbRigidBodyRagdollControlsModifier"){
            behaviorView->appendRigidBodyRagdollControlsModifier();
        }else if (text == "BSSpeedSamplerModifier"){
            behaviorView->appendBSSpeedSamplerModifier();
        }else if (text == "hkbDetectCloseToGroundModifier"){
            behaviorView->appendDetectCloseToGroundModifier();
        }else if (text == "BSLookAtModifier"){
            behaviorView->appendBSLookAtModifier();
        }else if (text == "BSTimerModifier"){
            behaviorView->appendBSTimerModifier();
        }else if (text == "BSPassByTargetTriggerModifier"){
            behaviorView->appendBSPassByTargetTriggerModifier();
        }else if (text == "hkbHandIKControlsModifier"){
            behaviorView->appendHandIKControlsModifier();
        }else{
            LogFile::writeToLog("ModifierListUI::addModifier(): Invalid type!!");
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("ModifierListUI::addModifier(): The data is nullptr!!");
    }
}

void ModifierListUI::removeModifier(int index){
    if (bsData && behaviorView){
        if (index < bsData->getNumberOfModifiers() && index >= 0){
            behaviorView->removeItemFromGraph(behaviorView->getSelectedIconsChildIcon(bsData->getModifierAt(index)), index);
            behaviorView->removeObjects();
        }else{
            WARNING_MESSAGE("ModifierListUI::removeModifier(): Invalid index of generator to remove!!");
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("ModifierListUI::removeModifier(): The data or behaviorView is nullptr!!");
    }
}
