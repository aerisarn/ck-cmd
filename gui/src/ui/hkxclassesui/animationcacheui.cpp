#include "animationcacheui.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QFileDialog>

#include "src/utility.h"
#include "src/filetypes/projectfile.h"
#include "src/ui/genericdatawidgets.h"
#include "src/animSetData/animcachevariable.h"
#include "src/animSetData/animcacheclipinfo.h"
#include "src/animSetData/animcacheanimationinfo.h"
#include "src/animSetData/animcacheanimsetdata.h"

using namespace UI;

CacheVariableWidget::CacheVariableWidget()
    : bsData(nullptr),
      lyt(new QVBoxLayout),
      nameLyt(new QFormLayout),
      minValueLyt(new QFormLayout),
      maxValueLyt(new QFormLayout),
      name(new LineEdit),
      minValue(new SpinBox),
      maxValue(new SpinBox),
      returnButton(new QPushButton("Return"))
{
    setTitle("Behavior Variable Cache Data");
    setLayout(lyt);
    lyt->addWidget(returnButton);
    lyt->addLayout(nameLyt);
    lyt->addLayout(minValueLyt);
    lyt->addLayout(maxValueLyt);
    nameLyt->setHorizontalSpacing(2);
    minValueLyt->setHorizontalSpacing(2);
    maxValueLyt->setHorizontalSpacing(2);
    nameLyt->addRow("Variable Name: ", name);
    minValueLyt->addRow("Minimum Value: ", minValue);
    maxValueLyt->addRow("Maximum Value: ", maxValue);
    toggleSignals(true);
}

void CacheVariableWidget::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textChanged(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(minValue, SIGNAL(valueChanged(int)), this, SLOT(setMinimumValue(int)), Qt::UniqueConnection);
        connect(maxValue, SIGNAL(valueChanged(int)), this, SLOT(setMaximumValue(int)), Qt::UniqueConnection);
        connect(returnButton, SIGNAL(released()), this, SIGNAL(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textChanged(QString)), this, SLOT(setName(QString)));
        disconnect(minValue, SIGNAL(valueChanged(int)), this, SLOT(setMinimumValue(int)));
        disconnect(maxValue, SIGNAL(valueChanged(int)), this, SLOT(setMaximumValue(int)));
        disconnect(returnButton, SIGNAL(released()), this, SIGNAL(returnToWidget()));
    }
}

void CacheVariableWidget::loadData(AnimCacheVariable *data){
    toggleSignals(false);
    if (data){
        bsData = data;
        name->setText(bsData->getName());
        minValue->setValue(bsData->minValue);
        maxValue->setValue(bsData->maxValue);
    }else{
        LogFile::writeToLog("CacheVariableWidget::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void CacheVariableWidget::setName(const QString & newname){
    (bsData) ? bsData->name = newname : LogFile::writeToLog("CacheVariableWidget::setName(): The data is nullptr!!");
}

void CacheVariableWidget::setMinimumValue(int newvalue){
    (bsData) ? bsData->minValue = newvalue : LogFile::writeToLog("CacheVariableWidget::setMinimumValue(): The data is nullptr!!");
}

void CacheVariableWidget::setMaximumValue(int newvalue){
    (bsData) ? bsData->maxValue = newvalue : LogFile::writeToLog("CacheVariableWidget::setMaximumValue(): The data is nullptr!!");
}

////////////////////////////////////////////////////////////////////////////////////////////////


CacheClipWidget::CacheClipWidget()
    : bsData(nullptr),
      lyt(new QVBoxLayout),
      eventLyt(new QFormLayout),
      unknownLyt(new QFormLayout),
      eventName(new LineEdit),
      unknown(new SpinBox),
      buttonLyt(new QHBoxLayout),
      addClipButton(new QPushButton("Add Clip Generator")),
      removeClipButton(new QPushButton("Remove Selected Clip Generator")),
      clipGenerators(new TableWidget),
      returnButton(new QPushButton("Return"))
{
    setTitle("Clip Generator Cache Data");
    setLayout(lyt);
    lyt->addWidget(returnButton);
    lyt->addLayout(eventLyt);
    lyt->addLayout(unknownLyt);
    lyt->addLayout(buttonLyt);
    lyt->addWidget(clipGenerators);
    eventLyt->setHorizontalSpacing(2);
    unknownLyt->setHorizontalSpacing(2);
    QStringList list = {"Clip Generator Names"};
    clipGenerators->setColumnCount(list.size());
    clipGenerators->setHorizontalHeaderLabels(list);
    clipGenerators->setEditTriggers(QAbstractItemView::DoubleClicked);
    clipGenerators->setToolTip("Double Click To Edit");
    eventLyt->addRow("Event Name: ", eventName);
    unknownLyt->addRow("Unknown Variable: ", unknown);
    buttonLyt->addWidget(addClipButton);
    buttonLyt->addWidget(removeClipButton);
    toggleSignals(true);
}

void CacheClipWidget::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(eventName, SIGNAL(textChanged(QString)), this, SLOT(setEventName(QString)), Qt::UniqueConnection);
        connect(unknown, SIGNAL(valueChanged(int)), this, SLOT(setUnknown(int)), Qt::UniqueConnection);
        connect(addClipButton, SIGNAL(released()), this, SLOT(addClipGenerator()), Qt::UniqueConnection);
        connect(removeClipButton, SIGNAL(released()), this, SLOT(removeClipGenerator()), Qt::UniqueConnection);
        connect(clipGenerators, SIGNAL(cellChanged(int,int)), this, SLOT(setClipGeneratorAt(int,int)), Qt::UniqueConnection);
        connect(returnButton, SIGNAL(released()), this, SIGNAL(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(eventName, SIGNAL(textChanged(QString)), this, SLOT(setEventName(QString)));
        disconnect(unknown, SIGNAL(valueChanged(int)), this, SLOT(setUnknown(int)));
        disconnect(addClipButton, SIGNAL(released()), this, SLOT(addClipGenerator()));
        disconnect(removeClipButton, SIGNAL(released()), this, SLOT(removeClipGenerator()));
        disconnect(clipGenerators, SIGNAL(cellChanged(int,int)), this, SLOT(setClipGeneratorAt(int,int)));
        disconnect(returnButton, SIGNAL(released()), this, SIGNAL(returnToWidget()));
    }
}

void CacheClipWidget::loadData(AnimCacheClipInfo *data){
    toggleSignals(false);
    if (data){
        bsData = data;
        eventName->setText(bsData->eventName);
        unknown->setValue(bsData->unknown);
        clipGenerators->setRowCount(bsData->clipGenerators.size());
        for (auto i = 0; i < bsData->clipGenerators.size(); i++){
            auto item = clipGenerators->item(i, 0);
            (item) ? item->setText(bsData->clipGenerators.at(i)) : clipGenerators->setItem(i, 0, new TableWidgetItem(bsData->clipGenerators.at(i)));
        }
    }else{
        LogFile::writeToLog("CacheClipWidget::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void CacheClipWidget::setEventName(const QString & name){
    (bsData) ? bsData->eventName = name : LogFile::writeToLog("CacheClipWidget::setEventName(): The data is nullptr!!");
}

void CacheClipWidget::setUnknown(int newvalue){
    (bsData) ? bsData->unknown = newvalue : LogFile::writeToLog("CacheClipWidget::setUnknown(): The data is nullptr!!");
}

void CacheClipWidget::addClipGenerator(){
    if (bsData){
        auto index = clipGenerators->rowCount();
        bsData->clipGenerators.append("NEWCLIP");
        clipGenerators->setRowCount(index + 1);
        auto item = clipGenerators->item(index, 0);
        (item) ? item->setText("NEWCLIP") : clipGenerators->setItem(index, 0, new TableWidgetItem("NEWCLIP"));
    }else{
        LogFile::writeToLog("CacheClipWidget::addClipGenerator(): The data is nullptr!!");
    }
}

void CacheClipWidget::removeClipGenerator(){
    if (bsData){
        auto index = clipGenerators->currentRow();
        if (index > -1 && index < bsData->clipGenerators.size()){
            bsData->clipGenerators.removeAt(index);
            clipGenerators->removeRow(index);
        }
    }else{
        LogFile::writeToLog("CacheClipWidget::removeClipGenerator(): The data is nullptr!!");
    }
}

void CacheClipWidget::setClipGeneratorAt(int row, int column){
    if (bsData){
        if (row > -1 && row < bsData->clipGenerators.size()){
            bsData->clipGenerators[row] = clipGenerators->item(row, column)->text();
        }else{
            LogFile::writeToLog("CacheClipWidget::setClipGeneratorAt(): Mismatch between data and UI!!");
        }
    }else{
        LogFile::writeToLog("CacheClipWidget::setClipGeneratorAt(): The data is nullptr!!");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////


CacheWidget::CacheWidget()
    : bsData(nullptr),
      projectData(nullptr),
      lyt(new QVBoxLayout),
      eventButtonLyt(new QHBoxLayout),
      addEventButton(new QPushButton("Add Cache Event")),
      removeEventButton(new QPushButton("Remove Selected Cache Event")),
      cacheEvents(new TableWidget),
      variableButtonLyt(new QHBoxLayout),
      addVariableButton(new QPushButton("Add Cache Behavior Variable")),
      removeVariableButton(new QPushButton("Remove Selected Cache Behavior Variable")),
      behaviorVariables(new TableWidget),
      clipButtonLyt(new QHBoxLayout),
      addClipButton(new QPushButton("Add Cache Clip Generator Name")),
      removeClipButton(new QPushButton("Remove Selected Cache Clip Generator Name")),
      clipGenerators(new TableWidget),
      animationButtonLyt(new QHBoxLayout),
      addAnimationButton(new QPushButton("Add Cache Animation Name")),
      removeAnimationButton(new QPushButton("Remove Selected Cache Animation Name")),
      animations(new TableWidget),
      variableUI(new CacheVariableWidget),
      clipUI(new CacheClipWidget),
      returnButton(new QPushButton("Return"))
{
    //setTitle("Cache Data");
    auto wid = new QWidget();
    wid->setLayout(lyt);
    lyt->addWidget(returnButton);
    lyt->addLayout(eventButtonLyt);
    lyt->addWidget(cacheEvents);
    eventButtonLyt->addWidget(addEventButton);
    eventButtonLyt->addWidget(removeEventButton);
    lyt->addLayout(variableButtonLyt);
    lyt->addWidget(behaviorVariables);
    variableButtonLyt->addWidget(addVariableButton);
    variableButtonLyt->addWidget(removeVariableButton);
    lyt->addLayout(clipButtonLyt);
    lyt->addWidget(clipGenerators);
    clipButtonLyt->addWidget(addClipButton);
    clipButtonLyt->addWidget(removeClipButton);
    lyt->addLayout(animationButtonLyt);
    lyt->addWidget(animations);
    animationButtonLyt->addWidget(addAnimationButton);
    animationButtonLyt->addWidget(removeAnimationButton);
    QStringList list = {"Event Names"};
    cacheEvents->setColumnCount(list.size());
    cacheEvents->setHorizontalHeaderLabels(list);
    cacheEvents->setEditTriggers(QAbstractItemView::DoubleClicked);
    cacheEvents->setToolTip("Double Click To Edit");
    list = QStringList({"AnimCacheVariable", "Value"});
    behaviorVariables->setColumnCount(list.size());
    behaviorVariables->setHorizontalHeaderLabels(list);
    behaviorVariables->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list = QStringList({"AnimCacheClipInfo", "Value"});
    clipGenerators->setColumnCount(list.size());
    clipGenerators->setHorizontalHeaderLabels(list);
    clipGenerators->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list = QStringList({"Animation Names"});
    animations->setColumnCount(list.size());
    animations->setHorizontalHeaderLabels(list);
    animations->setEditTriggers(QAbstractItemView::DoubleClicked);
    animations->setToolTip("Double Click To Edit");
    eventButtonLyt->setSpacing(2);
    variableButtonLyt->setSpacing(2);
    clipButtonLyt->setSpacing(2);
    animationButtonLyt->setSpacing(2);
    addWidget(wid);
    addWidget(variableUI);
    addWidget(clipUI);
    toggleSignals(true);
}

void CacheWidget::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(cacheEvents, SIGNAL(cellChanged(int,int)), this, SLOT(setCacheEventNameAt(int,int)), Qt::UniqueConnection);
        connect(addEventButton, SIGNAL(released()), this, SLOT(addCacheEvent()), Qt::UniqueConnection);
        connect(removeEventButton, SIGNAL(released()), this, SLOT(removeCacheEvent()), Qt::UniqueConnection);
        connect(addVariableButton, SIGNAL(released()), this, SLOT(addBehaviorVariable()), Qt::UniqueConnection);
        connect(removeVariableButton, SIGNAL(released()), this, SLOT(removeBehaviorVariable()), Qt::UniqueConnection);
        connect(behaviorVariables, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewBehaviorVariable(int,int)), Qt::UniqueConnection);
        connect(addClipButton, SIGNAL(released()), this, SLOT(addClipGenerator()), Qt::UniqueConnection);
        connect(removeClipButton, SIGNAL(released()), this, SLOT(removeClipGenerator()), Qt::UniqueConnection);
        connect(clipGenerators, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewClipGenerator(int,int)), Qt::UniqueConnection);
        connect(addAnimationButton, SIGNAL(released()), this, SLOT(addAnimation()), Qt::UniqueConnection);
        connect(removeAnimationButton, SIGNAL(released()), this, SLOT(removeAnimation()), Qt::UniqueConnection);
        connect(animations, SIGNAL(cellChanged(int,int)), this, SLOT(setAnimationNameAt(int,int)), Qt::UniqueConnection);
        connect(variableUI, SIGNAL(returnToWidget()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(clipUI, SIGNAL(returnToWidget()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(returnButton, SIGNAL(released()), this, SIGNAL(returnToMainWidget()), Qt::UniqueConnection);
    }else{
        disconnect(cacheEvents, SIGNAL(cellChanged(int,int)), this, SLOT(setCacheEventNameAt(int,int)));
        disconnect(addEventButton, SIGNAL(released()), this, SLOT(addCacheEvent()));
        disconnect(removeEventButton, SIGNAL(released()), this, SLOT(removeCacheEvent()));
        disconnect(addVariableButton, SIGNAL(released()), this, SLOT(addBehaviorVariable()));
        disconnect(removeVariableButton, SIGNAL(released()), this, SLOT(removeBehaviorVariable()));
        disconnect(behaviorVariables, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewBehaviorVariable(int,int)));
        disconnect(addClipButton, SIGNAL(released()), this, SLOT(addClipGenerator()));
        disconnect(removeClipButton, SIGNAL(released()), this, SLOT(removeClipGenerator()));
        disconnect(clipGenerators, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewClipGenerator(int,int)));
        disconnect(addAnimationButton, SIGNAL(released()), this, SLOT(addAnimation()));
        disconnect(removeAnimationButton, SIGNAL(released()), this, SLOT(removeAnimation()));
        disconnect(animations, SIGNAL(cellChanged(int,int)), this, SLOT(setAnimationNameAt(int,int)));
        disconnect(variableUI, SIGNAL(returnToWidget()), this, SLOT(returnToWidget()));
        disconnect(clipUI, SIGNAL(returnToWidget()), this, SLOT(returnToWidget()));
        disconnect(returnButton, SIGNAL(released()), this, SIGNAL(returnToMainWidget()));
    }
}

void CacheWidget::loadData(AnimCacheAnimSetData *data, ProjectFile *project){
    toggleSignals(false);
    if (data && project){
        bsData = data;
        projectData = project;
        cacheEvents->setRowCount(bsData->cacheEvents.size());
        auto setrowitem = [&](TableWidget *table, const QString & itemname, int row, const QString & message){
            (table->item(row, 0)) ? table->item(row, 0)->setText(itemname) : table->setItem(row, 0, new TableWidgetItem(itemname)), table->setItem(row, 1, new TableWidgetItem(message));
        };
        for (auto i = 0; i < bsData->cacheEvents.size(); i++){
            setrowitem(cacheEvents, bsData->cacheEvents.at(i), i, "");
        }
        behaviorVariables->setRowCount(bsData->behaviorVariables.size());
        for (auto i = 0; i < bsData->behaviorVariables.size(); i++){
            setrowitem(behaviorVariables, bsData->behaviorVariables.at(i)->getName(), i, "Double Click To Edit");
        }
        clipGenerators->setRowCount(bsData->clipGenerators.size());
        for (auto i = 0; i < bsData->clipGenerators.size(); i++){
            setrowitem(clipGenerators, bsData->clipGenerators.at(i)->getEventName(), i, "Double Click To Edit");
        }
        animations->setRowCount(bsData->animations.size());
        for (auto i = 0; i < bsData->animations.size(); i++){
            setrowitem(animations, projectData->findAnimationNameFromEncryptedData(bsData->animations.at(i)->crcAnimationName), i, "");
        }
    }else{
        LogFile::writeToLog("CacheWidget::loadData(): The data or project is nullptr!!");
    }
    toggleSignals(true);
}

void CacheWidget::setCacheEventNameAt(int row, int column){
    if (bsData){
        (row > -1 && row < bsData->cacheEvents.size()) ? bsData->cacheEvents[row] = cacheEvents->item(row, column)->text() : LogFile::writeToLog("CacheWidget::setEventNameAt(): Mismatch between data and UI!!");
    }else{
        LogFile::writeToLog("CacheWidget::setEventNameAt(): The data is nullptr!!");
    }
}

void CacheWidget::addCacheEvent(){
    if (bsData){
        auto index = cacheEvents->rowCount();
        bsData->cacheEvents.append("NEW_EVENT");
        cacheEvents->setRowCount(index + 1);
        (cacheEvents->item(index, 0)) ? cacheEvents->item(index, 0)->setText("NEW_EVENT") : cacheEvents->setItem(index, 0, new TableWidgetItem("NEW_EVENT"));
    }else{
        LogFile::writeToLog("CacheWidget::addCacheEvent(): The data is nullptr!!");
    }
}

void CacheWidget::removeCacheEvent(){
    if (bsData){
        auto index = cacheEvents->currentRow();
        if (index > -1 && index < bsData->cacheEvents.size()){
            bsData->cacheEvents.removeAt(index);
            cacheEvents->removeRow(index);
        }
    }else{
        LogFile::writeToLog("CacheWidget::removeCacheEvent(): The data is nullptr!!");
    }
}

void CacheWidget::addBehaviorVariable(){
    if (bsData){
        auto index = behaviorVariables->rowCount();
        bsData->behaviorVariables.append(new AnimCacheVariable("NEW_VARIABLE"));
        behaviorVariables->setRowCount(index + 1);
        if (behaviorVariables->item(index, 0)){
            behaviorVariables->item(index, 0)->setText("NEW_VARIABLE");
        }else{
            behaviorVariables->setItem(index, 0, new TableWidgetItem("NEW_VARIABLE"));
            behaviorVariables->setItem(index, 1, new TableWidgetItem("Double Click To Edit"));
        }
    }else{
        LogFile::writeToLog("CacheWidget::addBehaviorVariable(): The data is nullptr!!");
    }
}

void CacheWidget::removeBehaviorVariable(){
    if (bsData){
        auto index = behaviorVariables->currentRow();
        if (index > -1 && index < bsData->behaviorVariables.size()){
            bsData->behaviorVariables.removeAt(index);
            behaviorVariables->removeRow(index);
        }
    }else{
        LogFile::writeToLog("CacheWidget::removeBehaviorVariable(): The data is nullptr!!");
    }
}

void CacheWidget::addClipGenerator(){
    if (bsData){
        auto index = clipGenerators->rowCount();
        bsData->clipGenerators.append(new AnimCacheClipInfo("NEWCLIPGENERATOR"));
        clipGenerators->setRowCount(index + 1);
        if (clipGenerators->item(index, 0)){
            clipGenerators->item(index, 0)->setText("NEWCLIPGENERATOR");
        }else{
            clipGenerators->setItem(index, 0, new TableWidgetItem("NEWCLIPGENERATOR"));
            clipGenerators->setItem(index, 1, new TableWidgetItem("Double Click To Edit"));
        }
    }else{
        LogFile::writeToLog("CacheWidget::addClipGenerator(): The data is nullptr!!");
    }
}

void CacheWidget::removeClipGenerator(){
    if (bsData){
        auto index = clipGenerators->currentRow();
        if (index > -1 && index < bsData->clipGenerators.size()){
            bsData->clipGenerators.removeAt(index);
            clipGenerators->removeRow(index);
        }
    }else{
        LogFile::writeToLog("CacheWidget::removeClipGenerator(): The data is nullptr!!");
    }
}

void CacheWidget::setAnimationNameAt(int row, int column){
    if (bsData && projectData){
        if (row > -1 && row < bsData->animations.size()){
            auto name = animations->item(row, column)->text();
            if (name.count("\\") > 0){
                name = animations->item(row, column)->text().section("\\", -1, -1);
                auto pathextension = animations->item(row, column)->text().remove("\\"+name);
                bsData->animations[row]->setAnimationData(projectData->getProjectAnimationsPath()+"/"+pathextension, name, true);
            }else{
                bsData->animations[row]->setAnimationData(projectData->getProjectAnimationsPath(), name, true);
            }
        }else{
            LogFile::writeToLog("CacheWidget::setAnimationNameAt(): Mismatch between data and UI!!");
        }
    }else{
        LogFile::writeToLog("CacheWidget::setAnimationNameAt(): The data is nullptr!!");
    }
}

void CacheWidget::addAnimation(){
    if (projectData){
        auto filename = QFileDialog::getOpenFileName(this, tr("Select an animation file to add to the current animation cache..."), projectData->fileName().section("/", 0, -2), tr("hkx Files (*.hkx)"));
        if (filename != ""){
            if (bsData && projectData){
                auto index = animations->rowCount();
                auto path = filename.section("/animations/", -1, -1).section("/", 0, -2);
                auto name = filename.section("/", -1, -1).remove(".hkx");
                (path != "") ? path = "/"+path : NULL;
                bsData->animations.append(new AnimCacheAnimationInfo(projectData->getProjectAnimationsPath()+path, name, true));
                animations->setRowCount(index + 1);
                if (animations->item(index, 0)){
                    animations->item(index, 0)->setText(projectData->findAnimationNameFromEncryptedData(bsData->animations.back()->crcAnimationName));
                }else{
                    animations->setItem(index, 0, new TableWidgetItem(projectData->findAnimationNameFromEncryptedData(bsData->animations.back()->crcAnimationName)));
                }
            }else{
                LogFile::writeToLog("CacheWidget::addCacheEvent(): The data is nullptr!!");
            }
        }
    }else{
        LogFile::writeToLog("CacheWidget::addCacheEvent(): The projectData is nullptr!!");
    }
}

void CacheWidget::removeAnimation(){
    if (bsData){
        auto index = animations->currentRow();
        if (index > -1 && index < bsData->animations.size()){
            bsData->animations.removeAt(index);
            animations->removeRow(index);
        }
    }else{
        LogFile::writeToLog("CacheWidget::removeCacheEvent(): The data is nullptr!!");
    }
}

void CacheWidget::viewBehaviorVariable(int row, int column){
    if (bsData){
        if (column == 1){
            if (row > -1 && row < bsData->behaviorVariables.size()){
                variableUI->loadData(bsData->behaviorVariables[row]);
                setCurrentIndex(VARIABLE_WIDGET);
            }
        }
    }else{
        LogFile::writeToLog("CacheWidget::viewBehaviorVariable(): The data is nullptr!!");
    }
}

void CacheWidget::viewClipGenerator(int row, int column){
    if (bsData){
        if (column == 1){
            if (row > -1 && row < bsData->clipGenerators.size()){
                clipUI->loadData(bsData->clipGenerators[row]);
                setCurrentIndex(CLIP_WIDGET);
            }
        }
    }else{
        LogFile::writeToLog("CacheWidget::viewBehaviorVariable(): The data is nullptr!!");
    }
}

void CacheWidget::returnToWidget(){
    loadData(bsData, projectData);
    setCurrentIndex(MAIN_WIDGET);
}

///////////////////////////////////////////////////////////////////////////////////////////////////



AnimationCacheUI::AnimationCacheUI()
    : bsData(nullptr),
      projectData(nullptr),
      lyt(new QVBoxLayout),
      groupBox(new QGroupBox("Animation Cache Files")),
      cacheFiles(new TableWidget),
      cacheUI(new CacheWidget),
      cacheButtonLyt(new QHBoxLayout),
      addCacheButton(new QPushButton("Add Cache File")),
      removeCacheButton(new QPushButton("Remove Selected Cache File"))
{
    lyt->addWidget(cacheFiles);
    groupBox->setLayout(lyt);
    QStringList list = {"Filename", "Value"};
    cacheFiles->setColumnCount(list.size());
    cacheFiles->setHorizontalHeaderLabels(list);
    cacheFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lyt->addLayout(cacheButtonLyt);
    lyt->addWidget(cacheFiles);
    cacheButtonLyt->addWidget(addCacheButton);
    cacheButtonLyt->addWidget(removeCacheButton);
    cacheButtonLyt->setSpacing(2);
    addWidget(groupBox);
    addWidget(cacheUI);
    toggleSignals(true);
}

void AnimationCacheUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(cacheUI, SIGNAL(returnToMainWidget()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(addCacheButton, SIGNAL(released()), this, SLOT(addCacheFile()), Qt::UniqueConnection);
        connect(removeCacheButton, SIGNAL(released()), this, SLOT(removeCacheFile()), Qt::UniqueConnection);
        connect(cacheFiles, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedFile(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(cacheUI, SIGNAL(returnToMainWidget()), this, SLOT(returnToWidget()));
        disconnect(addCacheButton, SIGNAL(released()), this, SLOT(addCacheFile()));
        disconnect(removeCacheButton, SIGNAL(released()), this, SLOT(removeCacheFile()));
        disconnect(cacheFiles, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedFile(int,int)));
    }
}

void AnimationCacheUI::loadData(ProjectFile *project){
    toggleSignals(false);
    if (project){
        projectData = project;
        bsData = projectData->getProjectCacheData();
		if (bsData)
		{
			cacheFiles->setRowCount(bsData->fileNames.size());
			for (auto i = 0; i < bsData->fileNames.size(); i++) {
				if (cacheFiles->item(i, 0)) {
					cacheFiles->item(i, 0)->setText(bsData->fileNames.at(i));
				}
				else {
					cacheFiles->setItem(i, 0, new TableWidgetItem(bsData->fileNames.at(i)));
					cacheFiles->setItem(i, 1, new TableWidgetItem("Double Click To Edit"));
				}
			}
		}
		else {
			LogFile::writeToLog("AnimationCacheUI::loadData(): The data is nullptr!!");
		}
    }else{
        LogFile::writeToLog("AnimationCacheUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void AnimationCacheUI::viewSelectedFile(int row, int column){
    if (bsData){
        if (column == 1){
            if (row > -1 && row < bsData->animSetData.size()){
                cacheUI->loadData(bsData->animSetData[row], projectData);
                setCurrentIndex(CACHE_WIDGET);
            }
        }
    }else{
        LogFile::writeToLog("AnimationCacheUI::viewSelectedFile(): The data is nullptr!!");
    }
}

void AnimationCacheUI::addCacheFile(){
    if (bsData){
        auto index = cacheFiles->rowCount();
        bsData->animSetData.append(new AnimCacheAnimSetData(QStringList("FullBody.txt")));
        bsData->fileNames.append(projectData->getProjectName()+"ProjectData\\"+projectData->getProjectName()+".txt");
        cacheFiles->setRowCount(index + 1);
        if (cacheFiles->item(index, 0)){
            cacheFiles->item(index, 0)->setText(bsData->fileNames.last());
        }else{
            cacheFiles->setItem(index, 0, new TableWidgetItem(bsData->fileNames.last()));
            cacheFiles->setItem(index, 0, new TableWidgetItem("Double Click To Edit"));
        }
    }else{
        LogFile::writeToLog("AnimationCacheUI::addCacheFile(): The data is nullptr!!");
    }
}

void AnimationCacheUI::removeCacheFile(){
    if (bsData){
        auto index = cacheFiles->currentRow();
        if (index > -1 && index < bsData->animSetData.size()){
            bsData->animSetData.removeAt(index);
            cacheFiles->removeRow(index);
        }
    }else{
        LogFile::writeToLog("AnimationCacheUI::removeCacheFile(): The data is nullptr!!");
    }
}

void AnimationCacheUI::returnToWidget(){
    loadData(projectData);  //Inefficient...
    setCurrentIndex(MAIN_WIDGET);
}
