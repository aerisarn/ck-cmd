#include "skeletonui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/hksimplelocalframe.h"
#include "src/filetypes/characterfile.h"
#include "src/hkxclasses/animation/hkaskeleton.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/skeletonfile.h"

using namespace UI;

SkeletonUI::SkeletonUI(const QString & title)
    : bsData(nullptr),
      lyt(new QVBoxLayout(this)),
      bones(new TableWidget)
{
    setTitle(title);
    QStringList list = {"Name", "Local Frame Name"};
    bones->setColumnCount(2);
    bones->setHorizontalHeaderLabels(list);
    lyt->addWidget(bones, 8);
    setLayout(lyt);
}

void SkeletonUI::loadData(HkxObject *data, bool isRagdoll){
    if (data){
        if (data->getSignature() == HKA_SKELETON){
            bsData = static_cast<hkaSkeleton *>(data);
            auto file = static_cast<SkeletonFile *>(bsData->getParentFile());
            setTitle(bsData->getName());
            QStringList boneNames;
            if (file){
                (isRagdoll) ? boneNames = static_cast<SkeletonFile *>(bsData->getParentFile())->getBonesFromSkeletonAt(1) : boneNames = static_cast<SkeletonFile *>(bsData->getParentFile())->getBonesFromSkeletonAt(0);
                for (auto i = 0; i < bsData->bones.size(), i < boneNames.size(); i++){
                    auto rowCount = bones->rowCount();
                    if (rowCount > i){
                        bones->setRowHidden(i, false);
                        auto item = bones->item(i, 0);
                        (item) ? item->setText(boneNames.at(i)) : bones->setItem(i, 0, new QTableWidgetItem(boneNames.at(i)));
                        item = bones->item(i, 1);
                        (item) ? item->setText(bsData->getLocalFrameName(i)) : bones->setItem(i, 0, new QTableWidgetItem(bsData->getLocalFrameName(i)));
                    }else{
                        bones->setRowCount(rowCount + 1);
                        bones->setItem(rowCount, 0, new QTableWidgetItem(boneNames.at(i)));
                        bones->setItem(rowCount, 1, new QTableWidgetItem(bsData->getLocalFrameName(i)));
                    }
                }
                for (auto j = bsData->bones.size(); j < bones->rowCount(); j++){
                    bones->setRowHidden(j, true);
                }
            }else{
                LogFile::writeToLog("SkeletonUI::loadData(): parent file is null!!!");
            }
        }else{
            LogFile::writeToLog("SkeletonUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("SkeletonUI::loadData(): data is null!!!");
    }
}

