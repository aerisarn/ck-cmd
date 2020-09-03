#include "boneindexarrayui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/filetypes/characterfile.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/mainwindow.h"

using namespace UI;

#define BASE_NUMBER_OF_ROWS 1
#define ADD_RAGDOLL_BONE_ROW 0

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList BoneIndexArrayUI::headerLabels = {
    "Bone Name",
    "Type",
    "Bone Index"
};

BoneIndexArrayUI::BoneIndexArrayUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      returnPB(new QPushButton("Return"))
{
    setTitle("hkbBoneIndexArray");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(ADD_RAGDOLL_BONE_ROW, NAME_COLUMN, new TableWidgetItem("Add Ragdoll Bone", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new ragdoll bone"));
    table->setItem(ADD_RAGDOLL_BONE_ROW, TYPE_COLUMN, new TableWidgetItem("Remove Ragdoll Bone", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected ragdoll bone"));
    table->setItem(ADD_RAGDOLL_BONE_ROW, VALUE_COLUMN, new TableWidgetItem("Change Ragdoll Bone", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to change the selected ragdoll bone"));
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BoneIndexArrayUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
    }else{
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
    }
}

void BoneIndexArrayUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_BONE_INDEX_ARRAY){
            bsData = static_cast<hkbBoneIndexArray *>(data);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("BoneIndexArrayUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BoneIndexArrayUI::loadDynamicTableRows(){
    if (bsData){
        auto numbones = bsData->getNumberOfBoneIndices();
        auto temp = ADD_RAGDOLL_BONE_ROW + numbones + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        auto bones = static_cast<BehaviorFile *>(bsData->getParentFile())->getRagdollBoneNames();
        for (auto i = ADD_RAGDOLL_BONE_ROW + 1, j = 0; j < numbones; i++, j++){
            temp = bsData->getBoneIndexAt(j);
            if (temp < bones.size() && temp >= 0){
                setRowItems(i, bones.at(temp), "Remove", "Edit", "Double click to remove this ragdoll bone", VIEW_BONES_TABLE_TIP);
            }else{
                WARNING_MESSAGE("BoneIndexArrayUI::loadDynamicTableRows(): Invalid bone index found!!");
            }
        }
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void BoneIndexArrayUI::setRowItems(int row, const QString & name, const QString & bind, const QString & value, const QString & tip1, const QString & tip2){
    if (table->item(row, NAME_COLUMN)){
        table->item(row, NAME_COLUMN)->setText(name);
    }else{
        table->setItem(row, NAME_COLUMN, new TableWidgetItem(name, Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::white), QBrush(Qt::black)));
    }
    if (table->item(row, TYPE_COLUMN)){
        table->item(row, TYPE_COLUMN)->setText(bind);
    }else{
        table->setItem(row, TYPE_COLUMN, new TableWidgetItem(bind, Qt::AlignCenter, QColor(Qt::red), QBrush(Qt::black), tip1));
    }
    if (table->item(row, VALUE_COLUMN)){
        table->item(row, VALUE_COLUMN)->setText(value);
    }else{
        table->setItem(row, VALUE_COLUMN, new TableWidgetItem(value, Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), tip2));
    }
}

void BoneIndexArrayUI::connectToTables(GenericTableWidget *ragdollBones){
    if (ragdollBones){
        disconnect(ragdollBones, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(ragdollBones, SIGNAL(elementSelected(int,QString)), this, SLOT(setRagdollBone(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewRagdollBones(int)), ragdollBones, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BoneIndexArrayUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto numbones = bsData->getNumberOfBoneIndices();
        if (row == ADD_RAGDOLL_BONE_ROW && column == NAME_COLUMN){
            addRagdollBone();
        }else if (row > ADD_RAGDOLL_BONE_ROW && row < ADD_RAGDOLL_BONE_ROW + numbones + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (numbones > result && result >= 0){
                if (column == VALUE_COLUMN){
                    emit viewRagdollBones(bsData->getBoneIndexAt(result) + 1);
                }else if (column == TYPE_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the ragdoll bone \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeRagdollBone(result);
                    }
                }
            }else{
                LogFile::writeToLog("BoneIndexArrayUI::viewSelectedChild(): Invalid index of bone to view!!");
            }
        }
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BoneIndexArrayUI::setRagdollBone(int index, const QString &name){
    if (bsData){
        auto row = table->currentRow() - BASE_NUMBER_OF_ROWS;
        if (name != ""){
            if (row >= 0 && row < bsData->getNumberOfBoneIndices()){
                if (index >= 0){
                    bsData->setBoneIndexAt(row, --index);
                    auto item = table->item(table->currentRow(), NAME_COLUMN);
                    (item) ? item->setText(name) : LogFile::writeToLog("BoneIndexArrayUI::setRagdollBone(): Table row does not exist!!");
                }else{
                    removeRagdollBone(row);
                }
            }else{
                LogFile::writeToLog("BoneIndexArrayUI::setRagdollBone(): Invalid bone index!!");
            }
        }else{
            LogFile::writeToLog("BoneIndexArrayUI::setRagdollBone(): Bone name is an empty string!!");
        }
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::setRagdollBone(): The 'bsData' pointer is nullptr!!");
    }
}

void BoneIndexArrayUI::addRagdollBone(){
    if (bsData){
        bsData->addBoneIndex(0);
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::addRagdollBone(): The data is nullptr!!");
    }
}

void BoneIndexArrayUI::removeRagdollBone(int index){
    if (bsData){
        if (index < bsData->getNumberOfBoneIndices() && index >= 0){
            bsData->removeBoneIndexAt(index);
        }else{
            WARNING_MESSAGE("BoneIndexArrayUI::removeRagdollBone(): Invalid index!!");
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("BoneIndexArrayUI::removeRagdollBone(): The data is nullptr!!");
    }
}
