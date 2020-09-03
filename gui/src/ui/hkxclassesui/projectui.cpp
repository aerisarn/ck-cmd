#include "projectui.h"
#include "src/ui/hkxclassesui/behaviorui/characterpropertiesui.h"
#include "src/ui/hkxclassesui/behaviorui/skeletonui.h"
#include "src/ui/hkxclassesui/behaviorui/animationsui.h"
#include "src/ui/hkxclassesui/behaviorui/footikdriverinfoui.h"
#include "src/ui/hkxclassesui/behaviorui/handikdriverinfoui.h"
#include "src/ui/hkxclassesui/animationcacheui.h"
#include "src/filetypes/projectfile.h"
#include "src/filetypes/characterfile.h"
#include "src/filetypes/skeletonfile.h"
#include "src/hkxclasses/behavior/hkbcharacterdata.h"
#include "src/hkxclasses/animation/hkaskeleton.h"
#include "src/hkxclasses/behavior/hkbfootikdriverinfo.h"
#include "src/hkxclasses/behavior/hkbhandikdriverinfo.h"

#include <QGridLayout>
#include <QFileSystemModel>
#include <QListView>
#include <QSizePolicy>

#include "src/ui/genericdatawidgets.h"

using namespace UI;

ProjectUI::ProjectUI(ProjectFile *file)
    : project(file),
      lyt(new QGridLayout),
      animationCacheUI(new AnimationCacheUI()),
      characterProperties(new CharacterPropertiesUI("Character Properties")),
      skeleton(new SkeletonUI("Skeleton")),
      animations(new AnimationsUI("Animations")),
      enableFootIKCB(new CheckBox("Enable Foot IK")),
      enableHandIKCB(new CheckBox("Enable Hand IK")),
      footIK(new FootIkDriverInfoUI),
      handIK(new HandIkDriverInfoUI),
      fileSys(new QFileSystemModel),
      fileView(new QListView),
      addBehaviorFile(new QPushButton("Add New Behavior")),
      initializeWithCharacterData(new CheckBox("Use Character Events/Variables"))
{
    animationCacheUI->setWindowTitle("Project Animation Cache Data!");
    (file) ? fileView->setRootIndex(fileSys->setRootPath(lastFileSelectedPath)) : fileView->setRootIndex(fileSys->setRootPath(QDir::currentPath()));
    fileSys->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList filters("*.hkx");
    fileSys->setNameFilters(filters);
    fileSys->setNameFilterDisables(false);
    fileView->setModel(fileSys);
    lyt->addWidget(addBehaviorFile, 0, 0, 1, 1);
    lyt->addWidget(fileView, 1, 0, 2, 1);
    lyt->addWidget(characterProperties, 0, 2, 4, 1);
    lyt->addWidget(skeleton, 0, 4, 4, 1);
    lyt->addWidget(animationCacheUI, 0, 6, 6, 2);
    auto layout = new QHBoxLayout;
    layout->addWidget(enableFootIKCB);
    layout->addWidget(enableHandIKCB);
    lyt->addLayout(layout, 3, 0, 1, 1);
    lyt->addWidget(footIK, 4, 0, 2, 1);
    lyt->addWidget(handIK, 4, 2, 2, 1);
    lyt->addWidget(animations, 4, 4, 2, 1);
    setLayout(lyt);
    connect(fileView, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(openFile(QModelIndex)), Qt::UniqueConnection);
    connect(enableFootIKCB, SIGNAL(clicked(bool)), this, SLOT(toggleFootIK(bool)), Qt::UniqueConnection);
    connect(enableHandIKCB, SIGNAL(clicked(bool)), this, SLOT(toggleHandIK(bool)), Qt::UniqueConnection);
    connect(addBehaviorFile, SIGNAL(released()), this, SLOT(addNewBehaviorFile()), Qt::UniqueConnection);
    connect(animations, SIGNAL(openAnimationFile(QString)), this, SIGNAL(openAnimation(QString)), Qt::UniqueConnection);
    connect(animations, SIGNAL(animationRemoved(int)), this, SIGNAL(animationRemoved(int)), Qt::UniqueConnection);
}

void ProjectUI::setFilePath(const QString & path){
    lastFileSelectedPath = path;
}

void ProjectUI::setDisabled(bool disable){
    if (disable){
        skeleton->setEnabled(false);
        animations->setEnabled(false);
        footIK->setEnabled(false);
        handIK->setEnabled(false);
        enableFootIKCB->setEnabled(false);
        enableHandIKCB->setEnabled(false);
    }else{
        skeleton->setEnabled(true);
        animations->setEnabled(true);
        footIK->setEnabled(true);
        handIK->setEnabled(true);
        enableFootIKCB->setEnabled(true);
        enableHandIKCB->setEnabled(true);
    }
}

void ProjectUI::setProject(ProjectFile *file){
    project = file;
    (project) ? fileView->setRootIndex(fileSys->setRootPath(lastFileSelectedPath+"/"+project->getBehaviorDirectoryName())) : NULL;
}

void ProjectUI::loadData(){
    if (project){
        setTitle(project->fileName());
        characterProperties->loadData(project->getCharacterData());
        animations->loadData(project->getCharacterStringData(), project->getAnimDataAt(project->getProjectName()));
        auto skeletondata = project->getSkeleton();
        skeleton->loadData(skeletondata);
        auto footik = project->getFootIkDriverInfo();
        (footik) ? footIK->loadData(footik), enableFootIKCB->setChecked(true) : footIK->setDisabled(true);
        auto bonenames = skeletondata->getBoneNames();
        footIK->loadBoneList(bonenames);
        auto handik = project->getHandIkDriverInfo();
        (handik) ? handIK->loadData(handik), enableHandIKCB->setChecked(true) : handIK->setDisabled(true);
        handIK->loadBoneList(bonenames);
        animationCacheUI->loadData(project);
    }else{
        LogFile::writeToLog("ProjectUI: project is nullptr!!");
    }
}

void ProjectUI::toggleFootIK(bool toggle){
    footIK->setEnabled(toggle);
    if (project){
        (toggle) ? project->addFootIK(), footIK->loadData(project->getFootIkDriverInfo()) : project->disableFootIK();
    }else{
        LogFile::writeToLog("ProjectUI: project is nullptr!!");
    }
}

void ProjectUI::toggleHandIK(bool toggle){
    handIK->setEnabled(toggle);
    if (project){
        (toggle) ? project->addHandIK(), handIK->loadData(project->getHandIkDriverInfo()) : project->disableHandIK();
    }else{
        LogFile::writeToLog("ProjectUI: project is nullptr!!");
    }
}

void ProjectUI::addNewBehaviorFile(){
    emit addBehavior(initializeWithCharacterData->isChecked());
}

AnimationsUI *ProjectUI::getAnimations() const{
    return animations;
}
