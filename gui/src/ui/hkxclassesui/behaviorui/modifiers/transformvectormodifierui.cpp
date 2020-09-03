#include "transformvectormodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbTransformVectorModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 10

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ROTATION_ROW 2
#define TRANSLATION_ROW 3
#define VECTOR_IN_ROW 4
#define VECTOR_OUT_ROW 5
#define ROTATE_ONLY_ROW 6
#define INVERSE_ROW 7
#define COMPUTE_ON_ACTIVATE_ROW 8
#define COMPUTE_ON_MODIFY_ROW 9

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList TransformVectorModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

TransformVectorModifierUI::TransformVectorModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      rotation(new QuadVariableWidget),
      translation(new QuadVariableWidget),
      vectorIn(new QuadVariableWidget),
      vectorOut(new QuadVariableWidget),
      rotateOnly(new CheckBox),
      inverse(new CheckBox),
      computeOnActivate(new CheckBox),
      computeOnModify(new CheckBox)
{
    setTitle("hkbTransformVectorModifier");
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
    table->setItem(ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("rotation"));
    table->setItem(ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROTATION_ROW, VALUE_COLUMN, rotation);
    table->setItem(TRANSLATION_ROW, NAME_COLUMN, new TableWidgetItem("translation"));
    table->setItem(TRANSLATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TRANSLATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TRANSLATION_ROW, VALUE_COLUMN, translation);
    table->setItem(VECTOR_IN_ROW, NAME_COLUMN, new TableWidgetItem("vectorIn"));
    table->setItem(VECTOR_IN_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(VECTOR_IN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VECTOR_IN_ROW, VALUE_COLUMN, vectorIn);
    table->setItem(VECTOR_OUT_ROW, NAME_COLUMN, new TableWidgetItem("vectorOut"));
    table->setItem(VECTOR_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(VECTOR_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VECTOR_OUT_ROW, VALUE_COLUMN, vectorOut);
    table->setItem(ROTATE_ONLY_ROW, NAME_COLUMN, new TableWidgetItem("rotateOnly"));
    table->setItem(ROTATE_ONLY_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ROTATE_ONLY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROTATE_ONLY_ROW, VALUE_COLUMN, rotateOnly);
    table->setItem(INVERSE_ROW, NAME_COLUMN, new TableWidgetItem("inverse"));
    table->setItem(INVERSE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERSE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERSE_ROW, VALUE_COLUMN, inverse);
    table->setItem(COMPUTE_ON_ACTIVATE_ROW, NAME_COLUMN, new TableWidgetItem("computeOnActivate"));
    table->setItem(COMPUTE_ON_ACTIVATE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(COMPUTE_ON_ACTIVATE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(COMPUTE_ON_ACTIVATE_ROW, VALUE_COLUMN, computeOnActivate);
    table->setItem(COMPUTE_ON_MODIFY_ROW, NAME_COLUMN, new TableWidgetItem("computeOnModify"));
    table->setItem(COMPUTE_ON_MODIFY_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(COMPUTE_ON_MODIFY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(COMPUTE_ON_MODIFY_ROW, VALUE_COLUMN, computeOnModify);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void TransformVectorModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(rotation, SIGNAL(editingFinished()), this, SLOT(setRotation()), Qt::UniqueConnection);
        connect(translation, SIGNAL(editingFinished()), this, SLOT(setTranslation()), Qt::UniqueConnection);
        connect(vectorIn, SIGNAL(editingFinished()), this, SLOT(setVectorIn()), Qt::UniqueConnection);
        connect(vectorOut, SIGNAL(editingFinished()), this, SLOT(setVectorOut()), Qt::UniqueConnection);
        connect(rotateOnly, SIGNAL(released()), this, SLOT(setRotateOnly()), Qt::UniqueConnection);
        connect(inverse, SIGNAL(released()), this, SLOT(setInverse()), Qt::UniqueConnection);
        connect(computeOnActivate, SIGNAL(released()), this, SLOT(setComputeOnActivate()), Qt::UniqueConnection);
        connect(computeOnModify, SIGNAL(released()), this, SLOT(setComputeOnModify()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(rotation, SIGNAL(editingFinished()), this, SLOT(setRotation()));
        disconnect(translation, SIGNAL(editingFinished()), this, SLOT(setTranslation()));
        disconnect(vectorIn, SIGNAL(editingFinished()), this, SLOT(setVectorIn()));
        disconnect(vectorOut, SIGNAL(editingFinished()), this, SLOT(setVectorOut()));
        disconnect(rotateOnly, SIGNAL(released()), this, SLOT(setRotateOnly()));
        disconnect(inverse, SIGNAL(released()), this, SLOT(setInverse()));
        disconnect(computeOnActivate, SIGNAL(released()), this, SLOT(setComputeOnActivate()));
        disconnect(computeOnModify, SIGNAL(released()), this, SLOT(setComputeOnModify()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void TransformVectorModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("TransformVectorModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void TransformVectorModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_TRANSFORM_VECTOR_MODIFIER){
            bsData = static_cast<hkbTransformVectorModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            rotation->setValue(bsData->getRotation());
            translation->setValue(bsData->getTranslation());
            vectorIn->setValue(bsData->getVectorIn());
            vectorOut->setValue(bsData->getVectorOut());
            rotateOnly->setChecked(bsData->getRotateOnly());
            inverse->setChecked(bsData->getInverse());
            computeOnActivate->setChecked(bsData->getComputeOnActivate());
            computeOnModify->setChecked(bsData->getComputeOnModify());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ROTATION_ROW, BINDING_COLUMN, varBind, "rotation", table, bsData);
            UIHelper::loadBinding(TRANSLATION_ROW, BINDING_COLUMN, varBind, "translation", table, bsData);
            UIHelper::loadBinding(VECTOR_IN_ROW, BINDING_COLUMN, varBind, "vectorIn", table, bsData);
            UIHelper::loadBinding(VECTOR_OUT_ROW, BINDING_COLUMN, varBind, "vectorOut", table, bsData);
            UIHelper::loadBinding(ROTATE_ONLY_ROW, BINDING_COLUMN, varBind, "rotateOnly", table, bsData);
            UIHelper::loadBinding(INVERSE_ROW, BINDING_COLUMN, varBind, "inverse", table, bsData);
            UIHelper::loadBinding(COMPUTE_ON_ACTIVATE_ROW, BINDING_COLUMN, varBind, "computeOnActivate", table, bsData);
            UIHelper::loadBinding(COMPUTE_ON_MODIFY_ROW, BINDING_COLUMN, varBind, "computeOnModify", table, bsData);
        }else{
            LogFile::writeToLog("TransformVectorModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("TransformVectorModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void TransformVectorModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("TransformVectorModifierUI::setName(): The data is nullptr!!");
    }
}

void TransformVectorModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("TransformVectorModifierUI::setEnable(): The data is nullptr!!");
}

void TransformVectorModifierUI::setRotation(){
    (bsData) ? bsData->setRotation(rotation->value()) : LogFile::writeToLog("TransformVectorModifierUI::setRotation(): The data is nullptr!!");
}

void TransformVectorModifierUI::setTranslation(){
    (bsData) ? bsData->setTranslation(translation->value()) : LogFile::writeToLog("TransformVectorModifierUI::setTranslation(): The data is nullptr!!");
}

void TransformVectorModifierUI::setVectorIn(){
    (bsData) ? bsData->setVectorIn(vectorIn->value()) : LogFile::writeToLog("TransformVectorModifierUI::setVectorIn(): The data is nullptr!!");
}

void TransformVectorModifierUI::setVectorOut(){
    (bsData) ? bsData->setVectorOut(vectorOut->value()) : LogFile::writeToLog("TransformVectorModifierUI::setVectorOut(): The data is nullptr!!");
}

void TransformVectorModifierUI::setRotateOnly(){
    (bsData) ? bsData->setRotateOnly(rotateOnly->isChecked()) : LogFile::writeToLog("TransformVectorModifierUI::setRotateOnly(): The data is nullptr!!");
}

void TransformVectorModifierUI::setInverse(){
    (bsData) ? bsData->setInverse(inverse->isChecked()) : LogFile::writeToLog("TransformVectorModifierUI::setInverse(): The data is nullptr!!");
}

void TransformVectorModifierUI::setComputeOnActivate(){
    (bsData) ? bsData->setComputeOnActivate(computeOnActivate->isChecked()) : LogFile::writeToLog("TransformVectorModifierUI::setComputeOnActivate(): The data is nullptr!!");
}

void TransformVectorModifierUI::setComputeOnModify(){
    (bsData) ? bsData->setComputeOnModify(computeOnModify->isChecked()) : LogFile::writeToLog("TransformVectorModifierUI::setComputeOnModify(): The data is nullptr!!");
}

void TransformVectorModifierUI::viewSelected(int row, int column){
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
            case ROTATION_ROW:
                checkisproperty(ROTATION_ROW, "rotation"); break;
            case TRANSLATION_ROW:
                checkisproperty(TRANSLATION_ROW, "translation"); break;
            case VECTOR_IN_ROW:
                checkisproperty(VECTOR_IN_ROW, "vectorIn"); break;
            case VECTOR_OUT_ROW:
                checkisproperty(VECTOR_OUT_ROW, "vectorOut"); break;
            case ROTATE_ONLY_ROW:
                checkisproperty(ROTATE_ONLY_ROW, "rotateOnly"); break;
            case INVERSE_ROW:
                checkisproperty(INVERSE_ROW, "inverse"); break;
            case COMPUTE_ON_ACTIVATE_ROW:
                checkisproperty(COMPUTE_ON_ACTIVATE_ROW, "computeOnActivate"); break;
            case COMPUTE_ON_MODIFY_ROW:
                checkisproperty(COMPUTE_ON_MODIFY_ROW, "computeOnModify"); break;
            }
        }
    }else{
        LogFile::writeToLog("TransformVectorModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void TransformVectorModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("TransformVectorModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void TransformVectorModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("rotation", ROTATION_ROW);
            setname("translation", TRANSLATION_ROW);
            setname("vectorIn", VECTOR_IN_ROW);
            setname("vectorOut", VECTOR_OUT_ROW);
            setname("rotateOnly", ROTATE_ONLY_ROW);
            setname("inverse", INVERSE_ROW);
            setname("computeOnActivate", COMPUTE_ON_ACTIVATE_ROW);
            setname("computeOnModify", COMPUTE_ON_MODIFY_ROW);
        }
    }else{
        LogFile::writeToLog("TransformVectorModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void TransformVectorModifierUI::setBindingVariable(int index, const QString &name){
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
        case ROTATION_ROW:
            checkisproperty(ROTATION_ROW, "rotation", VARIABLE_TYPE_QUATERNION); break;
        case TRANSLATION_ROW:
            checkisproperty(TRANSLATION_ROW, "translation", VARIABLE_TYPE_VECTOR4); break;
        case VECTOR_IN_ROW:
            checkisproperty(VECTOR_IN_ROW, "vectorIn", VARIABLE_TYPE_VECTOR4); break;
        case VECTOR_OUT_ROW:
            checkisproperty(VECTOR_OUT_ROW, "vectorOut", VARIABLE_TYPE_VECTOR4); break;
        case ROTATE_ONLY_ROW:
            checkisproperty(ROTATE_ONLY_ROW, "rotateOnly", VARIABLE_TYPE_BOOL); break;
        case INVERSE_ROW:
            checkisproperty(INVERSE_ROW, "inverse", VARIABLE_TYPE_BOOL); break;
        case COMPUTE_ON_ACTIVATE_ROW:
            checkisproperty(COMPUTE_ON_ACTIVATE_ROW, "computeOnActivate", VARIABLE_TYPE_BOOL); break;
        case COMPUTE_ON_MODIFY_ROW:
            checkisproperty(COMPUTE_ON_MODIFY_ROW, "computeOnModify", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("TransformVectorModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
