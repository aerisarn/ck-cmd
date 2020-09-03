#include "behaviorvariablesui.h"

#include "src/ui/treegraphicsitem.h"
#include "src/ui/hkxclassesui/behaviorui/boneweightarrayui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/hkxclasses/behavior/generators/hkbgenerator.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/hkxclasses/behavior/generators/bsistatetagginggenerator.h"
#include "src/hkxclasses/behavior/generators/hkbmodifiergenerator.h"
#include "src/ui/hkxclassesui/behaviorui/generators/bsistatetagginggeneratorui.h"
#include "src/ui/hkxclassesui/behaviorui/generators/modifiergeneratorui.h"
#include "src/ui/hkxclassesui/behaviorui/generators/manualselectorgeneratorui.h"

#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphstringdata.h"
#include "src/hkxclasses/behavior/hkbvariablevalueset.h"

#include <QPushButton>
#include <QMessageBox>
#include <QSignalMapper>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>

#include "src/ui/genericdatawidgets.h"
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStackedLayout>

using namespace UI;

const QStringList BehaviorVariablesUI::types = {
    "BOOL",
    "INT32",
    "REAL",
    "POINTER",
    "VECTOR4",
    "QUATERNION"
};

const QStringList BehaviorVariablesUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

BehaviorVariablesUI::BehaviorVariablesUI(const QString &title)
    : dataUI(nullptr),
      verLyt(new QVBoxLayout),
      loadedData(nullptr),
      table(new TableWidget),
      addObjectPB(new QPushButton("Add Variable")),
      removeObjectPB(new QPushButton("Remove Selected Variable")),
      buttonLyt(new QHBoxLayout),
      boolName(new LineEdit),
      intName(new LineEdit),
      doubleName(new LineEdit),
      quadName(new LineEdit),
      boolCB(new CheckBox),
      intSB(new SpinBox),
      doubleSB(new DoubleSpinBox),
      quadWidget(new QuadVariableWidget),
      variableWidget(new TableWidget),
      boneWeightArrayWidget(new BoneWeightArrayUI()),
      stackLyt(new QStackedLayout),
      returnBoolPB(new QPushButton("Return To Parent")),
      returnIntPB(new QPushButton("Return To Parent")),
      returnDoublePB(new QPushButton("Return To Parent")),
      returnQuadPB(new QPushButton("Return To Parent")),
      typeSelector(new ComboBox),
      nameMapper(new QSignalMapper),
      valueMapper(new QSignalMapper)
{
    setTitle(title);
    //setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    stackLyt->addWidget(table);
    stackLyt->addWidget(variableWidget);
    stackLyt->addWidget(boneWeightArrayWidget);
    stackLyt->setCurrentIndex(TABLE_WIDGET);
    typeSelector->insertItems(0, types);
    buttonLyt->addWidget(addObjectPB, 1);
    buttonLyt->addWidget(typeSelector, 2);
    buttonLyt->addWidget(removeObjectPB, 1);
    //table->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels);
    verLyt->addLayout(buttonLyt, 1);
    verLyt->addLayout(stackLyt, 10);
    variableWidget->setRowCount(4);
    variableWidget->setColumnCount(3);
    variableWidget->setCellWidget(0, 0, boolName);
    variableWidget->setCellWidget(0, 1, boolCB);
    variableWidget->setCellWidget(0, 2, returnBoolPB);
    variableWidget->setCellWidget(1, 0, intName);
    variableWidget->setCellWidget(1, 1, intSB);
    variableWidget->setCellWidget(1, 2, returnIntPB);
    variableWidget->setCellWidget(2, 0, doubleName);
    variableWidget->setCellWidget(2, 1, doubleSB);
    variableWidget->setCellWidget(2, 2, returnDoublePB);
    variableWidget->setCellWidget(3, 0, quadName);
    variableWidget->setCellWidget(3, 1, quadWidget);
    variableWidget->setCellWidget(3, 2, returnQuadPB);
    nameMapper->setMapping(boolName, 0);
    nameMapper->setMapping(intName, 1);
    nameMapper->setMapping(doubleName, 2);
    nameMapper->setMapping(quadName, 3);
    connect(boolName, SIGNAL(editingFinished()), nameMapper, SLOT(map()), Qt::UniqueConnection);
    connect(intName, SIGNAL(editingFinished()), nameMapper, SLOT(map()), Qt::UniqueConnection);
    connect(doubleName, SIGNAL(editingFinished()), nameMapper, SLOT(map()), Qt::UniqueConnection);
    connect(quadName, SIGNAL(editingFinished()), nameMapper, SLOT(map()), Qt::UniqueConnection);
    valueMapper->setMapping(boolCB, 0);
    valueMapper->setMapping(intSB, 1);
    valueMapper->setMapping(doubleSB, 2);
    valueMapper->setMapping(quadWidget, 3);
    connect(boolCB, SIGNAL(released()), valueMapper, SLOT(map()), Qt::UniqueConnection);
    connect(intSB, SIGNAL(editingFinished()), valueMapper, SLOT(map()), Qt::UniqueConnection);
    connect(doubleSB, SIGNAL(editingFinished()), valueMapper, SLOT(map()), Qt::UniqueConnection);
    connect(quadWidget, SIGNAL(editingFinished()), valueMapper, SLOT(map()), Qt::UniqueConnection);
    connect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeVariable()), Qt::UniqueConnection);
    connect(addObjectPB, SIGNAL(pressed()), this, SLOT(addVariable()), Qt::UniqueConnection);
    connect(nameMapper, SIGNAL(mapped(int)), this, SLOT(renameSelectedVariable(int)), Qt::UniqueConnection);
    connect(valueMapper, SIGNAL(mapped(int)), this, SLOT(setVariableValue(int)), Qt::UniqueConnection);
    connect(table, SIGNAL(cellClicked(int,int)), this, SLOT(viewVariable(int,int)), Qt::UniqueConnection);
    connect(returnBoolPB, SIGNAL(released()), this, SLOT(returnToTable()), Qt::UniqueConnection);
    connect(returnIntPB, SIGNAL(released()), this, SLOT(returnToTable()), Qt::UniqueConnection);
    connect(returnDoublePB, SIGNAL(released()), this, SLOT(returnToTable()), Qt::UniqueConnection);
    connect(returnQuadPB, SIGNAL(released()), this, SLOT(returnToTable()), Qt::UniqueConnection);
    connect(boneWeightArrayWidget, SIGNAL(returnToParent()), this, SLOT(returnToTable()), Qt::UniqueConnection);
    setLayout(verLyt);
}

/*QSize BehaviorVariablesUI::sizeHint() const{
    return QSize(300, 300);
}*/

void BehaviorVariablesUI::setVariableValue(int type){
    if (loadedData){
        auto index = table->currentRow();
        switch (type){
        case VARIABLE_TYPE_BOOL:
            loadedData->setWordVariableValueAt(index, boolCB->isChecked()); break;
        case VARIABLE_TYPE_INT32:
            loadedData->setWordVariableValueAt(index, intSB->value()); break;
        case VARIABLE_TYPE_REAL:
            loadedData->setWordVariableValueAt(index, doubleSB->value()); break;
        case VARIABLE_TYPE_POINTER:
            loadedData->setWordVariableValueAt(index, intSB->value()); break;
        case VARIABLE_TYPE_VECTOR4:
        case VARIABLE_TYPE_QUATERNION:
            loadedData->setQuadVariableValueAt(index, quadWidget->value()); break;
        }
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::renameSelectedVariable(int type){
    if (loadedData){
        QString newName;
        switch (type){
        case VARIABLE_TYPE_BOOL:
            newName = boolName->text(); break;
        case VARIABLE_TYPE_INT32:
            newName = intName->text(); break;
        case VARIABLE_TYPE_REAL:
            newName = doubleName->text(); break;
        case VARIABLE_TYPE_VECTOR4:
        case VARIABLE_TYPE_QUATERNION:
            newName = quadName->text(); break;
        }
        table->item(table->currentRow(), 0)->setText(newName);
        loadedData->setVariableNameAt(table->currentRow(), newName);
        emit variableNameChanged(newName, table->currentRow());
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::removeVariableFromTable(int row){
    (row < table->rowCount() && row >= 0) ? table->removeRow(row) : LogFile::writeToLog("BehaviorVariablesUI::removeVariableFromTable(): Invalid row to remove!!");
}

void BehaviorVariablesUI::loadVariable(CheckBox *variableWid){
    if (loadedData){
        disconnect(variableWid, 0, this, 0);
        auto index = table->currentRow();
        boolName->setText(loadedData->getVariableNameAt(index));
        variableWid->setChecked(loadedData->getWordVariableValueAt(index));
        hideOtherVariables(VARIABLE_TYPE_BOOL);
        connect(variableWid, SIGNAL(released()), this, SLOT(setVariableValue()), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::loadVariable(SpinBox *variableWid){
    if (loadedData){
        disconnect(variableWid, 0, this, 0);
        auto index = table->currentRow();
        intName->setText(loadedData->getVariableNameAt(index));
        variableWid->setValue(loadedData->getWordVariableValueAt(index));
        hideOtherVariables(VARIABLE_TYPE_INT32);
        connect(variableWid, SIGNAL(editingFinished()), this, SLOT(setVariableValue()), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::loadVariable(DoubleSpinBox *variableWid){
    if (loadedData){
        disconnect(variableWid, 0, this, 0);
        auto index = table->currentRow();
        doubleName->setText(loadedData->getVariableNameAt(index));
        variableWid->setValue(loadedData->getWordVariableValueAt(index));
        hideOtherVariables(VARIABLE_TYPE_REAL);
        connect(variableWid, SIGNAL(editingFinished()), this, SLOT(setVariableValue()), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::loadVariable(QuadVariableWidget *variableWid){
    if (loadedData){
        disconnect(variableWid, 0, this, 0);
        auto index = table->currentRow();
        quadName->setText(loadedData->getVariableNameAt(index));
        variableWid->setValue(loadedData->getQuadVariableValueAt(index));
        hideOtherVariables(VARIABLE_TYPE_VECTOR4);
        connect(variableWid, SIGNAL(editingFinished()), this, SLOT(setVariableValue()));
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::hideOtherVariables(int indexToView){
    for (auto i = 0; i < variableWidget->rowCount(); i++){
        (i == indexToView) ? variableWidget->setRowHidden(i, false) : variableWidget->setRowHidden(i, true);
    }
}

void BehaviorVariablesUI::viewVariable(int row, int column){
    if (column == 2){
        if (loadedData){
            auto item = table->item(row, 1);
            auto type = item->text();
            if (type == "BOOL"){
                loadVariable(boolCB);
            }else if (type == "INT32"){
                loadVariable(intSB);
            }else if (type == "REAL"){
                loadVariable(doubleSB);
            }else if (type == "VECTOR4"){
                loadVariable(quadWidget);
            }else if (type == "QUATERNION"){
                loadVariable(quadWidget);
            }else if (type == "POINTER"){
                boneWeightArrayWidget->loadData(loadedData->getVariantVariable(row));
                stackLyt->setCurrentIndex(BONE_WEIGHTS_WIDGET);
                return;
            }
            stackLyt->setCurrentIndex(VARIABLE_WIDGET);
        }else{
            LogFile::writeToLog("BehaviorVariablesUI::viewVariable(): loadedData is nullptr!!");
        }
    }
}

void BehaviorVariablesUI::returnToTable(){
    stackLyt->setCurrentIndex(TABLE_WIDGET);
}

void BehaviorVariablesUI::addVariableToTable(const QString & name, const QString & type){
    auto row = table->rowCount();
    table->setRowCount(row + 1);
    table->setItem(row, 0, new QTableWidgetItem(name));
    table->setItem(row, 1, new QTableWidgetItem(type));
    table->setItem(row, 2, new QTableWidgetItem("Edit"));
    (stackLyt->currentIndex() == VARIABLE_WIDGET) ? stackLyt->setCurrentIndex(TABLE_WIDGET) : NULL;
    table->setCurrentCell(row, 0);
    //emit variableAdded(name);
}

void BehaviorVariablesUI::loadData(HkxObject *data){
    if (data){
        if (data->getSignature() == HKB_BEHAVIOR_GRAPH_DATA){
            loadedData = static_cast<hkbBehaviorGraphData *>(data);
            auto header = table->horizontalHeader();
            header->setSectionResizeMode(QHeaderView::Stretch);
            auto variableNames = loadedData->getVariableNames();
            for (auto i = 0; i < variableNames.size(); i++){
                auto row = table->rowCount();
                if (table->rowCount() > i){
                    table->setRowHidden(i, false);
                    (table->item(row, 0)) ? table->item(row, 0)->setText(variableNames.at(i)) : table->setItem(row, 0, new QTableWidgetItem(variableNames.at(i)));
                }else{
                    table->setRowCount(row + 1);
                    table->setItem(row, 0, new QTableWidgetItem(variableNames.at(i)));
                    table->setItem(row, 1, new QTableWidgetItem(getVariableTypeAsString(loadedData->getVariableTypeAt(i)).section("_", -1, -1)));
                    table->setItem(row, 2, new QTableWidgetItem("Edit"));
                }
            }
            for (auto j = variableNames.size(); j < table->rowCount(); j++){
                table->setRowHidden(j, true);
            }
            header->setSectionResizeMode(QHeaderView::Interactive);
        }
    }else{
        LogFile::writeToLog("BehaviorVariablesUI::viewVariable(): data is nullptr!!");
    }
}

void BehaviorVariablesUI::clear(){
    returnToTable();
    for (auto i = table->rowCount() - 1; i >= 0; i--){
        table->removeRow(i);
    }
}

void BehaviorVariablesUI::addVariable(){
    if (loadedData){
        auto addvariable = [&](hkVariableType type, const QString & typeofvar){
            loadedData->addVariable(type);
            addVariableToTable(loadedData->getVariableNameAt(loadedData->getNumberOfVariables() - 1), typeofvar);
            emit variableAdded(loadedData->getVariableNameAt(loadedData->getNumberOfVariables() - 1), typeofvar);
        };
        auto type = typeSelector->currentIndex();
        switch (type){
        case VARIABLE_TYPE_BOOL:
            addvariable(hkVariableType::VARIABLE_TYPE_BOOL, "VARIABLE_TYPE_BOOL"); break;
        case VARIABLE_TYPE_INT32:
            addvariable(hkVariableType::VARIABLE_TYPE_INT32, "VARIABLE_TYPE_INT32"); break;
        case VARIABLE_TYPE_REAL:
            addvariable(hkVariableType::VARIABLE_TYPE_REAL, "VARIABLE_TYPE_REAL"); break;
        case VARIABLE_TYPE_POINTER:
            addvariable(hkVariableType::VARIABLE_TYPE_POINTER, "VARIABLE_TYPE_POINTER"); break;
        case VARIABLE_TYPE_VECTOR4:
            addvariable(hkVariableType::VARIABLE_TYPE_VECTOR4, "VARIABLE_TYPE_VECTOR4"); break;
        case VARIABLE_TYPE_QUATERNION:
            addvariable(hkVariableType::VARIABLE_TYPE_QUATERNION, "VARIABLE_TYPE_QUATERNION"); break;
        }
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::removeVariable(){
    if (loadedData){
        disconnect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeVariable()));
        auto index = table->currentRow();
        auto message = static_cast<BehaviorFile *>(loadedData->getParentFile())->isVariableReferenced(index);
        if (message == ""){
            loadedData->removeVariable(index);
            (index < table->rowCount()) ? table->removeRow(index) : NULL;
            (stackLyt->currentIndex() == VARIABLE_WIDGET) ? stackLyt->setCurrentIndex(TABLE_WIDGET) : NULL;
            static_cast<BehaviorFile *>(loadedData->getParentFile())->updateVariableIndices(index);
            emit variableRemoved(index);
            table->setFocus();
        }else{
            WARNING_MESSAGE(message);
        }
        connect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeVariable()), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BehaviorVariablesUI: loadedData is nullptr!!");
    }
}

void BehaviorVariablesUI::setHkDataUI(HkDataUI *ui){
    dataUI = ui;
}
