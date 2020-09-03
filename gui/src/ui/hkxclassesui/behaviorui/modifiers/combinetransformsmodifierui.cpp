#include "combinetransformsmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbCombineTransformsModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 11

#define NAME_ROW 0
#define ENABLE_ROW 1
#define TRANSLATION_OUT_ROW 2
#define ROTATION_OUT_ROW 3
#define LEFT_TRANSLATION_ROW 4
#define LEFT_ROTATION_ROW 5
#define RIGHT_TRANSLATION_ROW 6
#define RIGHT_ROTATION_ROW 7
#define INVERT_LEFT_TRANSFORM_ROW 8
#define INVERT_RIGHT_TRANSFORM_ROW 9
#define INVERT_RESULT_ROW 10

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList CombineTransformsModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

CombineTransformsModifierUI::CombineTransformsModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      translationOut(new QuadVariableWidget),
      rotationOut(new QuadVariableWidget),
      leftTranslation(new QuadVariableWidget),
      leftRotation(new QuadVariableWidget),
      rightTranslation(new QuadVariableWidget),
      rightRotation(new QuadVariableWidget),
      invertLeftTransform(new CheckBox),
      invertRightTransform(new CheckBox),
      invertResult(new CheckBox)
{
    setTitle("hkbCombineTransformsModifier");
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
    table->setItem(TRANSLATION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("translationOut"));
    table->setItem(TRANSLATION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TRANSLATION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TRANSLATION_OUT_ROW, VALUE_COLUMN, translationOut);
    table->setItem(ROTATION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("rotationOut"));
    table->setItem(ROTATION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(ROTATION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROTATION_OUT_ROW, VALUE_COLUMN, rotationOut);
    table->setItem(LEFT_TRANSLATION_ROW, NAME_COLUMN, new TableWidgetItem("leftTranslation"));
    table->setItem(LEFT_TRANSLATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(LEFT_TRANSLATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LEFT_TRANSLATION_ROW, VALUE_COLUMN, leftTranslation);
    table->setItem(LEFT_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("leftRotation"));
    table->setItem(LEFT_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(LEFT_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LEFT_ROTATION_ROW, VALUE_COLUMN, leftRotation);
    table->setItem(RIGHT_TRANSLATION_ROW, NAME_COLUMN, new TableWidgetItem("rightTranslation"));
    table->setItem(RIGHT_TRANSLATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(RIGHT_TRANSLATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RIGHT_TRANSLATION_ROW, VALUE_COLUMN, rightTranslation);
    table->setItem(RIGHT_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("rightRotation"));
    table->setItem(RIGHT_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(RIGHT_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RIGHT_ROTATION_ROW, VALUE_COLUMN, rightRotation);
    table->setItem(INVERT_LEFT_TRANSFORM_ROW, NAME_COLUMN, new TableWidgetItem("invertLeftTransform"));
    table->setItem(INVERT_LEFT_TRANSFORM_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(INVERT_LEFT_TRANSFORM_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_LEFT_TRANSFORM_ROW, VALUE_COLUMN, invertLeftTransform);
    table->setItem(INVERT_RIGHT_TRANSFORM_ROW, NAME_COLUMN, new TableWidgetItem("invertRightTransform"));
    table->setItem(INVERT_RIGHT_TRANSFORM_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_RIGHT_TRANSFORM_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_RIGHT_TRANSFORM_ROW, VALUE_COLUMN, invertRightTransform);
    table->setItem(INVERT_RESULT_ROW, NAME_COLUMN, new TableWidgetItem("invertResult"));
    table->setItem(INVERT_RESULT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INVERT_RESULT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INVERT_RESULT_ROW, VALUE_COLUMN, invertResult);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void CombineTransformsModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(translationOut, SIGNAL(editingFinished()), this, SLOT(setTranslationOut()), Qt::UniqueConnection);
        connect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setRotationOut()), Qt::UniqueConnection);
        connect(leftTranslation, SIGNAL(editingFinished()), this, SLOT(setLeftTranslation()), Qt::UniqueConnection);
        connect(leftRotation, SIGNAL(editingFinished()), this, SLOT(setLeftRotation()), Qt::UniqueConnection);
        connect(rightTranslation, SIGNAL(editingFinished()), this, SLOT(setRightTranslation()), Qt::UniqueConnection);
        connect(rightRotation, SIGNAL(editingFinished()), this, SLOT(setRightRotation()), Qt::UniqueConnection);
        connect(invertLeftTransform, SIGNAL(released()), this, SLOT(setInvertLeftTransform()), Qt::UniqueConnection);
        connect(invertRightTransform, SIGNAL(released()), this, SLOT(setInvertRightTransform()), Qt::UniqueConnection);
        connect(invertResult, SIGNAL(released()), this, SLOT(setInvertResult()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(translationOut, SIGNAL(editingFinished()), this, SLOT(setTranslationOut()));
        disconnect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setRotationOut()));
        disconnect(leftTranslation, SIGNAL(editingFinished()), this, SLOT(setLeftTranslation()));
        disconnect(leftRotation, SIGNAL(editingFinished()), this, SLOT(setLeftRotation()));
        disconnect(rightTranslation, SIGNAL(editingFinished()), this, SLOT(setRightTranslation()));
        disconnect(rightRotation, SIGNAL(editingFinished()), this, SLOT(setRightRotation()));
        disconnect(invertLeftTransform, SIGNAL(released()), this, SLOT(setInvertLeftTransform()));
        disconnect(invertRightTransform, SIGNAL(released()), this, SLOT(setInvertRightTransform()));
        disconnect(invertResult, SIGNAL(released()), this, SLOT(setInvertResult()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void CombineTransformsModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("CombineTransformsModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void CombineTransformsModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_COMBINE_TRANSFORMS_MODIFIER){
            bsData = static_cast<hkbCombineTransformsModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            translationOut->setValue(bsData->getTranslationOut());
            rotationOut->setValue(bsData->getRotationOut());
            leftTranslation->setValue(bsData->getLeftTranslation());
            leftRotation->setValue(bsData->getLeftRotation());
            rightTranslation->setValue(bsData->getRightTranslation());
            rightRotation->setValue(bsData->getRightRotation());
            invertLeftTransform->setChecked(bsData->getInvertLeftTransform());
            invertRightTransform->setChecked(bsData->getInvertRightTransform());
            invertResult->setChecked(bsData->getInvertResult());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(TRANSLATION_OUT_ROW, BINDING_COLUMN, varBind, "translationOut", table, bsData);
            UIHelper::loadBinding(ROTATION_OUT_ROW, BINDING_COLUMN, varBind, "rotationOut", table, bsData);
            UIHelper::loadBinding(LEFT_TRANSLATION_ROW, BINDING_COLUMN, varBind, "leftTranslation", table, bsData);
            UIHelper::loadBinding(LEFT_ROTATION_ROW, BINDING_COLUMN, varBind, "leftRotation", table, bsData);
            UIHelper::loadBinding(RIGHT_TRANSLATION_ROW, BINDING_COLUMN, varBind, "rightTranslation", table, bsData);
            UIHelper::loadBinding(RIGHT_ROTATION_ROW, BINDING_COLUMN, varBind, "rightRotation", table, bsData);
            UIHelper::loadBinding(INVERT_LEFT_TRANSFORM_ROW, BINDING_COLUMN, varBind, "invertLeftTransform", table, bsData);
            UIHelper::loadBinding(INVERT_RIGHT_TRANSFORM_ROW, BINDING_COLUMN, varBind, "invertRightTransform", table, bsData);
            UIHelper::loadBinding(INVERT_RESULT_ROW, BINDING_COLUMN, varBind, "invertResult", table, bsData);
        }else{
            LogFile::writeToLog("CombineTransformsModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("CombineTransformsModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void CombineTransformsModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("CombineTransformsModifierUI::setName(): The data is nullptr!!");
    }
}

void CombineTransformsModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("CombineTransformsModifierUI::setEnable(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setTranslationOut(){
    (bsData) ? bsData->setTranslationOut(translationOut->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setTranslationOut(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setRotationOut(){
    (bsData) ? bsData->setRotationOut(rotationOut->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setRotationOut(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setLeftTranslation(){
    (bsData) ? bsData->setLeftTranslation(leftTranslation->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setLeftTranslation(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setLeftRotation(){
    (bsData) ? bsData->setLeftRotation(leftRotation->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setLeftRotation(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setRightTranslation(){
    (bsData) ? bsData->setRightTranslation(rightTranslation->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setRightTranslation(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setRightRotation(){
    (bsData) ? bsData->setRightRotation(rightRotation->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setRightRotation(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setInvertLeftTransform(){
    (bsData) ? bsData->setInvertLeftTransform(invertLeftTransform->isChecked()) : LogFile::writeToLog("CombineTransformsModifierUI::setInvertLeftTransform(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setInvertRightTransform(){
    (bsData) ? bsData->setInvertRightTransform(invertRightTransform->isChecked()) : LogFile::writeToLog("CombineTransformsModifierUI::setInvertRightTransform(): The data is nullptr!!");
}

void CombineTransformsModifierUI::setInvertResult(){
    (bsData) ? bsData->setInvertResult(invertResult->isChecked()) : LogFile::writeToLog("CombineTransformsModifierUI::setInvertResult(): The data is nullptr!!");
}

void CombineTransformsModifierUI::viewSelected(int row, int column){
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
            case TRANSLATION_OUT_ROW:
                checkisproperty(TRANSLATION_OUT_ROW, "translationOut"); break;
            case ROTATION_OUT_ROW:
                checkisproperty(ROTATION_OUT_ROW, "rotationOut"); break;
            case LEFT_TRANSLATION_ROW:
                checkisproperty(LEFT_TRANSLATION_ROW, "leftTranslation"); break;
            case LEFT_ROTATION_ROW:
                checkisproperty(LEFT_ROTATION_ROW, "leftRotation"); break;
            case RIGHT_TRANSLATION_ROW:
                checkisproperty(RIGHT_TRANSLATION_ROW, "rightTranslation"); break;
            case RIGHT_ROTATION_ROW:
                checkisproperty(RIGHT_ROTATION_ROW, "rightRotation"); break;
            case INVERT_LEFT_TRANSFORM_ROW:
                checkisproperty(INVERT_LEFT_TRANSFORM_ROW, "invertLeftTransform"); break;
            case INVERT_RIGHT_TRANSFORM_ROW:
                checkisproperty(INVERT_RIGHT_TRANSFORM_ROW, "invertRightTransform"); break;
            case INVERT_RESULT_ROW:
                checkisproperty(INVERT_RESULT_ROW, "invertResult"); break;
            }
        }
    }else{
        LogFile::writeToLog("CombineTransformsModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void CombineTransformsModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("CombineTransformsModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void CombineTransformsModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("translationOut", TRANSLATION_OUT_ROW);
            setname("rotationOut", ROTATION_OUT_ROW);
            setname("leftTranslation", LEFT_TRANSLATION_ROW);
            setname("leftRotation", LEFT_ROTATION_ROW);
            setname("rightTranslation", RIGHT_TRANSLATION_ROW);
            setname("rightRotation", RIGHT_ROTATION_ROW);
            setname("invertLeftTransform", INVERT_LEFT_TRANSFORM_ROW);
            setname("invertRightTransform", INVERT_RIGHT_TRANSFORM_ROW);
            setname("invertResult", INVERT_RESULT_ROW);
        }
    }else{
        LogFile::writeToLog("CombineTransformsModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void CombineTransformsModifierUI::setBindingVariable(int index, const QString &name){
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
        case TRANSLATION_OUT_ROW:
            checkisproperty(TRANSLATION_OUT_ROW, "translationOut", VARIABLE_TYPE_VECTOR4); break;
        case ROTATION_OUT_ROW:
            checkisproperty(ROTATION_OUT_ROW, "rotationOut", VARIABLE_TYPE_QUATERNION); break;
        case LEFT_TRANSLATION_ROW:
            checkisproperty(LEFT_TRANSLATION_ROW, "leftTranslation", VARIABLE_TYPE_VECTOR4); break;
        case LEFT_ROTATION_ROW:
            checkisproperty(LEFT_ROTATION_ROW, "leftRotation", VARIABLE_TYPE_QUATERNION); break;
        case RIGHT_TRANSLATION_ROW:
            checkisproperty(RIGHT_TRANSLATION_ROW, "rightTranslation", VARIABLE_TYPE_VECTOR4); break;
        case RIGHT_ROTATION_ROW:
            checkisproperty(RIGHT_ROTATION_ROW, "rightRotation", VARIABLE_TYPE_QUATERNION); break;
        case INVERT_LEFT_TRANSFORM_ROW:
            checkisproperty(INVERT_LEFT_TRANSFORM_ROW, "invertLeftTransform", VARIABLE_TYPE_BOOL); break;
        case INVERT_RIGHT_TRANSFORM_ROW:
            checkisproperty(INVERT_RIGHT_TRANSFORM_ROW, "invertRightTransform", VARIABLE_TYPE_BOOL); break;
        case INVERT_RESULT_ROW:
            checkisproperty(INVERT_RESULT_ROW, "invertResult", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("CombineTransformsModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
