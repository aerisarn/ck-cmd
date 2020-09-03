#include "dampingmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbDampingModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 15

#define NAME_ROW 0
#define ENABLE_ROW 1
#define KP_ROW 2
#define KI_ROW 3
#define KD_ROW 4
#define ENABLE_SCALAR_DAMPING_ROW 5
#define ENABLE_VECTOR_DAMPING_ROW 6
#define RAW_VALUE_ROW 7
#define DAMPED_VALUE_ROW 8
#define RAW_VECTOR_ROW 9
#define DAMPED_VECTOR_ROW 10
#define VEC_ERROR_SUM_ROW 11
#define VEC_PREVIOUS_ERROR_ROW 12
#define ERROR_SUM_ROW 13
#define PREVIOUS_ERROR_ROW 14

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList DampingModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

DampingModifierUI::DampingModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      kP(new DoubleSpinBox),
      kI(new DoubleSpinBox),
      kD(new DoubleSpinBox),
      enableScalarDamping(new CheckBox),
      enableVectorDamping(new CheckBox),
      rawValue(new DoubleSpinBox),
      dampedValue(new DoubleSpinBox),
      rawVector(new QuadVariableWidget),
      dampedVector(new QuadVariableWidget),
      vecErrorSum(new QuadVariableWidget),
      vecPreviousError(new QuadVariableWidget),
      errorSum(new DoubleSpinBox),
      previousError(new DoubleSpinBox)
{
    setTitle("hkbDampingModifier");
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
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(KP_ROW, NAME_COLUMN, new TableWidgetItem("kP"));
    table->setItem(KP_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(KP_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(KP_ROW, VALUE_COLUMN, kP);
    table->setItem(KI_ROW, NAME_COLUMN, new TableWidgetItem("kI"));
    table->setItem(KI_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(KI_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(KI_ROW, VALUE_COLUMN, kI);
    table->setItem(KD_ROW, NAME_COLUMN, new TableWidgetItem("kD"));
    table->setItem(KD_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(KD_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(KD_ROW, VALUE_COLUMN, kD);
    table->setItem(ENABLE_SCALAR_DAMPING_ROW, NAME_COLUMN, new TableWidgetItem("enableScalarDamping"));
    table->setItem(ENABLE_SCALAR_DAMPING_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_SCALAR_DAMPING_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_SCALAR_DAMPING_ROW, VALUE_COLUMN, enableScalarDamping);
    table->setItem(ENABLE_VECTOR_DAMPING_ROW, NAME_COLUMN, new TableWidgetItem("enableVectorDamping"));
    table->setItem(ENABLE_VECTOR_DAMPING_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_VECTOR_DAMPING_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_VECTOR_DAMPING_ROW, VALUE_COLUMN, enableVectorDamping);
    table->setItem(RAW_VALUE_ROW, NAME_COLUMN, new TableWidgetItem("rawValue"));
    table->setItem(RAW_VALUE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(RAW_VALUE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RAW_VALUE_ROW, VALUE_COLUMN, rawValue);
    table->setItem(DAMPED_VALUE_ROW, NAME_COLUMN, new TableWidgetItem("dampedValue"));
    table->setItem(DAMPED_VALUE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DAMPED_VALUE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DAMPED_VALUE_ROW, VALUE_COLUMN, dampedValue);
    table->setItem(RAW_VECTOR_ROW, NAME_COLUMN, new TableWidgetItem("rawVector"));
    table->setItem(RAW_VECTOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(RAW_VECTOR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RAW_VECTOR_ROW, VALUE_COLUMN, rawVector);
    table->setItem(DAMPED_VECTOR_ROW, NAME_COLUMN, new TableWidgetItem("dampedVector"));
    table->setItem(DAMPED_VECTOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(DAMPED_VECTOR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DAMPED_VECTOR_ROW, VALUE_COLUMN, dampedVector);
    table->setItem(VEC_ERROR_SUM_ROW, NAME_COLUMN, new TableWidgetItem("vecErrorSum"));
    table->setItem(VEC_ERROR_SUM_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(VEC_ERROR_SUM_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VEC_ERROR_SUM_ROW, VALUE_COLUMN, vecErrorSum);
    table->setItem(VEC_PREVIOUS_ERROR_ROW, NAME_COLUMN, new TableWidgetItem("vecPreviousError"));
    table->setItem(VEC_PREVIOUS_ERROR_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(VEC_PREVIOUS_ERROR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VEC_PREVIOUS_ERROR_ROW, VALUE_COLUMN, vecPreviousError);
    table->setItem(ERROR_SUM_ROW, NAME_COLUMN, new TableWidgetItem("errorSum"));
    table->setItem(ERROR_SUM_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ERROR_SUM_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ERROR_SUM_ROW, VALUE_COLUMN, errorSum);
    table->setItem(PREVIOUS_ERROR_ROW, NAME_COLUMN, new TableWidgetItem("previousError"));
    table->setItem(PREVIOUS_ERROR_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(PREVIOUS_ERROR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(PREVIOUS_ERROR_ROW, VALUE_COLUMN, previousError);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void DampingModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(kP, SIGNAL(editingFinished()), this, SLOT(setKP()), Qt::UniqueConnection);
        connect(kI, SIGNAL(editingFinished()), this, SLOT(setKI()), Qt::UniqueConnection);
        connect(kD, SIGNAL(editingFinished()), this, SLOT(setKD()), Qt::UniqueConnection);
        connect(enableScalarDamping, SIGNAL(released()), this, SLOT(setEnableScalarDamping()), Qt::UniqueConnection);
        connect(enableVectorDamping, SIGNAL(released()), this, SLOT(setEnableVectorDamping()), Qt::UniqueConnection);
        connect(rawValue, SIGNAL(editingFinished()), this, SLOT(setRawValue()), Qt::UniqueConnection);
        connect(dampedValue, SIGNAL(editingFinished()), this, SLOT(setDampedValue()), Qt::UniqueConnection);
        connect(rawVector, SIGNAL(editingFinished()), this, SLOT(setRawVector()), Qt::UniqueConnection);
        connect(dampedVector, SIGNAL(editingFinished()), this, SLOT(setDampedVector()), Qt::UniqueConnection);
        connect(vecErrorSum, SIGNAL(editingFinished()), this, SLOT(setVecErrorSum()), Qt::UniqueConnection);
        connect(vecPreviousError, SIGNAL(editingFinished()), this, SLOT(setVecPreviousError()), Qt::UniqueConnection);
        connect(errorSum, SIGNAL(editingFinished()), this, SLOT(setErrorSum()), Qt::UniqueConnection);
        connect(previousError, SIGNAL(editingFinished()), this, SLOT(setPreviousError()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(kP, SIGNAL(editingFinished()), this, SLOT(setKP()));
        disconnect(kI, SIGNAL(editingFinished()), this, SLOT(setKI()));
        disconnect(kD, SIGNAL(editingFinished()), this, SLOT(setKD()));
        disconnect(enableScalarDamping, SIGNAL(released()), this, SLOT(setEnableScalarDamping()));
        disconnect(enableVectorDamping, SIGNAL(released()), this, SLOT(setEnableVectorDamping()));
        disconnect(rawValue, SIGNAL(editingFinished()), this, SLOT(setRawValue()));
        disconnect(dampedValue, SIGNAL(editingFinished()), this, SLOT(setDampedValue()));
        disconnect(rawVector, SIGNAL(editingFinished()), this, SLOT(setRawVector()));
        disconnect(dampedVector, SIGNAL(editingFinished()), this, SLOT(setDampedVector()));
        disconnect(vecErrorSum, SIGNAL(editingFinished()), this, SLOT(setVecErrorSum()));
        disconnect(vecPreviousError, SIGNAL(editingFinished()), this, SLOT(setVecPreviousError()));
        disconnect(errorSum, SIGNAL(editingFinished()), this, SLOT(setErrorSum()));
        disconnect(previousError, SIGNAL(editingFinished()), this, SLOT(setPreviousError()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void DampingModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("DampingModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void DampingModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_DAMPING_MODIFIER){
            bsData = static_cast<hkbDampingModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            kP->setValue(bsData->getKP());
            kI->setValue(bsData->getKI());
            kD->setValue(bsData->getKD());
            enableScalarDamping->setChecked(bsData->getEnableScalarDamping());
            enableVectorDamping->setChecked(bsData->getEnableVectorDamping());
            rawValue->setValue(bsData->getRawValue());
            dampedValue->setValue(bsData->getDampedValue());
            rawVector->setValue(bsData->getRawVector());
            dampedVector->setValue(bsData->getDampedVector());
            vecErrorSum->setValue(bsData->getVecErrorSum());
            vecPreviousError->setValue(bsData->getVecPreviousError());
            errorSum->setValue(bsData->getErrorSum());
            previousError->setValue(bsData->getPreviousError());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(KP_ROW, BINDING_COLUMN, varBind, "kP", table, bsData);
            UIHelper::loadBinding(KI_ROW, BINDING_COLUMN, varBind, "kI", table, bsData);
            UIHelper::loadBinding(KD_ROW, BINDING_COLUMN, varBind, "kD", table, bsData);
            UIHelper::loadBinding(ENABLE_SCALAR_DAMPING_ROW, BINDING_COLUMN, varBind, "enableScalarDamping", table, bsData);
            UIHelper::loadBinding(ENABLE_VECTOR_DAMPING_ROW, BINDING_COLUMN, varBind, "enableVectorDamping", table, bsData);
            UIHelper::loadBinding(RAW_VALUE_ROW, BINDING_COLUMN, varBind, "rawValue", table, bsData);
            UIHelper::loadBinding(DAMPED_VALUE_ROW, BINDING_COLUMN, varBind, "dampedValue", table, bsData);
            UIHelper::loadBinding(RAW_VECTOR_ROW, BINDING_COLUMN, varBind, "rawVector", table, bsData);
            UIHelper::loadBinding(DAMPED_VECTOR_ROW, BINDING_COLUMN, varBind, "dampedVector", table, bsData);
            UIHelper::loadBinding(VEC_ERROR_SUM_ROW, BINDING_COLUMN, varBind, "vecErrorSum", table, bsData);
            UIHelper::loadBinding(VEC_PREVIOUS_ERROR_ROW, BINDING_COLUMN, varBind, "vecPreviousError", table, bsData);
            UIHelper::loadBinding(ERROR_SUM_ROW, BINDING_COLUMN, varBind, "errorSum", table, bsData);
            UIHelper::loadBinding(PREVIOUS_ERROR_ROW, BINDING_COLUMN, varBind, "previousError", table, bsData);
        }else{
            LogFile::writeToLog("DampingModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("DampingModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void DampingModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("DampingModifierUI::setName(): The data is nullptr!!");
    }
}

void DampingModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("DampingModifierUI::setEnable(): The data is nullptr!!");
}

void DampingModifierUI::setKP(){
    (bsData) ? bsData->setKP(kP->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setKP(): The data is nullptr!!");
}

void DampingModifierUI::setKI(){
    (bsData) ? bsData->setKI(kI->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setKI(): The data is nullptr!!");
}

void DampingModifierUI::setKD(){
    (bsData) ? bsData->setKD(kD->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setKD(): The data is nullptr!!");
}

void DampingModifierUI::setEnableScalarDamping(){
    (bsData) ? bsData->setEnableScalarDamping(enableScalarDamping->isChecked()) : LogFile::writeToLog("DampingModifierUI::setEnableScalarDamping(): The data is nullptr!!");
}

void DampingModifierUI::setEnableVectorDamping(){
    (bsData) ? bsData->setEnableVectorDamping(enableVectorDamping->isChecked()) : LogFile::writeToLog("DampingModifierUI::setEnableVectorDamping(): The data is nullptr!!");
}

void DampingModifierUI::setRawValue(){
    (bsData) ? bsData->setRawValue(rawValue->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setRawValue(): The data is nullptr!!");
}

void DampingModifierUI::setDampedValue(){
    (bsData) ? bsData->setDampedValue(dampedValue->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setDampedValue(): The data is nullptr!!");
}

void DampingModifierUI::setRawVector(){
    (bsData) ? bsData->setRawVector(rawVector->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setRawVector(): The data is nullptr!!");
}

void DampingModifierUI::setDampedVector(){
    (bsData) ? bsData->setDampedVector(dampedVector->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setDampedVector(): The data is nullptr!!");
}

void DampingModifierUI::setVecErrorSum(){
    (bsData) ? bsData->setVecErrorSum(vecErrorSum->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setVecErrorSum(): The data is nullptr!!");
}

void DampingModifierUI::setVecPreviousError(){
    (bsData) ? bsData->setVecPreviousError(vecPreviousError->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setVecPreviousError(): The data is nullptr!!");
}

void DampingModifierUI::setErrorSum(){
    (bsData) ? bsData->setErrorSum(errorSum->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setErrorSum(): The data is nullptr!!");
}

void DampingModifierUI::setPreviousError(){
    (bsData) ? bsData->setPreviousError(previousError->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setPreviousError(): The data is nullptr!!");
}

void DampingModifierUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case ENABLE_ROW:
                checkisproperty(ENABLE_ROW, "enable"); break;
            case KP_ROW:
                checkisproperty(KP_ROW, "kP"); break;
            case KI_ROW:
                checkisproperty(KI_ROW, "kI"); break;
            case KD_ROW:
                checkisproperty(KD_ROW, "kD"); break;
            case ENABLE_SCALAR_DAMPING_ROW:
                checkisproperty(ENABLE_SCALAR_DAMPING_ROW, "enableScalarDamping"); break;
            case ENABLE_VECTOR_DAMPING_ROW:
                checkisproperty(ENABLE_VECTOR_DAMPING_ROW, "enableVectorDamping"); break;
            case RAW_VALUE_ROW:
                checkisproperty(RAW_VALUE_ROW, "rawValue"); break;
            case DAMPED_VALUE_ROW:
                checkisproperty(DAMPED_VALUE_ROW, "dampedValue"); break;
            case RAW_VECTOR_ROW:
                checkisproperty(RAW_VECTOR_ROW, "rawVector"); break;
            case DAMPED_VECTOR_ROW:
                checkisproperty(DAMPED_VECTOR_ROW, "dampedVector"); break;
            case VEC_ERROR_SUM_ROW:
                checkisproperty(VEC_ERROR_SUM_ROW, "vecErrorSum"); break;
            case VEC_PREVIOUS_ERROR_ROW:
                checkisproperty(VEC_PREVIOUS_ERROR_ROW, "vecPreviousError"); break;
            case ERROR_SUM_ROW:
                checkisproperty(ERROR_SUM_ROW, "errorSum"); break;
            case PREVIOUS_ERROR_ROW:
                checkisproperty(PREVIOUS_ERROR_ROW, "previousError"); break;
            }
        }
    }else{
        LogFile::writeToLog("DampingModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void DampingModifierUI::selectTableToView(bool viewisProperty, const QString & path){
    if (bsData){
        if (viewisProperty){
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
        LogFile::writeToLog("DampingModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void DampingModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("kP", KP_ROW);
            setname("kI", KI_ROW);
            setname("kD", KD_ROW);
            setname("enableScalarDamping", ENABLE_SCALAR_DAMPING_ROW);
            setname("enableVectorDamping", ENABLE_VECTOR_DAMPING_ROW);
            setname("rawValue", RAW_VALUE_ROW);
            setname("dampedValue", DAMPED_VALUE_ROW);
            setname("rawVector", RAW_VECTOR_ROW);
            setname("dampedVector", DAMPED_VECTOR_ROW);
            setname("vecErrorSum", VEC_ERROR_SUM_ROW);
            setname("vecPreviousError", VEC_PREVIOUS_ERROR_ROW);
            setname("errorSum", ERROR_SUM_ROW);
            setname("previousError", PREVIOUS_ERROR_ROW);
        }
    }else{
        LogFile::writeToLog("DampingModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void DampingModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case ENABLE_ROW:
            checkisproperty(ENABLE_ROW, "enable", VARIABLE_TYPE_BOOL); break;
        case KP_ROW:
            checkisproperty(KP_ROW, "kP", VARIABLE_TYPE_REAL); break;
        case KI_ROW:
            checkisproperty(KI_ROW, "kI", VARIABLE_TYPE_REAL); break;
        case KD_ROW:
            checkisproperty(KD_ROW, "kD", VARIABLE_TYPE_REAL); break;
        case ENABLE_SCALAR_DAMPING_ROW:
            checkisproperty(ENABLE_SCALAR_DAMPING_ROW, "enableScalarDamping", VARIABLE_TYPE_BOOL); break;
        case ENABLE_VECTOR_DAMPING_ROW:
            checkisproperty(ENABLE_VECTOR_DAMPING_ROW, "enableVectorDamping", VARIABLE_TYPE_BOOL); break;
        case RAW_VALUE_ROW:
            checkisproperty(RAW_VALUE_ROW, "rawValue", VARIABLE_TYPE_REAL); break;
        case DAMPED_VALUE_ROW:
            checkisproperty(DAMPED_VALUE_ROW, "dampedValue", VARIABLE_TYPE_REAL); break;
        case RAW_VECTOR_ROW:
            checkisproperty(RAW_VECTOR_ROW, "rawVector", VARIABLE_TYPE_VECTOR4); break;
        case DAMPED_VECTOR_ROW:
            checkisproperty(DAMPED_VECTOR_ROW, "dampedVector", VARIABLE_TYPE_VECTOR4); break;
        case VEC_ERROR_SUM_ROW:
            checkisproperty(VEC_ERROR_SUM_ROW, "vecErrorSum", VARIABLE_TYPE_VECTOR4); break;
        case VEC_PREVIOUS_ERROR_ROW:
            checkisproperty(VEC_PREVIOUS_ERROR_ROW, "vecPreviousError", VARIABLE_TYPE_VECTOR4); break;
        case ERROR_SUM_ROW:
            checkisproperty(ERROR_SUM_ROW, "errorSum", VARIABLE_TYPE_REAL); break;
        case PREVIOUS_ERROR_ROW:
            checkisproperty(PREVIOUS_ERROR_ROW, "previousError", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("DampingModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
