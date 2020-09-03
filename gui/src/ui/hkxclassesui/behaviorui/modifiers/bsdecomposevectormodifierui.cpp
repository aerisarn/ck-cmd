#include "bsdecomposevectormodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/bsdecomposevectormodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 7

#define NAME_ROW 0
#define ENABLE_ROW 1
#define VECTOR_ROW 2
#define X_ROW 3
#define Y_ROW 4
#define Z_ROW 5
#define W_ROW 6

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSDecomposeVectorModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSDecomposeVectorModifierUI::BSDecomposeVectorModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      vector(new QuadVariableWidget),
      x(new DoubleSpinBox),
      y(new DoubleSpinBox),
      z(new DoubleSpinBox),
      w(new DoubleSpinBox)
{
    setTitle("BSDecomposeVectorModifier");
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
    table->setItem(VECTOR_ROW, NAME_COLUMN, new TableWidgetItem("vector"));
    table->setItem(VECTOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(VECTOR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VECTOR_ROW, VALUE_COLUMN, vector);
    table->setItem(X_ROW, NAME_COLUMN, new TableWidgetItem("x"));
    table->setItem(X_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(X_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(X_ROW, VALUE_COLUMN, x);
    table->setItem(Y_ROW, NAME_COLUMN, new TableWidgetItem("y"));
    table->setItem(Y_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(Y_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(Y_ROW, VALUE_COLUMN, y);
    table->setItem(Z_ROW, NAME_COLUMN, new TableWidgetItem("z"));
    table->setItem(Z_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(Z_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(Z_ROW, VALUE_COLUMN, z);
    table->setItem(W_ROW, NAME_COLUMN, new TableWidgetItem("w"));
    table->setItem(W_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(W_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(W_ROW, VALUE_COLUMN, w);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSDecomposeVectorModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(vector, SIGNAL(editingFinished()), this, SLOT(setVector()), Qt::UniqueConnection);
        connect(x, SIGNAL(editingFinished()), this, SLOT(setX()), Qt::UniqueConnection);
        connect(y, SIGNAL(editingFinished()), this, SLOT(setY()), Qt::UniqueConnection);
        connect(z, SIGNAL(editingFinished()), this, SLOT(setZ()), Qt::UniqueConnection);
        connect(w, SIGNAL(editingFinished()), this, SLOT(setW()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(vector, SIGNAL(editingFinished()), this, SLOT(setVector()));
        disconnect(x, SIGNAL(editingFinished()), this, SLOT(setX()));
        disconnect(y, SIGNAL(editingFinished()), this, SLOT(setY()));
        disconnect(z, SIGNAL(editingFinished()), this, SLOT(setZ()));
        disconnect(w, SIGNAL(editingFinished()), this, SLOT(setW()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSDecomposeVectorModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSDecomposeVectorModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSDecomposeVectorModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_DECOMPOSE_VECTOR_MODIFIER){
            bsData = static_cast<BSDecomposeVectorModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            vector->setValue(bsData->getVector());
            x->setValue(bsData->getX());
            y->setValue(bsData->getY());
            z->setValue(bsData->getZ());
            w->setValue(bsData->getW());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(VECTOR_ROW, BINDING_COLUMN, varBind, "vector", table, bsData);
            UIHelper::loadBinding(X_ROW, BINDING_COLUMN, varBind, "x", table, bsData);
            UIHelper::loadBinding(Y_ROW, BINDING_COLUMN, varBind, "y", table, bsData);
            UIHelper::loadBinding(Z_ROW, BINDING_COLUMN, varBind, "z", table, bsData);
            UIHelper::loadBinding(W_ROW, BINDING_COLUMN, varBind, "w", table, bsData);
        }else{
            LogFile::writeToLog("BSDecomposeVectorModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSDecomposeVectorModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSDecomposeVectorModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSDecomposeVectorModifierUI::setName(): The data is nullptr!!");
    }
}

void BSDecomposeVectorModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSDecomposeVectorModifierUI::setEnable(): The data is nullptr!!");
}

void BSDecomposeVectorModifierUI::setVector(){
    (bsData) ? bsData->setVector(vector->value()) : LogFile::writeToLog("BSDecomposeVectorModifierUI::setVector(): The data is nullptr!!");
}

void BSDecomposeVectorModifierUI::setX(){
    (bsData) ? bsData->setX(x->value()) : LogFile::writeToLog("BSDecomposeVectorModifierUI::setX(): The data is nullptr!!");
}

void BSDecomposeVectorModifierUI::setY(){
    (bsData) ? bsData->setY(y->value()) : LogFile::writeToLog("BSDecomposeVectorModifierUI::setY(): The data is nullptr!!");
}

void BSDecomposeVectorModifierUI::setZ(){
    (bsData) ? bsData->setZ(z->value()) : LogFile::writeToLog("BSDecomposeVectorModifierUI::setZ(): The data is nullptr!!");
}

void BSDecomposeVectorModifierUI::setW(){
    (bsData) ? bsData->setW(w->value()) : LogFile::writeToLog("BSDecomposeVectorModifierUI::setW(): The data is nullptr!!");
}

void BSDecomposeVectorModifierUI::viewSelected(int row, int column){
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
            case VECTOR_ROW:
                checkisproperty(VECTOR_ROW, "vector"); break;
            case X_ROW:
                checkisproperty(X_ROW, "x"); break;
            case Y_ROW:
                checkisproperty(Y_ROW, "y"); break;
            case Z_ROW:
                checkisproperty(Z_ROW, "z"); break;
            case W_ROW:
                checkisproperty(W_ROW, "w"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSDecomposeVectorModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSDecomposeVectorModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSDecomposeVectorModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSDecomposeVectorModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("vector", VECTOR_ROW);
            setname("x", X_ROW);
            setname("y", Y_ROW);
            setname("z", Z_ROW);
            setname("w", W_ROW);
        }
    }else{
        LogFile::writeToLog("BSDecomposeVectorModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSDecomposeVectorModifierUI::setBindingVariable(int index, const QString &name){
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
        case VECTOR_ROW:
            checkisproperty(VECTOR_ROW, "vector", VARIABLE_TYPE_VECTOR4); break;
        case X_ROW:
            checkisproperty(X_ROW, "x", VARIABLE_TYPE_REAL); break;
        case Y_ROW:
            checkisproperty(Y_ROW, "y", VARIABLE_TYPE_REAL); break;
        case Z_ROW:
            checkisproperty(Z_ROW, "z", VARIABLE_TYPE_REAL); break;
        case W_ROW:
            checkisproperty(W_ROW, "w", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSDecomposeVectorModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
