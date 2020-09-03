#include "boneweightarrayui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/filetypes/characterfile.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"
#include "src/ui/genericdatawidgets.h"

using namespace UI;

#define NAME_COLUMN 0
#define VALUE_COLUMN 1

const QStringList BoneWeightArrayUI::headerLabels = {
    "Bone Name",
    "Weight"
};

BoneWeightArrayUI::BoneWeightArrayUI()
    : bsData(nullptr),
      topLyt(new QGridLayout(this)),
      returnPB(new QPushButton("Return")),
      bones(new TableWidget),
      selectedBone(new DoubleSpinBox),
      label(new QLabel("Selected Bone Weight:")),
      setAllCB(new QCheckBox("Set All")),
      invertAllPB(new QPushButton("Invert All"))
{
    setTitle("hkbBoneWeightArray");
    bones->setSelectionBehavior(QAbstractItemView::SelectRows);
    bones->setColumnCount(2);
    bones->setHorizontalHeaderLabels(headerLabels);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(bones, 1, 0, 8, 4);
    topLyt->addWidget(setAllCB, 10, 0, 2, 1);
    topLyt->addWidget(invertAllPB, 10, 1, 2, 1);
    topLyt->addWidget(label, 10, 2, 2, 1);
    topLyt->addWidget(selectedBone, 10, 3, 2, 1);
    setLayout(topLyt);
    //label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //selectedBone->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    selectedBone->setMinimum(0);
    selectedBone->setMaximum(1);
    selectedBone->setSingleStep(0.05);
    connect(selectedBone, SIGNAL(editingFinished()), this, SLOT(setBoneWeight()), Qt::UniqueConnection);
    connect(bones, SIGNAL(cellClicked(int,int)), this, SLOT(loadBoneWeight(int,int)), Qt::UniqueConnection);
    connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
    connect(invertAllPB, SIGNAL(released()), this, SLOT(invert()), Qt::UniqueConnection);
}

void BoneWeightArrayUI::loadData(HkxObject *data, bool isRagdoll){
    blockSignals(true);
    if (data && data->getSignature() == HKB_BONE_WEIGHT_ARRAY){
        HkxFile *file = dynamic_cast<BehaviorFile *>(bsData->getParentFile());
        QStringList boneNames;
        bsData = static_cast<hkbBoneWeightArray *>(data);
        if (file){
            (isRagdoll) ? boneNames = static_cast<BehaviorFile *>(file)->getRagdollBoneNames() : boneNames = static_cast<BehaviorFile *>(file)->getRigBoneNames();
        }else{
            file = dynamic_cast<CharacterFile *>(bsData->getParentFile());
            if (file){
                (isRagdoll) ? boneNames = static_cast<CharacterFile *>(file)->getRagdollBoneNames() : boneNames = static_cast<BehaviorFile *>(file)->getRigBoneNames();
            }else{
                LogFile::writeToLog("BoneWeightArrayUI::loadData(): Parent file type is unrecognized!!!");
            }
        }
        for (auto i = 0; i < bsData->getBoneWeightsSize() && i < boneNames.size(); i++){
            auto rowCount = bones->rowCount();
            if (rowCount > i){
                auto itemname = bones->item(i, NAME_COLUMN);
                auto itemvalue = bones->item(i, VALUE_COLUMN);
                bones->setRowHidden(i, false);
                (itemname) ? itemname->setText(boneNames.at(i)) : bones->setItem(i, NAME_COLUMN, new TableWidgetItem(boneNames.at(i), Qt::AlignCenter, QColor(Qt::white)));
                (itemvalue) ? itemvalue->setText(QString::number(bsData->getBoneWeightAt(i), char('f'), 6)) : bones->setItem(i, VALUE_COLUMN, new TableWidgetItem(QString::number(bsData->getBoneWeightAt(i), char('f'), 6), Qt::AlignCenter, QColor(Qt::white)));
            }else{
                bones->setRowCount(rowCount + 1);
                bones->setItem(rowCount, NAME_COLUMN, new TableWidgetItem(boneNames.at(i), Qt::AlignCenter, QColor(Qt::white)));
                bones->setItem(i, VALUE_COLUMN, new TableWidgetItem(QString::number(bsData->getBoneWeightAt(i), char('f'), 6), Qt::AlignCenter, QColor(Qt::white)));
            }
        }
        for (auto j = bsData->getBoneWeightsSize(); j < bones->rowCount(); j++){
            bones->setRowHidden(j, true);
        }
    }else{
        LogFile::writeToLog("BoneWeightArrayUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    blockSignals(false);
}

void BoneWeightArrayUI::setBoneWeight(){
    if (bsData){
        auto row = bones->currentRow();
        auto numbones = bsData->getBoneWeightsSize();
        if (numbones > row && row >= 0){
            if (setAllCB->isChecked()){
                for (auto i = 0; i < numbones; i++){
                    bsData->setBoneWeightAt(i, selectedBone->value());
                    auto item = bones->item(i, VALUE_COLUMN);
                    (item) ? item->setText(QString::number(selectedBone->value(), char('f'), 6)) : bones->setItem(i, VALUE_COLUMN, new TableWidgetItem(QString::number(selectedBone->value(), char('f'), 6), Qt::AlignCenter, QColor(Qt::white)));
                }
            }else{
                bsData->setBoneWeightAt(row, selectedBone->value());
                auto item = bones->item(row, VALUE_COLUMN);
                (item) ? item->setText(QString::number(selectedBone->value(), char('f'), 6)) : bones->setItem(row, VALUE_COLUMN, new TableWidgetItem(QString::number(selectedBone->value(), char('f'), 6), Qt::AlignCenter, QColor(Qt::white)));
            }
        }
    }else{
        LogFile::writeToLog("BoneWeightArrayUI::setBoneWeight(): The 'bsData' pointer is nullptr!!");
    }
}

void BoneWeightArrayUI::loadBoneWeight(int row, int){
    (bsData) ? selectedBone->setValue(bsData->getBoneWeightAt(row)) : LogFile::writeToLog("BoneWeightArrayUI::setBoneWeight(): The 'bsData' pointer is nullptr!!");
}

void BoneWeightArrayUI::invert(){
    if (bsData){
        auto numbones = bsData->getBoneWeightsSize();
        for (auto i = 0; i < numbones; i++){
            bsData->setBoneWeightAt(i, 1 - bsData->getBoneWeightAt(i));
        }
        for (auto i = 0; i < bones->rowCount() && i < numbones; i++){
            auto item = bones->item(i, VALUE_COLUMN);
            (item) ? item->setText(QString::number(bsData->boneWeights.at(i), char('f'), 6)) : bones->setItem(i, VALUE_COLUMN, new TableWidgetItem(QString::number(bsData->boneWeights.at(i), char('f'), 6), Qt::AlignCenter, QColor(Qt::white)));
        }
    }else{
        LogFile::writeToLog("BoneWeightArrayUI::invert(): The 'bsData' pointer is nullptr!!");
    }
}
