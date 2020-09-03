#include "animationtranslationui.h"

#include "src/utility.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define LOCAL_TIME_ROW 0
#define X_ROW 1
#define Y_ROW 2
#define Z_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList AnimationTranslationUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

AnimationTranslationUI::AnimationTranslationUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      localTime(new DoubleSpinBox),
      x(new DoubleSpinBox),
      y(new DoubleSpinBox),
      z(new DoubleSpinBox)
{
    localTime->setMinimum(0);
    setTitle("Animation Translation");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(LOCAL_TIME_ROW, NAME_COLUMN, new TableWidgetItem("localTime"));
    table->setItem(LOCAL_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(LOCAL_TIME_ROW, VALUE_COLUMN, localTime);
    table->setItem(X_ROW, NAME_COLUMN, new TableWidgetItem("x"));
    table->setItem(X_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(X_ROW, VALUE_COLUMN, x);
    table->setItem(Y_ROW, NAME_COLUMN, new TableWidgetItem("y"));
    table->setItem(Y_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(Y_ROW, VALUE_COLUMN, y);
    table->setItem(Z_ROW, NAME_COLUMN, new TableWidgetItem("z"));
    table->setItem(Z_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(Z_ROW, VALUE_COLUMN, z);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void AnimationTranslationUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(localTime, SIGNAL(editingFinished()), this, SLOT(setLocalTime()), Qt::UniqueConnection);
        connect(x, SIGNAL(editingFinished()), this, SLOT(setX()), Qt::UniqueConnection);
        connect(y, SIGNAL(editingFinished()), this, SLOT(setY()), Qt::UniqueConnection);
        connect(z, SIGNAL(editingFinished()), this, SLOT(setZ()), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(localTime, SIGNAL(editingFinished()), this, SLOT(setLocalTime()));
        disconnect(x, SIGNAL(editingFinished()), this, SLOT(setX()));
        disconnect(y, SIGNAL(editingFinished()), this, SLOT(setY()));
        disconnect(z, SIGNAL(editingFinished()), this, SLOT(setZ()));
    }
}

void AnimationTranslationUI::loadData(SkyrimAnimationTranslation *trans, qreal maxtime){
    toggleSignals(false);
    if (trans){
        bsData = trans;
        localTime->setMaximum(maxtime);
        localTime->setValue(bsData->localTime);
        x->setValue(bsData->x);
        y->setValue(bsData->y);
        z->setValue(bsData->z);
    }else{
        LogFile::writeToLog("AnimationTranslationUI::loadData(): Data is null!!!");
    }
    toggleSignals(true);
}

void AnimationTranslationUI::setLocalTime(){
    (bsData) ? bsData->localTime = localTime->value() : LogFile::writeToLog("AnimationTranslationUI::setlocalTime(): Data is null!!!");
}

void AnimationTranslationUI::setX(){
    (bsData) ? bsData->x = x->value() : LogFile::writeToLog("AnimationTranslationUI::setx(): Data is null!!!");
}

void AnimationTranslationUI::setY(){
    (bsData) ? bsData->y = y->value() : LogFile::writeToLog("AnimationTranslationUI::sety(): Data is null!!!");
}

void AnimationTranslationUI::setZ(){
    (bsData) ? bsData->z = z->value() : LogFile::writeToLog("AnimationTranslationUI::setz(): Data is null!!!");
}
