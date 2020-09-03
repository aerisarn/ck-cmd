#include "animationsui.h"
#include "src/ui/skyrimanimationdataui.h"

#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbcharacterstringdata.h"
#include "src/ui/genericdatawidgets.h"
#include "src/animData/skyrimanimdata.h"
#include "src/animData/projectanimdata.h"
#include "src/filetypes/characterfile.h"

#include <QPushButton>
#include <QMessageBox>
#include <QStackedLayout>
#include <QStringList>
#include <QSignalMapper>
#include <QTableWidget>
#include <QHeaderView>
#include <QCoreApplication>
#include <QFileDialog>

using namespace UI;

const QStringList AnimationsUI::headerLabels = {
    "Name",
    "Value"
};

AnimationsUI::AnimationsUI(const QString &title)
    : dataUI(nullptr),
      verLyt(new QVBoxLayout),
      animData(nullptr),
      loadedData(nullptr),
      table(new TableWidget),
      buttonLyt(new QHBoxLayout),
      addObjectPB(new QPushButton("Add Animation")),
      removeObjectPB(new QPushButton("Remove Animation")),
      stackLyt(new QStackedLayout),
      animationUI(new SkyrimAnimationDataUI())
{
    setTitle(title);
    stackLyt->addWidget(table);
    stackLyt->addWidget(animationUI);
    stackLyt->setCurrentIndex(TABLE_WIDGET);
    buttonLyt->addWidget(addObjectPB, 1);
    buttonLyt->addSpacing(2);
    buttonLyt->addWidget(removeObjectPB, 1);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels(headerLabels);
    verLyt->addLayout(buttonLyt);
    verLyt->addLayout(stackLyt);
    setLayout(verLyt);
    toggleSignals(true);
}

void AnimationsUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeAnimation()), Qt::UniqueConnection);
        connect(addObjectPB, SIGNAL(pressed()), this, SLOT(addAnimation()), Qt::UniqueConnection);
        //connect(animationName, SIGNAL(editingFinished()), this, SLOT(renameSelectedAnimation()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellClicked(int,int)), this, SLOT(viewAnimation(int,int)), Qt::UniqueConnection);
        connect(animationUI, SIGNAL(returnToParent()), this, SLOT(returnToTable()), Qt::UniqueConnection);
    }else{
        disconnect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeAnimation()));
        disconnect(addObjectPB, SIGNAL(pressed()), this, SLOT(addAnimation()));
        //disconnect(animationName, SIGNAL(editingFinished()), this, SLOT(renameSelectedAnimation()));
        disconnect(table, SIGNAL(cellClicked(int,int)), this, SLOT(viewAnimation(int,int)));
        disconnect(animationUI, SIGNAL(returnToParent()), this, SLOT(returnToTable()));
    }
}

void AnimationsUI::viewAnimation(int row, int column){
    if (column == 1){
        if (animData){
            if (loadedData){
                if (loadedData->getNumberOfAnimations() > row && row >= 0){
                    animationUI->loadData(animData->findMotionData(row));
                    stackLyt->setCurrentIndex(ANIMATION_WIDGET);
                }else{
                    LogFile::writeToLog("AnimationsUI::viewAnimation(): Invalid row selected!!");
                }
            }else{
                LogFile::writeToLog("AnimationsUI::viewAnimation(): loadedData is nullptr!!");
            }
        }else{
            LogFile::writeToLog("AnimationsUI::viewAnimation(): animData is nullptr!!");
        }
    }
}

void AnimationsUI::returnToTable(){
    stackLyt->setCurrentIndex(TABLE_WIDGET);
}

void AnimationsUI::renameSelectedAnimation(){
    if (loadedData){
        /*auto newName = animationName->text();
        table->item(table->currentRow(), 0)->setText(newName);
        loadedData->setAnimation(newName, table->currentRow());
        emit animationNameChanged(newName, table->currentRow());*/
    }else{
        LogFile::writeToLog("AnimationsUI::viewAnimation(): loadedData is nullptr!!");
    }
}

void AnimationsUI::loadData(HkxObject *data, ProjectAnimData *animdata){
    if (data){
        if (data->getSignature() == HKB_CHARACTER_STRING_DATA){
            loadedData = static_cast<hkbCharacterStringData *>(data);
            animData = animdata;
            auto animationNames = loadedData->getAnimationNames();
            for (auto i = 0; i < animationNames.size(); i++){
                auto row = table->rowCount();
                if (table->rowCount() > i){
                    table->setRowHidden(i, false);
                    (table->item(row, 0)) ? table->item(row, 0)->setText(animationNames.at(i)) : table->setItem(row, 0, new QTableWidgetItem(animationNames.at(i)));
                }else{
                    table->setRowCount(row + 1);
                    table->setItem(row, 0, new QTableWidgetItem(animationNames.at(i)));
                    table->setItem(row, 1, new QTableWidgetItem("Edit"));
                }
            }
            for (auto j = animationNames.size(); j < table->rowCount(); j++){
                table->setRowHidden(j, true);
            }
        }
    }else{
        LogFile::writeToLog("AnimationsUI::loadData(): The data is nullptr!!");
    }
}

void AnimationsUI::clear(){
    for (auto i = table->rowCount() - 1; i >= 0; i--){
        table->removeRow(i);
    }
}

void AnimationsUI::addAnimation(){
    if (loadedData){
        auto filename = QFileDialog::getOpenFileName(this, tr("Open hkx animation file..."), loadedData->getParentFile()->fileName(), tr("hkx Files (*.hkx)"));
        if (filename != "" && (filename.contains(loadedData->getParentFile()->fileName().section("/", 0, -3)+"/"+"animations", Qt::CaseInsensitive) || filename.contains("SharedKillMoves", Qt::CaseInsensitive)) && loadedData->getAnimationIndex(filename) == -1){
            auto animationNames = loadedData->getAnimationNames();
            auto row = table->rowCount();
            loadedData->addAnimation(filename.section("/", -2, -1).replace("/", "\\"));
            animData->appendAnimation(new SkyrimAnimationMotionData(animData, animationNames.size() - 1));  //Need to set duration later...
            table->setRowCount(row + 1);
            table->setItem(row, 0, new QTableWidgetItem(animationNames.last()));
            table->setItem(row, 1, new QTableWidgetItem("Edit"));
            (stackLyt->currentIndex() == ANIMATION_WIDGET) ? stackLyt->setCurrentIndex(TABLE_WIDGET) : NULL;
            table->setCurrentCell(row, 0);
            emit openAnimationFile(filename);
            emit animationAdded(animationNames.last());
        }
    }
}

void AnimationsUI::removeAnimation(){
    if (loadedData && !static_cast<CharacterFile *>(loadedData->getParentFile())->isAnimationUsed(table->item(table->currentRow(), table->currentColumn())->text())){
        auto index = table->currentRow();
        (!animData->removeAnimation(index)) ? LogFile::writeToLog("AnimationsUI::removeAnimation(): Failed!") : NULL;
        loadedData->animationNames.removeAt(index);
        (index < table->rowCount()) ? table->removeRow(index) : NULL;
        (stackLyt->currentIndex() == ANIMATION_WIDGET) ? stackLyt->setCurrentIndex(TABLE_WIDGET) : NULL;
        loadedData->setIsFileChanged(true);
        emit animationRemoved(index);
        table->setFocus();
    }else{
        WARNING_MESSAGE(QString("Animation is in use! Check the text log for information on where the animation is used..."))
    }
}

void AnimationsUI::setHkDataUI(HkDataUI *ui){
    dataUI = ui;
}
