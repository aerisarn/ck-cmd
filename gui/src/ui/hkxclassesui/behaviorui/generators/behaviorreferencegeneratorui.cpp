#include "behaviorreferencegeneratorui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/genericdatawidgets.h"

#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 2

#define NAME_ROW 0
#define BEHAVIOR_NAME_ROW 1

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList BehaviorReferenceGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

BehaviorReferenceGeneratorUI::BehaviorReferenceGeneratorUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      behaviorName(new ComboBox)
{
    setTitle("hkbBehaviorReferenceGenerator");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(BEHAVIOR_NAME_ROW, NAME_COLUMN, new TableWidgetItem("behaviorName"));
    table->setItem(BEHAVIOR_NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setCellWidget(BEHAVIOR_NAME_ROW, VALUE_COLUMN, behaviorName);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BehaviorReferenceGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(behaviorName, SIGNAL(currentTextChanged(QString)), this, SLOT(setBehaviorName(QString)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName()));
        disconnect(behaviorName, SIGNAL(currentTextChanged(QString)), this, SLOT(setBehaviorName(QString)));
    }
}

void BehaviorReferenceGeneratorUI::loadData(HkxObject *data){
    QStringList behaviors;
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_BEHAVIOR_REFERENCE_GENERATOR){
            bsData = static_cast<hkbBehaviorReferenceGenerator *>(data);
            name->setText(bsData->getName());
            behaviors = static_cast<BehaviorFile *>(bsData->getParentFile())->getAllBehaviorFileNames();
            (behaviorName->count() != behaviors.size()) ? behaviorName->clear(), behaviorName->insertItems(0, behaviors) : NULL;
            auto index = behaviorName->findText(bsData->getBehaviorName(), Qt::MatchFixedString);
            if (index >= 0){
                behaviorName->setCurrentIndex(index);
                bsData->setBehaviorName(behaviorName->currentText());
            }else{
                WARNING_MESSAGE("BehaviorReferenceGeneratorUI::loadData(): The data has an invalid behavior name!!!");
            }
        }else{
            LogFile::writeToLog(QString("BehaviorReferenceGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BehaviorReferenceGeneratorUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BehaviorReferenceGeneratorUI::setName(const QString & newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BehaviorReferenceGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BehaviorReferenceGeneratorUI::setBehaviorName(const QString & text){
    if (bsData){
        if (bsData->getParentFilename().contains(text)){
            toggleSignals(false);
            behaviorName->setCurrentIndex(behaviorName->findText(bsData->getBehaviorName(), Qt::MatchFixedString));
            toggleSignals(true);
            WARNING_MESSAGE("I'M SORRY HAL BUT I CAN'T LET YOU DO THAT.\nYou are attempting to create a circular branch!!!");
        }else{
            bsData->setBehaviorName(text);
        }
    }else{
        LogFile::writeToLog("BehaviorReferenceGeneratorUI::setBehaviorName(): The data is nullptr!!");
    }
}

void BehaviorReferenceGeneratorUI::behaviorRenamed(const QString &name, int index){
    if (name != ""){
        (index == behaviorName->currentIndex()) ? bsData->setBehaviorName(name) : NULL;
        toggleSignals(false);
        behaviorName->removeItem(index);
        behaviorName->insertItem(--index, name);
        toggleSignals(true);
    }else{
        LogFile::writeToLog("BehaviorReferenceGeneratorUI::behaviorRenamed(): The new variable name is the empty string!!");
    }
}
