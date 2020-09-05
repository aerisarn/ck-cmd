#include "mainwindow.h"
#include "src/filetypes/animationfile.h"
#include "src/filetypes/projectfile.h"
#include "src/filetypes/characterfile.h"
#include "src/filetypes/skeletonfile.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/generators/hkbgenerator.h"
#include "src/hkxclasses/behavior/hkbcharacterdata.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/hkxclassesui/hkdataui.h"
#include "src/ui/hkxclassesui/behaviorui/behaviorvariablesui.h"
#include "src/ui/hkxclassesui/behaviorui/characterpropertiesui.h"
#include "src/ui/hkxclassesui/behaviorui/eventsui.h"
#include "src/ui/hkxclassesui/projectui.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorgraph.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/animationsui.h"

#include <QtWidgets>

using namespace UI;

#define MAX_REFERENCED_BEHAVIOR_FILES 30
#define CONFIRM_CLOSE_PROJECT_WITHOUT_SAVING "WARNING: There are unsaved changes to the project, character or behavior files currently open!\nAre you sure you want to close them without saving?"
#define CONFIRM_CLOSE_FILE_WITHOUT_SAVING "WARNING: There are unsaved changes to the behavior file currently open!\nAre you sure you want to close it without saving?"

MainWindow::MainWindow()
    :
      projectExportPath("data/meshes/actors"),
      topLyt(new QGridLayout(this)),
      topMB(new QMenuBar(this)),
      openPackedProjectA(new QAction("Open Packed Project", this)),
      openUnpackedProjectA(new QAction("Open Unpacked Project", this)),
      exportToSkyrimDirA(new QAction("Export Project To Skyrim Directory", this)),
      exportCurrentFileA(new QAction("Export Current File To Skyrim Directory", this)),
      fileM(new QMenu("File", this)),
      newProjectA(new QAction("Create New Project", this)),
      saveA(new QAction("Save Viewed Behavior File", this)),
      saveProjectA(new QAction("Save Project", this)),
      exitA(new QAction("Exit", this)),
      viewM(new QMenu("View", this)),
      expandA(new QAction("Expand Branch", this)),
      collapseA(new QAction("Retract Branch", this)),
      zoomInA(new QAction("Zoom In", this)),
      zoomOutA(new QAction("Zoom Out", this)),
      findGeneratorA(new QAction("Find Generator", this)),
      findModifierA(new QAction("Find Modifier", this)),
      refocusA(new QAction("Refocus On Selected Item", this)),
      settingsM(new QMenu("Settings", this)),
      mergeM(new QMenu("Merge", this)),
      mergeBehaviorsA(new QAction("Merge Two Behavior Files", this)),
      mergeProjectsA(new QAction("Merge Two Projects", this)),
      generateFNISPatchA(new QAction("Generate FNIS Patch", this)),
      documentationM(new QMenu("Documentation", this)),
      classReferenceHubA(new QAction("UI And Class Reference", this)),
      donateM(new QMenu("Donate", this)),
      sendGibsA(new QAction("Send GIBS", this)),
      setPathToGameFolderA(new QAction("Set Path To Game Directory", this)),
      setProjectExportPathA(new QAction("Set Project Export Path", this)),
      setGameModeA(new QAction("Set Game Mode", this)),
      tabs(new QTabWidget),
      projectFile(nullptr),
      skeletonFile(nullptr),
      projectUI(new ProjectUI(nullptr)),
      iconGBLyt(new QVBoxLayout(this)),
      behaviorGraphViewGB(new QGroupBox("Behavior Graph")),
      objectDataSA(new QScrollArea),
      eventsWid(new EventsUI("Events")),
      variablesWid(new BehaviorVariablesUI("Behavior Variables")),
      objectDataWid(new HkDataUI("Object Data")),
      lastFileSelected("C:/")
{
    //setStyleSheet("QComboBox {background: yellow};QWidget {background: darkGray}");
    projectUI->setDisabled(true);
    hkxcmdPath = QDir::currentPath()+"/hkxcmd.exe";
#ifdef QT_DEBUG
    hkxcmdPath = "c:/users/wayne/desktop/hkxcmd.exe";
#endif
    newProjectA->setStatusTip("Create a new character project!");
    newProjectA->setShortcut(QKeySequence::New);
    openPackedProjectA->setStatusTip("Open a hkx project file!");
    openPackedProjectA->setShortcut(QKeySequence::Open);
    openUnpackedProjectA->setStatusTip("Open a hkx xml project file!");
    //openUnpackedProjectA->setShortcut(QKeySequence::Open);
    exportToSkyrimDirA->setStatusTip("Pack and export the current project to the working Skyrim directory!");
    //exportToSkyrimDirA->setShortcut(QKeySequence::Copy);
    exportCurrentFileA->setStatusTip("Pack and export the current file to the working Skyrim directory!");
    exportCurrentFileA->setShortcut(QKeySequence::Paste);
    saveA->setStatusTip("Save file!");
    saveA->setShortcut(QKeySequence::Save);
    saveProjectA->setStatusTip("Save project!");
    saveProjectA->setShortcut(QKeySequence("Ctrl+S+A"));
    exitA->setStatusTip("Exit!");
    exitA->setShortcut(QKeySequence::Close);
    fileM->addAction(newProjectA);
    fileM->addAction(openPackedProjectA);
    fileM->addAction(openUnpackedProjectA);
    fileM->addAction(exportToSkyrimDirA);
    fileM->addAction(exportCurrentFileA);
    fileM->addAction(saveA);
    fileM->addAction(saveProjectA);
    fileM->addAction(exitA);
    zoomInA->setStatusTip("Zoom In!");
    zoomInA->setShortcut(QKeySequence::ZoomIn);
    zoomOutA->setStatusTip("Zoom Out!");
    zoomOutA->setShortcut(QKeySequence::ZoomOut);
    expandA->setStatusTip("Expand All Children Of The Currently Selected Object!");
    expandA->setShortcut(QKeySequence("Ctrl+E"));
    collapseA->setStatusTip("Retract All Children Of The Currently Selected Object!");
    collapseA->setShortcut(QKeySequence("Ctrl+R"));
    refocusA->setStatusTip("Centers the behavior view on the selected item!");
    refocusA->setShortcut(QKeySequence::Refresh);
    setGameModeA->setCheckable(true);
    settingsM->addAction(setPathToGameFolderA);
    setPathToGameFolderA->setStatusTip("Set The Path To The Game Directory!");
    setPathToGameFolderA->setShortcut(QKeySequence("Ctrl+F"));
    settingsM->addAction(setProjectExportPathA);
    setProjectExportPathA->setStatusTip("Set The Project Export Path!");
    setProjectExportPathA->setShortcut(QKeySequence("Ctrl+X"));
    //settingsM->addAction(setGameModeA);
    mergeM->addAction(mergeBehaviorsA);
    mergeM->addAction(mergeProjectsA);
    mergeM->addAction(generateFNISPatchA);
    mergeBehaviorsA->setStatusTip("Merge Two Behavior Files Together!");
    mergeBehaviorsA->setShortcut(QKeySequence("Ctrl+I"));
    mergeProjectsA->setStatusTip("Merge Two Projects Together!");
    //mergeProjectsA->setShortcut(QKeySequence("Ctrl+P"));
    generateFNISPatchA->setStatusTip("Merge Project With FNIS!");
    generateFNISPatchA->setShortcut(QKeySequence("Ctrl+P"));
    findGeneratorA->setStatusTip("Find Generator In Current Behavior!");
    findGeneratorA->setShortcut(QKeySequence("Ctrl+G"));
    findModifierA->setStatusTip("Find Modifier In Current Behavior!");
    findModifierA->setShortcut(QKeySequence("Ctrl+M"));
    viewM->addAction(zoomInA);
    viewM->addAction(zoomOutA);
    viewM->addAction(expandA);
    viewM->addAction(collapseA);
    viewM->addAction(refocusA);
    viewM->addAction(findGeneratorA);
    viewM->addAction(findModifierA);
    classReferenceHubA->setStatusTip("View Help!");
    classReferenceHubA->setShortcut(QKeySequence("Ctrl+H"));
    documentationM->addAction(classReferenceHubA);
    donateM->addAction(sendGibsA);
    sendGibsA->setStatusTip("Donate To The Author!");
    sendGibsA->setShortcut(QKeySequence("Ctrl+D"));
    topMB->setMaximumHeight(50);
    topMB->addMenu(fileM);
    topMB->addMenu(viewM);
    topMB->addMenu(settingsM);
    topMB->addMenu(mergeM);
    topMB->addMenu(documentationM);
    topMB->addMenu(donateM);
    eventsWid->setHkDataUI(objectDataWid);
    variablesWid->setHkDataUI(objectDataWid);
    objectDataWid->setEventsVariablesAnimationsUI(eventsWid, variablesWid, projectUI->getAnimations());
    iconGBLyt->addWidget(tabs);
    behaviorGraphViewGB->setLayout(iconGBLyt);
    objectDataSA->setWidgetResizable(true);
    topLyt->addWidget(topMB, 0, 0, 1, 10);
    topLyt->addWidget(behaviorGraphViewGB, 1, 0, 10, 6);
    topLyt->addWidget(objectDataSA, 1, 6, 6, 4);
    topLyt->addWidget(eventsWid, 7, 6, 4, 2);
    topLyt->addWidget(variablesWid, 7, 8, 4, 2);
    tabs->setTabsClosable(true);
    readSettings();
    setLayout(topLyt);
    objectDataSA->setWidget(objectDataWid);
    objectDataSA->setVisible(false);
    if (!findGameDirectory(QString("Skyrim"), skyrimDirectory)){
        WARNING_MESSAGE("The TESV executable was not found!");
    }
    if (!findGameDirectory(QString("Skyrim Special Edition"), skyrimSpecialEdtionDirectory)){
        WARNING_MESSAGE("The SSE executable was not found!");
    }else{
        skyrimBehaviorUpdateToolFullPath = skyrimSpecialEdtionDirectory+"/Tools/HavokBehaviorPostProcess/HavokBehaviorPostProcess.exe";
        if (!QFile(skyrimBehaviorUpdateToolFullPath).exists()){
            WARNING_MESSAGE(QString("The tool for the conversion of Havok 32bit to Havok 64bit \"HavokBehaviorPostProcess.exe\" was not found!\n\nTo obtain it, you need to download the Creation Kit for Skyrim Special Edition!"));
        }
    }
    connect(newProjectA, SIGNAL(triggered(bool)), this, SLOT(createNewProject()), Qt::UniqueConnection);
    connect(openPackedProjectA, SIGNAL(triggered(bool)), this, SLOT(openPackedProject()), Qt::UniqueConnection);
    connect(openUnpackedProjectA, SIGNAL(triggered(bool)), this, SLOT(openUnpackedProject()), Qt::UniqueConnection);
    connect(saveA, SIGNAL(triggered(bool)), this, SLOT(save()), Qt::UniqueConnection);
    connect(exportToSkyrimDirA, SIGNAL(triggered(bool)), this, SLOT(packAndExportProjectToSkyrimDirectory(bool)), Qt::UniqueConnection);
    connect(exportCurrentFileA, SIGNAL(triggered(bool)), this, SLOT(packAndExportFileToSkyrimDirectory(bool)), Qt::UniqueConnection);
    connect(saveProjectA, SIGNAL(triggered(bool)), this, SLOT(saveProject(bool)), Qt::UniqueConnection);
    connect(zoomInA, SIGNAL(triggered(bool)), this, SLOT(zoomIn()), Qt::UniqueConnection);
    connect(zoomOutA, SIGNAL(triggered(bool)), this, SLOT(zoomOut()), Qt::UniqueConnection);
    connect(expandA, SIGNAL(triggered(bool)), this, SLOT(expandBranches()), Qt::UniqueConnection);
    connect(collapseA, SIGNAL(triggered(bool)), this, SLOT(collapseBranches()), Qt::UniqueConnection);
    connect(findGeneratorA, SIGNAL(triggered(bool)), this, SLOT(findGenerator()), Qt::UniqueConnection);
    connect(findModifierA, SIGNAL(triggered(bool)), this, SLOT(findModifier()), Qt::UniqueConnection);
    connect(refocusA, SIGNAL(triggered(bool)), this, SLOT(refocus()), Qt::UniqueConnection);
    connect(exitA, SIGNAL(triggered(bool)), this, SLOT(exit()), Qt::UniqueConnection);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changedTabs(int)), Qt::UniqueConnection);
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)), Qt::UniqueConnection);
    connect(projectUI, SIGNAL(openFile(QModelIndex)), this, SLOT(openBehaviorFile(QModelIndex)), Qt::UniqueConnection);
    connect(projectUI, SIGNAL(addBehavior(bool)), this, SLOT(addNewBehavior(bool)), Qt::UniqueConnection);
    connect(projectUI, SIGNAL(openAnimation(QString)), this, SLOT(openAnimationFile(QString)), Qt::UniqueConnection);
    connect(projectUI, SIGNAL(animationRemoved(int)), this, SLOT(removeAnimation(int)), Qt::UniqueConnection);
    connect(setGameModeA, SIGNAL(changed()), this, SLOT(setGameMode()), Qt::UniqueConnection);
    connect(setPathToGameFolderA, SIGNAL(triggered(bool)), this, SLOT(setPathToGameDirectory()), Qt::UniqueConnection);
    connect(setProjectExportPathA, SIGNAL(triggered(bool)), this, SLOT(setProjectExportPath()), Qt::UniqueConnection);
    connect(mergeBehaviorsA, SIGNAL(triggered(bool)), this, SLOT(mergeBehaviors()), Qt::UniqueConnection);
    connect(mergeProjectsA, SIGNAL(triggered(bool)), this, SLOT(mergeProjects()), Qt::UniqueConnection);
    connect(generateFNISPatchA, SIGNAL(triggered(bool)), this, SLOT(mergeFNIS()), Qt::UniqueConnection);
    connect(classReferenceHubA, SIGNAL(triggered(bool)), this, SLOT(viewDocumentation()), Qt::UniqueConnection);
    connect(sendGibsA, SIGNAL(triggered(bool)), this, SLOT(sendGIBS()), Qt::UniqueConnection);
}

MainWindow::~MainWindow(){
    //
}

QMessageBox::StandardButton MainWindow::yesNoDialogue(const QString & message){
    return QMessageBox::warning(0, "Skyrim Behavior Tool", message, QMessageBox::Yes | QMessageBox::No);
}

QMessageBox::StandardButton MainWindow::closeAllDialogue(){
    auto ret = QMessageBox::warning(this, "Skyrim Behavior Tool", CONFIRM_CLOSE_PROJECT_WITHOUT_SAVING, QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Cancel);
    (ret == QMessageBox::SaveAll) ? saveProject(true) : NULL;
    return ret;
}

QMessageBox::StandardButton MainWindow::closeFileDialogue(){
    auto ret = QMessageBox::warning(this, "Skyrim Behavior Tool", CONFIRM_CLOSE_FILE_WITHOUT_SAVING, QMessageBox::Yes | QMessageBox::Save | QMessageBox::Cancel);
    (ret == QMessageBox::Save) ? save() : NULL;
    return ret;
}

void MainWindow::changedTabs(int index){
    if (projectFile){
        if (!projectFile->behaviorFiles.isEmpty()){
            if (index <= 0){
                variablesWid->hide();
                eventsWid->hide();
                objectDataSA->hide();
                topLyt->removeWidget(objectDataSA);
                topLyt->removeWidget(eventsWid);
                topLyt->removeWidget(variablesWid);
                topLyt->removeWidget(behaviorGraphViewGB);
                topLyt->addWidget(behaviorGraphViewGB, 1, 0, 9, 10);
            }else{
                auto graphindex = getBehaviorGraphIndex(tabs->tabText(index));
                if (graphindex < projectFile->behaviorFiles.size() && graphindex < behaviorGraphs.size() && graphindex > -1){
                    objectDataWid->changeCurrentDataWidget(nullptr);
                    objectDataWid->loadBehaviorView(behaviorGraphs.at(graphindex));
                    variablesWid->clear();
                    variablesWid->loadData(projectFile->behaviorFiles.at(graphindex)->getBehaviorGraphData());
                    eventsWid->clear();
                    eventsWid->loadData(projectFile->behaviorFiles.at(graphindex)->getBehaviorGraphData());
                    topLyt->removeWidget(behaviorGraphViewGB);
                    topLyt->addWidget(behaviorGraphViewGB, 1, 0, 9, 6);
                    topLyt->addWidget(objectDataSA, 1, 6, 6, 4);
                    topLyt->addWidget(eventsWid, 7, 6, 4, 2);
                    topLyt->addWidget(variablesWid, 7, 8, 4, 2);
                    variablesWid->show();
                    eventsWid->show();
                    objectDataSA->show();
                }else{
                    LogFile::writeToLog("The tab index is out of sync with the behavior files or behavior graphs!");
                }
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::changedTabs(): No project opened!");
    }
}

void MainWindow::expandBranches(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index >= 0 && index < behaviorGraphs.size()){
            behaviorGraphs.at(index)->expandBranch();
        }else{
            if (tabs->count() < 2){
                WARNING_MESSAGE("No behavior file is currently being viewed!!!")
            }else{
                WARNING_MESSAGE("MainWindow::expandBranches() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::expandBranches(): No project opened!");
    }
}

void MainWindow::collapseBranches(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index >= 0 && index < behaviorGraphs.size()){
            behaviorGraphs.at(index)->contractBranch();
        }else{
            if (tabs->count() < 2){
                WARNING_MESSAGE("No behavior file is currently being viewed!!!")
            }else{
                WARNING_MESSAGE("MainWindow::collapseBranches() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::collapseBranches(): No project opened!");
    }
}

void MainWindow::refocus(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index != -1){
            if (index >= 0 && index < behaviorGraphs.size()){
                (!behaviorGraphs.at(index)->refocus()) ? LogFile::writeToLog("MainWindow::refocus(): No behavior graph item is currently selected!") : NULL;
            }else{
                if (tabs->count() < 2){
                    WARNING_MESSAGE("No behavior file is currently being viewed!!!")
                }else{
                    WARNING_MESSAGE("MainWindow::refocus() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
                }
            }
        }else{
            LogFile::writeToLog("MainWindow::refocus(): No behavior graph is currently viewed!");
        }
    }else{
        LogFile::writeToLog("MainWindow::refocus(): No project opened!");
    }
}

void MainWindow::setGameMode(){
    //
    //
    //
    //
}

void MainWindow::setPathToGameDirectory(){
    bool ok;
    auto path = QInputDialog::getText(this, tr("Set path to game directory!"), tr("Path:"), QLineEdit::Normal, skyrimDirectory, &ok);
    (ok && path != "") ? skyrimDirectory = path : NULL;
}

void MainWindow::setProjectExportPath(){
    bool ok;
    auto path = QInputDialog::getText(this, tr("Set project export path!"), tr("Path:"), QLineEdit::Normal, projectExportPath, &ok);
    (ok && path != "") ? projectExportPath = path : NULL;
}

void MainWindow::save(){
    auto count = 1;
    saveFile(getBehaviorGraphIndex(tabs->tabText(tabs->currentIndex())), count);
}

void MainWindow::saveFile(int index, int & taskCount, bool usenormalsave){
    if (projectFile){
        mutex.lock();
        if (index >= 0 && index < projectFile->behaviorFiles.size()){
            auto backupPath = QDir::currentPath()+"/Backup";
            QDir dir(backupPath);
            (!dir.exists()) ? dir.mkpath(backupPath) : NULL;
            auto behavior = projectFile->behaviorFiles.at(index);
            mutex.unlock();
            if (behavior){
                auto filename = backupPath+"\\"+behavior->fileName().section("/",-1,-1);
                QFile backup(filename);
                if (backup.exists()){
                    (!backup.remove()) ? LogFile::writeToLog(QString("Failed to remove to old backup file "+backupPath.section("/",-1,-1)+"!").toLocal8Bit().data()) : NULL;
                }
                if (!behavior->copy(behavior->fileName(), filename)){
                    LogFile::writeToLog("Backup failed!");
                }else{
                    (!behavior->remove()) ? LogFile::writeToLog(QString("Failed to remove to old file "+backupPath.section("/",-1,-1)+"!").toLocal8Bit().data()) : NULL;
                }
                (usenormalsave) ? behavior->write() : behavior->mergedWrite();
                behavior->setIsChanged(false);
                mutex.lock();
                taskCount--;
                conditionVar.notify_one();
                mutex.unlock();
            }else{
                LogFile::writeToLog("MainWindow::saveFile(): behavior is nullptr!");
            }
        }else{
            LogFile::writeToLog("MainWindow::saveFile(): No behavior file is currently viewed!");
            mutex.unlock();
        }
    }else{
        LogFile::writeToLog("MainWindow::saveFile(): No project opened!");
    }
}

void MainWindow::saveProject(bool usenormalsave){
    auto removefile = [](const QString & path){
        QFile animData(path);
        if (animData.exists()){
            if (!animData.remove()){
                LogFile::writeToLog("Failed to remove to old animData file!");
            }
        }
    };
    if (projectFile && projectFile->character && skeletonFile && projectFile->skyrimAnimData && projectFile->skyrimAnimSetData){
        ProgressDialog progress("Saving project...", "", 0, 100, this, Qt::Dialog);
        progress.setWindowModality(Qt::WindowModal);
        auto percent = 0;
        std::vector <std::thread> threads;
        auto taskCount = projectFile->behaviorFiles.size();
        auto previousCount = taskCount;
        auto fileindex = 0;
        auto maxThreads = std::thread::hardware_concurrency() - 1;
        auto taskdifference = 0;
        auto numbehaviors = projectFile->behaviorFiles.size();
        qreal difference = ((1.0)/((qreal)(numbehaviors)))*(100.0) - 20;
        progress.setProgress("Saving project file...", 1);
        threads.push_back(std::thread(&ProjectFile::write, projectFile));
        progress.setProgress("Saving character file...", 2);
        threads.push_back(std::thread(&CharacterFile::write, projectFile->character));
        if (projectFile->hasAnimData()){
            removefile(lastFileSelectedPath+"/animationdatasinglefile.txt");
            progress.setProgress("Saving animation translation and rotation data (animationdatasinglefile.txt)...", 10);
            threads.push_back(std::thread(&SkyrimAnimData::write, projectFile->skyrimAnimData, QString(lastFileSelectedPath+"/animationdatasinglefile.txt")));
        }
        if (projectFile->hasAnimSetData()){
            removefile(lastFileSelectedPath+"/animationsetdatasinglefile.txt");
            progress.setProgress("Saving animation cache file (animationsetdatasinglefile.txt)...", 15);
            threads.push_back(std::thread(&SkyrimAnimSetData::write, projectFile->skyrimAnimSetData, QString(lastFileSelectedPath+"/animationsetdatasinglefile.txt")));
        }
        for (auto i = 0; i < threads.size(); i++){
            (threads.at(i).joinable()) ? threads.at(i).join() : LogFile::writeToLog("MainWindow::saveProject(): Thread "+QString::number(i)+" failed to join!!!");
        }
        threads.clear();
        progress.setProgress("Saving behavior files...", 20);
        std::unique_lock<std::mutex> locker(mutex);
        for (uint i = 0; i < maxThreads, fileindex < numbehaviors; i++, fileindex++){
            threads.push_back(std::thread(&MainWindow::saveFile, this, fileindex, std::ref(taskCount), usenormalsave));
            threads.back().detach();
        }
        while (taskCount > 0){
            if (taskCount < numbehaviors){
                percent += difference;
                progress.setValue(percent);
            }
            taskdifference = previousCount - taskCount;
            previousCount = taskCount;
            for (; taskdifference > 0; taskdifference--){
                if (fileindex < numbehaviors){
                    threads.push_back(std::thread(&MainWindow::saveFile, this, fileindex, std::ref(taskCount), usenormalsave));
                    threads.back().detach();
                    fileindex++;
                }
            }
            conditionVar.wait(locker, [&](){return (taskCount < previousCount);});
        }
        threads.clear();
        projectFile->setIsChanged(false);
        projectFile->character->setIsChanged(false);
    }else{
        WARNING_MESSAGE("No project open or corrupt project missing essential data!");
    }
}

void MainWindow::packAndExportProjectToSkyrimDirectory(bool exportanimdata){
    if (projectFile){
        LogFile::writeToLog("Exporting the current project to the Skyrim game directory...");
        auto path = skyrimDirectory+"/"+projectExportPath;
        auto projectFolder = path+"/"+lastFileSelectedPath.section("/", -1, -1);
        convertProject(lastFileSelectedPath, projectFolder, "-f SAVE_CONCISE");
        if (exportanimdata){
            exportAnimationData();
        }
        /*auto errors = projectFile->detectErrorsInProject();
        if (errors != ""){
            USER_MESSAGE(errors);
        }*/
    }else{
        WARNING_MESSAGE("MainWindow::packAndExportProjectToSkyrimDirectory(): No project open!");
    }
}

void MainWindow::exportAnimationData(){
    auto exportanimdata = [&](const QString & filename){
        QFile animData(lastFileSelectedPath+"/"+filename);
        if (animData.exists()){
            QDir dir(skyrimDirectory+"/data/meshes");
            if (dir.exists()){
                QFile file(skyrimDirectory+"/data/meshes/"+filename);
                (file.exists()) ? file.remove() : NULL;
                (!animData.copy(skyrimDirectory+"/data/meshes/"+filename)) ? LogFile::writeToLog("Failed to export animationdatasinglefile.txt to the game directory!") : NULL;
            }else{
                if (QDir().mkdir(skyrimDirectory+"/data/meshes")){
                    QFile file(skyrimDirectory+"/data/meshes/"+filename);
                    (file.exists()) ? file.remove() : NULL;
                    (!animData.copy(skyrimDirectory+"/data/meshes/"+filename)) ? LogFile::writeToLog("Failed to export animationdatasinglefile.txt to the game directory!") : NULL;
                }else{
                    LogFile::writeToLog("Failed to create directory: "+skyrimDirectory+"/data/meshes"+"!");
                }
            }
        }else{
            LogFile::writeToLog(filename+" was not found in the project directory!");
        }
    };
    exportanimdata("animationdatasinglefile.txt");
    exportanimdata("animationsetdatasinglefile.txt");
}

void MainWindow::packAndExportFileToSkyrimDirectory(bool exportanimdata){
    QTime timer;
    auto count = 1;
    if (projectFile && projectFile->character){
        if (projectFile->character){
            timer.start();
            LogFile::writeToLog("Exporting the current file to the Skyrim game directory...");
            ProgressDialog dialog("Exporting the current file to the Skyrim game directory...", "", 0, 100, this);
            auto path = skyrimDirectory+"/data/meshes/actors";
            auto projectFolder = path+"/"+lastFileSelectedPath.section("/", -1, -1);
            auto filename = tabs->tabText(tabs->currentIndex());
            auto temppath = projectFolder+"/"+projectFile->character->getBehaviorDirectoryName()+"/"+filename;
            if (hkxcmd(lastFileSelectedPath+"/"+projectFile->character->getBehaviorDirectoryName()+"/"+filename, temppath, count) == HKXCMD_SUCCESS){
                (exportanimdata) ? exportAnimationData() : NULL;
                dialog.setProgress("Behavior file exported sucessfully!", dialog.maximum());
                LogFile::writeToLog("Time taken to export the file is approximately "+QString::number(timer.elapsed())+" milliseconds");
                auto errors = projectFile->detectErrorsInBehavior(temppath);
                if (errors != ""){
                    USER_MESSAGE(errors);
                }
            }else{
                dialog.setProgress("Behavior file export failed!", dialog.maximum());
                LogFile::writeToLog("MainWindow::packAndExportFileToSkyrimDirectory(): Failed to convert and export the behavior directory to the correct folder in the Skyrim directory!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::packAndExportFileToSkyrimDirectory(): No project open!");
    }
}

void MainWindow::mergeBehaviors(){
    auto taskcount = 1;
    auto recessivefilename = QFileDialog::getOpenFileName(this, tr("Select recessive behavior file..."), QDir::currentPath(), tr("hkx Files (*.hkx)"));
    if (recessivefilename != ""){
        auto recessiveBehavior = openBehaviorForMerger(recessivefilename);
        auto dominantfilename = QFileDialog::getOpenFileName(this, tr("Select dominant behavior file..."), QDir::currentPath(), tr("hkx Files (*.hkx)"));
        auto dominantBehavior = openBehaviorForMerger(dominantfilename);
        if (!dominantBehavior || !recessiveBehavior){
            WARNING_MESSAGE("The attempt to merge projects failed!");
        }else{
            dominantBehavior->merge(recessiveBehavior, taskcount, std::ref(mutex), std::ref(conditionVar));
            dominantBehavior->write();
            delete dominantBehavior;
            delete recessiveBehavior;
        }
    }
}

void MainWindow::mergeProjects(){
    QTime timer;
    timer.start();
    auto timeelapsed = timer.elapsed();
    auto recessivefilename = QFileDialog::getOpenFileName(this, tr("Select recessive project file..."), QDir::currentPath(), tr("hkx Files (*.hkx)"));
    if (recessivefilename != ""){
        convertProject(recessivefilename, "");
        openProject(recessivefilename, false, true);
        auto recessiveProject = projectFile;
        auto dominantfilename = QFileDialog::getOpenFileName(this, tr("Select dominant project file..."), QDir::currentPath(), tr("hkx Files (*.hkx)"));
        convertProject(dominantfilename, "");
        openProject(dominantfilename, false, true);
        auto dominantProject = projectFile;
        if (dominantProject->merge(recessiveProject)){
            saveProject(false);
            //TO DO: Merge animdata...
            //packAndExportProjectToSkyrimDirectory();
            //delete recessiveProject;
            //delete projectFile;
            //projectFile = nullptr;
            LogFile::writeToLog("Time taken to merge project \""+recessivefilename+"\" with project \""+dominantfilename+"\" is "+QString::number(timer.elapsed() - timeelapsed)+" milliseconds");
            USER_MESSAGE("The project \""+recessivefilename+"\" was sucessfully merged with \""+dominantfilename+"\" project! This program will now close!");
        }else{
            WARNING_MESSAGE("The attempt to merge projects failed! This program will now close!");
        }
        QApplication::exit(0);
    }
}

void MainWindow::mergeFNIS(){
    QString othercharactername;
    auto replacefile = [&](){
        QFile file(othercharactername);
        if (file.exists() && file.remove()){
            if (!QFile(QString(othercharactername).replace(".hkx", "-out.hkx")).rename(othercharactername)){
                WARNING_MESSAGE("The attempt to rename defaultfemale failed!");return;
            }
        }else{
            LogFile::writeToLog("MainWindow::mergeFNIS(): Failed to remove the character file \""+othercharactername+"!");
        }
    };
    QTime timer;
    timer.start();
    auto timeelapsed = timer.elapsed();
    auto dominantfilename = QFileDialog::getOpenFileName(this, tr("Select project to merge with FNIS..."), QDir::currentPath(), tr("hkx Files (*.hkx)"));
    if (dominantfilename != ""){
        auto recessivefilename = skyrimDirectory+"/data/meshes/actors/character/"+dominantfilename.section("/", -1, -1);
        auto tempproject = new ProjectFile(this, recessivefilename, true, "Characters\\"+dominantfilename.section("/", -1, -1));
        tempproject->write();
        delete tempproject;
        convertProject(recessivefilename, "");
        openProject(recessivefilename, false, false, true);
        auto recessiveProject = projectFile;
        convertProject(dominantfilename, "");
        openProject(dominantfilename, false, false, true);
        auto dominantProject = projectFile;
        dominantProject->ensureAllRefedAnimationsExist();
        recessiveProject->ensureAllRefedAnimationsExist();
        projectFile->writeOrderOfFiles();
        if (dominantProject->merge(recessiveProject, true)){
            //TO DO: Merge animdata...
            //NEED TO FIX ANIMTION INDICES IN ANIMDATA FILES BEFORE MERGING!!!
            //dominantProject->mergeAnimationCaches(recessiveProject);
            projectFile->detectErrorsInProject();
            saveProject(false);
            packAndExportProjectToSkyrimDirectory(false);
            othercharactername = skyrimDirectory+"/data/meshes/actors/character/characters female/defaultfemale.hkx";
            auto defaultfemale = new CharacterFile(this, projectFile, othercharactername);
            auto count = 1;
            if (hkxcmd(othercharactername, "", count, "") == HKXCMD_SUCCESS){
                replacefile();
                if (defaultfemale->parse()){
                    if (projectFile->character && defaultfemale->merge(projectFile->character)){
                        {
                            if (!QFile(othercharactername).remove()){
                                WARNING_MESSAGE("The attempt to remove "+othercharactername+" failed!")//;return;
                            }
                        }
                        defaultfemale->write();
                        if (hkxcmd(othercharactername, "", count) == HKXCMD_SUCCESS){
                            replacefile();
                        }else{
                            WARNING_MESSAGE("The attempt to convert "+othercharactername+" failed!");return;
                        }
                    }else{
                        WARNING_MESSAGE("The attempt to merge defaultfemale failed!");return;
                    }
                }else{
                    WARNING_MESSAGE("The attempt to parse defaultfemale failed!");return;
                }
            }else{
                WARNING_MESSAGE("The attempt to convert "+othercharactername+" failed!");return;
            }
            LogFile::writeToLog("Time taken to merge project \""+recessivefilename+"\" with project \""+dominantfilename+"\" is "+QString::number(timer.elapsed() - timeelapsed)+" milliseconds");
            USER_MESSAGE("FNIS patch was sucessful! This program will now close!");
        }else{
            WARNING_MESSAGE("The FNIS patch failed! This program will now close! Check the debuglog.txt in the application directory for what went wrong!");
        }
        QApplication::exit(0);
    }
}

void MainWindow::viewDocumentation(){
    QUrl url("file:///"+QDir::currentPath()+"/docs/Skyrim_Behavior_Tool_Documentation.html");
    if (!QDesktopServices::openUrl(url)){
        WARNING_MESSAGE("MainWindow::viewDocumentation(): Documentation not found!");
    }
}

void MainWindow::sendGIBS(){
    if (!QDesktopServices::openUrl(QUrl("https://www.nexusmods.com/Core/Libs/Common/Widgets/PayPalPopUp?user=4959835"))){
        WARNING_MESSAGE("MainWindow::sendGIBS(): Cannot send GIBS!");
    }
}

void MainWindow::findGenerator(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index >= 0 && index < behaviorGraphs.size()){
            (objectDataWid) ? objectDataWid->connectToGeneratorTable() : NULL;
        }else{
            if (tabs->count() < 2){
                WARNING_MESSAGE("No behavior file is currently being viewed!!!")
            }else{
                WARNING_MESSAGE("MainWindow::findGenerator() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::findGenerator(): No project opened!");
    }
}

void MainWindow::findModifier(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index >= 0 && index < behaviorGraphs.size()){
            (objectDataWid) ? objectDataWid->connectToModifierTable() : NULL;
        }else{
            if (tabs->count() < 2){
                WARNING_MESSAGE("No behavior file is currently being viewed!!!")
            }else{
                WARNING_MESSAGE("MainWindow::findModifier() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::findModifier(): No project opened!");
    }
}

BehaviorFile * MainWindow::openBehaviorForMerger(QString & filepath){
    BehaviorFile *ptr = nullptr;
    auto count = 1;
    if (filepath != "" && hkxcmd(filepath, filepath, count, "") == HKXCMD_SUCCESS){
        {
            QFile file(QString(filepath).replace(".hkx", "-out.hkx"));
            if (file.exists() && file.remove()){
                if (!file.rename(filepath)){
                    WARNING_MESSAGE("The attempt to rename "+filepath+" failed!");
                }
            }else{
                LogFile::writeToLog("MainWindow::openBehaviorForMerger(): Failed to remove the behavior file \""+filepath+"!");
            }
        }
        ptr = new BehaviorFile(this, nullptr, nullptr, filepath);
        if (!ptr->parse()){
            LogFile::writeToLog("MainWindow::openBehaviorForMerger(): The selected behavior file \""+filepath+"\" was not parsed!");
        }
    }else{
        LogFile::writeToLog("MainWindow::openBehaviorForMerger(): Failed to convert the behavior file \""+filepath+"!");
    }
    return ptr;
}

void MainWindow::exit(){
    /*if (exitProgram()) {
        writeSettings();
    }*/
    QApplication::exit(0);
}

void MainWindow::closeTab(int index){
    objectDataWid->unloadDataWidget();
    if (projectFile){
        if (!index && projectFile->behaviorFiles.isEmpty()){
            closeAll();
        }else{
            (--index >= 0 && index < behaviorGraphs.size()) ? tabs->removeTab(index + 1) : LogFile::writeToLog("MainWindow::closeTab(): The tab index is out of sync with the behavior files or behavior graphs!");
        }
    }else{
        LogFile::writeToLog("No project open!");
    }
}

QString MainWindow::generateUniqueBehaviorName(){
    auto name = QString("Behavior_0.hkx");
    auto num = 1;
    QStringList behaviornames;
    QDirIterator it(lastFileSelectedPath+"/"+projectFile->getBehaviorDirectoryName());
    while (it.hasNext()){
        if (QFileInfo(it.next()).fileName().contains(".hkx")){
            behaviornames.append(it.fileInfo().fileName());
        }
    }
    for (auto i = 0; i < behaviornames.size(); i++){
        if (behaviornames.at(i) == name){
            auto index = name.lastIndexOf('_');
            (index > -1) ? name.remove(++index, name.size()) : NULL;
            name.append(QString::number(num)+".hkx");
            (++num > 1) ? i = 0 : NULL;
        }
    }
    return lastFileSelectedPath+"/"+projectFile->getBehaviorDirectoryName()+"/"+name;
}

void MainWindow::addNewBehavior(bool initData){
    //ProgressDialog dialog("Creating new behavior file in the current project...", "", 0, 100, this);
    objectDataWid->unloadDataWidget();
    if (projectFile){
        if (projectFile->character){
            bool ok;
            auto filename = QInputDialog::getText(this, "Add A New Behavior File To The Current Project!", "Behavior Name:", QLineEdit::Normal, "NEW_BEHAVIOR", &ok);
            if (ok && !filename.isEmpty() && !filename.contains(QRegExp("^[a-z-A-Z\\]+$"))){
                if (filename != ""){
                    projectFile->behaviorFiles.append(new BehaviorFile(this, projectFile, projectFile->character, filename));
                    (initData) ? projectFile->behaviorFiles.last()->generateDefaultCharacterData() : projectFile->behaviorFiles.last()->generateNewBehavior();
                    //dialog.setProgress("Drawing behavior graph...", 50);
                    behaviorGraphs.append(new BehaviorGraphView(objectDataWid, projectFile->behaviorFiles.last()));
                    tabs->addTab(behaviorGraphs.last(), filename.section("/", -1, -1));
                    if (!behaviorGraphs.last()->drawGraph(static_cast<DataIconManager *>(projectFile->behaviorFiles.last()->getBehaviorGraph()), false)){
                        LogFile::writeToLog("MainWindow::addNewBehavior(): The behavior graph was drawn incorrectly!");
                    }
                    tabs->setCurrentIndex(tabs->count() - 1);
                    //dialog.setProgress("Writing to file...", 50);
                    //projectFile->behaviorFiles.last()->setIsChanged(true);
                    save();
                    //???
                    //projectFile->behaviorFiles.last()->toggleChanged(true);
                    //saveFile(tabs->count() - 2);
                    //dialog.setProgress("Behavior file creation sucessful!!", dialog.maximum());
                    return;
                }else{
                    LogFile::writeToLog("Name generation failed!");
                }
            }else{
                WARNING_MESSAGE("Invalid filename!!!");
            }
        }else{
            LogFile::writeToLog("No character file open!");
        }
    }else{
        LogFile::writeToLog("No project open!");
    }
    //dialog.setProgress("Behavior file creation failed!!", dialog.maximum());
}

void MainWindow::openProject(QString & filepath, bool loadui, bool loadanimdata, bool isFNIS, bool xml){
    auto cleanup = [&](){
        if (projectFile){
            delete projectFile;
            projectFile = nullptr;
        }
    };
    QString behavior;
    QStringList behaviornames;
    auto percent = 20;
    QTime timer;
    if (tabs->count() > 0){
        closeAll();
    }
    timer.start();
    auto timeelapsed = timer.elapsed();
    lastFileSelected = filepath;
    objectDataWid->changeCurrentDataWidget(nullptr);
    lastFileSelectedPath = filepath.section("/", 0, -2);
    projectFile = new ProjectFile(this, lastFileSelected);
    ProgressDialog progress("Reading animation data..."+projectFile->getFileName(), "", percent, 100, this);
    if (loadanimdata){
        QString path1;
        QString path2;
        if (isFNIS){
            path1 = lastFileSelectedPath.section("/", 0, -3)+"/animationdatasinglefile.txt";
            path2 = lastFileSelectedPath.section("/", 0, -3)+"/animationsetdatasinglefile.txt";
        }else{
            path1 = lastFileSelectedPath+"/animationdatasinglefile.txt";
            path2 = lastFileSelectedPath+"/animationsetdatasinglefile.txt";
        }
        std::thread thread(&ProjectFile::readAnimationSetData, projectFile, path2);
        if (!projectFile->readAnimationData(path1)){
            LogFile::writeToLog("MainWindow::openProject(): The project animation data file could not be parsed!!!");
        }
        thread.join();
    }
    progress.setProgress("Loading project file...", 50);
    if (projectFile->parse()){
        LogFile::writeToLog("Time taken to open file \""+filepath+"\" is approximately "+QString::number(timer.elapsed() - timeelapsed)+" milliseconds\n");
        progress.setProgress("Loading character data...", 50);
        timeelapsed = timer.elapsed();
		std::string current_locale_text = lastFileSelectedPath.toLocal8Bit().constData();
		fs::path character_path = current_locale_text;
		character_path /= projectFile->getCharacterFilePathAt(0).toLocal8Bit().constData();
		if (xml)
			character_path.replace_extension(".xml");
        auto character = new CharacterFile(this, projectFile, QString::fromStdString(character_path.string()));
        if (character->parse()){
            projectFile->setCharacterFile(character);
            projectFile->loadEncryptedAnimationNames();
            LogFile::writeToLog("Time taken to open file \""+ QString::fromStdString(character_path.string()) +"\" is approximately "+QString::number(timer.elapsed() - timeelapsed)+" milliseconds\n");
            timeelapsed = timer.elapsed();
            progress.setProgress("Loading skeleton data...", 50);
			fs::path skeleton_path = lastFileSelectedPath.toLocal8Bit().constData();
			skeleton_path /= character->getRigName().toLocal8Bit().constData();
			if (xml)
				skeleton_path.replace_extension(".xml");
            skeletonFile = new SkeletonFile(this, QString::fromStdString(skeleton_path.string()));
            if (skeletonFile->parse() || isFNIS){
                character->setSkeletonFile(skeletonFile);
                if (loadui){
                    projectUI->setFilePath(lastFileSelectedPath);
                    tabs->addTab(projectUI, "Character Data");
                    projectUI->setProject(projectFile);
                    projectUI->loadData();
                    projectUI->setDisabled(false);
                }
                LogFile::writeToLog("Time taken to open file \""+ QString::fromStdString(skeleton_path.string()) +"\" is approximately "+QString::number(timer.elapsed() - timeelapsed)+" milliseconds\n");
                //Get behavior filenames...
                QDirIterator it(lastFileSelectedPath+"/behaviors"); //Wont work for dogs, wolves!!!
                while (it.hasNext()){
                    behavior = it.next();
                    (behavior.contains(".hkx")) ? behaviornames.append(behavior) : NULL;
					(behavior.contains(".xml")) ? behaviornames.append(behavior) : NULL;
                }
                progress.setProgress("Loading behavior files...", 0);
                //This also reads files that may not belong to the current project! See dog!!
                std::vector <std::future<bool>> futures;
                auto maxThreads = std::thread::hardware_concurrency() - 1;
                auto taskCount = behaviornames.size();
                auto previousCount = taskCount;
                auto behaviorIndex = 0;
                auto taskdifference = 0;
                percent = 0;
                qreal difference = ((1.0)/((qreal)(behaviornames.size())))*(100.0);
                std::unique_lock<std::mutex> locker(mutex);
                //Start reading behavior files...
                for (uint i = 0; i < maxThreads, behaviorIndex < behaviornames.size(); i++, behaviorIndex++){
                    futures.push_back(std::async(std::launch::async, &MainWindow::openBehavior, this, behaviornames.at(behaviorIndex), std::ref(taskCount), false));
                }
                while (taskCount > 0){
                    if (taskCount < behaviornames.size()){
                        percent += difference;
                        progress.setValue(percent);
                    }
                    taskdifference = previousCount - taskCount;
                    previousCount = taskCount;
                    for (; taskdifference > 0; taskdifference--){
                        if (behaviorIndex < behaviornames.size()){
                            futures.push_back(std::async(std::launch::async, &MainWindow::openBehavior, this, behaviornames.at(behaviorIndex), std::ref(taskCount), false));
                            behaviorIndex++;
                        }
                    }
                    conditionVar.wait(locker, [&](){return (taskCount < previousCount);});
                }
                for (auto i = 0; i < futures.size(); i++){
                    if (!futures.at(i).get()){
                        LogFile::writeToLog("Open project failed! \""+behaviornames.at(i)+"\" failed to parse!");
                        cleanup();
                        return;
                    }
                }
                futures.clear();
                if (loadui){
                    percent = 0;
                    progress.setProgress("Loading behavior graphs...", percent);
                    for (auto i = 0; i < projectFile->behaviorFiles.size(); i++){
                        behaviorGraphs.append(new BehaviorGraphView(objectDataWid, projectFile->behaviorFiles.at(i)));
                    }
                    taskCount = behaviornames.size();
                    previousCount = taskCount;
                    behaviorIndex = 0;
                    taskdifference = 0;
                    difference = ((1.0)/((qreal)(behaviornames.size())))*(100.0);
                    for (auto i = 0; i < projectFile->behaviorFiles.size(); i++){
                        behaviorGraphs.at(i)->drawGraph(static_cast<DataIconManager *>(projectFile->behaviorFiles.at(i)->getBehaviorGraph()), false);
                        percent += difference;
                        progress.setValue(percent);
                    }
                    //locker.lock();
                    /*for (uint i = 0; i < maxThreads, behaviorIndex < behaviornames.size(); i++, behaviorIndex++){
                        threads.push_back(std::thread(&TreeGraphicsView::drawGraphMT, behaviorGraphs[behaviorIndex], static_cast<DataIconManager *>(projectFile->behaviorFiles.at(behaviorIndex)->getBehaviorGraph()), false, std::ref(taskCount), std::ref(mutex), std::ref(conditionVar)));
                        threads.back().detach();
                    }
                    while (taskCount > 0){
                        if (taskCount < behaviornames.size()){
                            percent += difference;
                            progress.setValue(percent);
                        }
                        taskdifference = previousCount - taskCount;
                        previousCount = taskCount;
                        for (; taskdifference > 0; taskdifference--){
                            if (behaviorIndex < behaviornames.size()){
                                threads.push_back(std::thread(&TreeGraphicsView::drawGraphMT, behaviorGraphs[behaviorIndex], static_cast<DataIconManager *>(projectFile->behaviorFiles.at(behaviorIndex)->getBehaviorGraph()), false, std::ref(taskCount), std::ref(mutex), std::ref(conditionVar)));
                                threads.back().detach();
                                behaviorIndex++;
                            }
                        }
                        conditionVar.wait(locker, [&](){return (taskCount < previousCount);});
                    }*/
                    futures.clear();
                }
                //projectFile->generateAnimClipDataForProject();
                (loadui && !behaviornames.isEmpty()) ? openBehaviorFile(behaviornames.first()) : NULL;
                progress.setProgress("Project loaded sucessfully!!!", progress.maximum());
            }else{
                LogFile::writeToLog(QString("MainWindow::openProject(): The skeleton file "+character->getRigName()+" could not be parsed!!!").toLocal8Bit().data());
                cleanup();
            }
            if (loadui){
                auto errors = projectFile->detectErrorsInProject();
                LogFile::writeToLog("Time taken to open project \""+filepath+"\" is approximately "+QString::number(timer.elapsed())+" milliseconds");
                for (auto i = 0; i < projectFile->behaviorFiles.size(); i++){
                    LogFile::writeToLog(projectFile->behaviorFiles.at(i)->getErrors());
                }
                if (errors != ""){
                    USER_MESSAGE(errors);
                }
            }
        }else{
            LogFile::writeToLog(QString("MainWindow::openProject(): The character file "+projectFile->getCharacterFilePathAt(0)+" could not be parsed!!!").toLocal8Bit().data());
            cleanup();
        }
    }else{
        LogFile::writeToLog(QString("MainWindow::openProject(): The project file "+lastFileSelected+" could not be parsed!!!\nYou have tried to open non-project file or the project file is corrupted!").toLocal8Bit().data());
        cleanup();
    }
}

bool MainWindow::openBehavior(const QString & filename, int & taskCount, bool checkisopen){
    auto result = false;
    if (filename != ""){
        if (projectFile){
            if (checkisopen){
                for (auto i = 0; i < projectFile->behaviorFiles.size(); i++){
                    if (projectFile->behaviorFiles.at(i)->fileName() == filename){
                        LogFile::writeToLog("MainWindow::openBehavior(): The selected behavior file \""+filename+"\" is already open!");
                        mutex.lock();
                        taskCount--;
                        conditionVar.notify_one();
                        mutex.unlock();
                        return true;
                    }
                }
            }else{
                mutex.lock();
                objectDataWid->changeCurrentDataWidget(nullptr);
                mutex.unlock();
            }
            auto newbehavior = new BehaviorFile(this, projectFile, projectFile->character, filename);
            mutex.lock();
            projectFile->behaviorFiles.append(newbehavior);
            newbehavior = projectFile->behaviorFiles.last();
            mutex.unlock();
            (!newbehavior->parse()) ? LogFile::writeToLog("MainWindow::openBehavior(): The selected behavior file \""+filename+"\" failed to parse!") : result = true;
        }else{
            LogFile::writeToLog("MainWindow::openBehavior(): No project is opened!");
        }
    }else{
        LogFile::writeToLog("MainWindow::openBehavior(): Filename is null string!");
    }
    mutex.lock();
    taskCount--;
    conditionVar.notify_one();
    mutex.unlock();
    return result;
}

bool MainWindow::closeAll(){
    if (projectFile){
        auto unsavedChanges = false;
        for (auto i = 0; i < behaviorGraphs.size(); i++){
            (projectFile->behaviorFiles.at(i)->getIsChanged()) ? unsavedChanges = true : NULL;
        }
        (projectFile->character->getIsChanged()) ? unsavedChanges = true : NULL;
        if (!unsavedChanges || closeAllDialogue() != QMessageBox::Cancel){
            projectUI->setDisabled(true);
            objectDataWid->unloadDataWidget();
            tabs->clear();
            for (auto j = 0; j < behaviorGraphs.size(); j++){
                (behaviorGraphs.at(j)) ? delete behaviorGraphs.at(j) : NULL;
            }
            behaviorGraphs.clear();
            (projectFile) ? delete projectFile, projectFile = nullptr : NULL;
            return true;
        }else{
            return false;
        }
    }else{
        return true;
    }
}

void MainWindow::removeBehaviorGraphs(const QStringList & filenames){
    for (auto i = 0; i < behaviorGraphs.size(); i++){
        for (auto j = 0; j < filenames.size(); j++){
            if (behaviorGraphs.at(i)->behavior->fileName() == filenames.at(j)){
                behaviorGraphs.at(i)->deleteAllObjectBranches();
                delete behaviorGraphs.at(i);
                behaviorGraphs.removeAt(i);
                projectFile->deleteBehaviorFile(filenames.at(j));
                i = 0;
                j = filenames.size();
            }
        }
    }
}

void MainWindow::openPackedProject(){
    auto filename = QFileDialog::getOpenFileName(this, tr("Open hkx project file..."), lastFileSelected, tr("hkx Files (*.hkx)"));
    (filename != "") ? convertProject(filename), openProject(filename) : LogFile::writeToLog("MainWindow::openPackedProject(): Null string filename!!!");
}

void MainWindow::openUnpackedProject(){
    auto filename = QFileDialog::getOpenFileName(this, tr("Open hkx xml project file..."), lastFileSelected, tr("xml Files (*.xml)"));
    (filename != "") ? openProject(filename) : LogFile::writeToLog("MainWindow::openUnpackedProject(): Null string filename!!!");
}

void MainWindow::openBehaviorFile(const QModelIndex & index){
    auto fileName = index.data().toString();
    for (auto j = 0; j < behaviorGraphs.size(); j++){
        if (!fileName.compare(behaviorGraphs.at(j)->getBehaviorFilename().section("/", -1, -1), Qt::CaseInsensitive)){
            objectDataWid->loadBehaviorView(behaviorGraphs.at(j));
            tabs->addTab(behaviorGraphs.at(j), fileName);
            tabs->setCurrentIndex(tabs->count() - 1);
            return;
        }
    }
    LogFile::writeToLog("MainWindow::openBehaviorFile(): The selected behavior file was not found!");
}

void MainWindow::openBehaviorFile(const QString & fileName){
    auto name = fileName.section("/", -1, -1);
    for (auto j = 0; j < behaviorGraphs.size(); j++){
        if (!name.compare(behaviorGraphs.at(j)->getBehaviorFilename().section("/", -1, -1), Qt::CaseInsensitive)){
            objectDataWid->loadBehaviorView(behaviorGraphs.at(j));
            tabs->addTab(behaviorGraphs.at(j), name);
            tabs->setCurrentIndex(tabs->count() - 1);
            return;
        }
    }
    LogFile::writeToLog("MainWindow::openBehaviorFile(): The selected behavior file was not found!");
}

void MainWindow::openAnimationFile(const QString &animationname){
    auto count = 1;
    if (hkxcmd(animationname, animationname, count, "-v:xml") == HKXCMD_SUCCESS){
        auto ptr = new AnimationFile(this, QString(animationname).replace(".hkx", "-out.hkx"));
        if (ptr->parse()){
            projectFile->setAnimationIndexDuration(-1, projectFile->character->getNumberOfAnimations() - 1, ptr->getDuration());
            projectFile->addEncryptedAnimationName(animationname);
            (!ptr->remove()) ? LogFile::writeToLog("MainWindow::openAnimationFile: Failed to remove the duplicate animation file!") : NULL;
            delete ptr;
        }else{
            LogFile::writeToLog("MainWindow::openAnimationFile(): The selected animation file was not opened!");
        }
    }else{
        LogFile::writeToLog("MainWindow::openAnimationFile: Failed to convert the animation file!");
    }
}

void MainWindow::removeAnimation(int index){
    (projectFile) ? projectFile->removeEncryptedAnimationName(index) : LogFile::writeToLog("MainWindow::removeAnimation: projectFile is null!");
}

void MainWindow::zoomIn(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index >= 0 && index < projectFile->behaviorFiles.size() && index < behaviorGraphs.size()){
            behaviorGraphs.at(index)->zoom(zoomFactor);
        }else{
            if (tabs->count() < 2){
                LogFile::writeToLog("No behavior file is currently being viewed!!!");
            }else{
                WARNING_MESSAGE("MainWindow::zoomIn() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::zoomIn(): No project opened!");
    }
}

void MainWindow::zoomOut(){
    if (projectFile){
        auto index = tabs->currentIndex() - 1;
        if (index >= 0 && index < projectFile->behaviorFiles.size() && index < behaviorGraphs.size()){
            behaviorGraphs.at(index)->zoom(-zoomFactor);
        }else{
            if (tabs->count() < 2){
                LogFile::writeToLog("No behavior file is currently being viewed!!!");
            }else{
                WARNING_MESSAGE("MainWindow::zoomIn() failed!\nThe tab index is out of sync with the behavior files or behavior graphs!");
            }
        }
    }else{
        LogFile::writeToLog("MainWindow::zoomIn(): No project opened!");
    }
}

int MainWindow::getBehaviorGraphIndex(const QString & filename){
    for (auto j = 0; j < behaviorGraphs.size(); j++){
        if (!filename.compare(behaviorGraphs.at(j)->getBehaviorFilename().section("/", -1, -1), Qt::CaseInsensitive)){
            if (j >= projectFile->behaviorFiles.size() || filename.compare(projectFile->behaviorFiles.at(j)->getFileName(), Qt::CaseInsensitive) != 0){
                LogFile::writeToLog("MainWindow::getBehaviorGraphIndex(): The index is invalid!");
                return -1;
            }
            return j;
        }
    }
    return -1;
}

void MainWindow::readSettings(){
    QSettings settings("QtProject", "Skyrim Behavior Editor");
    settings.beginGroup("Main Window");
    auto pos = settings.value("pos", QPoint(500, 500)).toPoint();
    auto size = settings.value("size", QSize(900, 900)).toSize();
    resize(size);
    move(pos);
    settings.endGroup();
}

void MainWindow::writeSettings(){
    QSettings settings("QtProject", "Skyrim Behavior Editor");
    settings.beginGroup("Main Window");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();
}

bool MainWindow::findGameDirectory(const QString & gamename, QString & gamedirectory){
    auto drives = QDir::drives();
    bool value = false;
    for (auto i = 0; i < drives.size(); i++){
        auto driveName = drives.at(i).absolutePath();
        auto dir = QDir(driveName);
        auto path = driveName+"Program Files/Steam/steamapps/common/"+gamename;
        if (dir.exists(path)){
            gamedirectory = path;
            value = true;
        }else{
            path = driveName+"Steam/steamapps/common/"+gamename;
            if (dir.exists(path)){
                gamedirectory = path;
                value = true;
            }else{
                path = driveName+"Steam Games/steamapps/common/"+gamename;
                if (dir.exists(path)){
                    gamedirectory = path;
                    value = true;
                }
            }
        }
    }
    return value;
}

bool MainWindow::convertProject(const QString &filepath, const QString &newpath, const QString &flags){
    QStringList pathtoallfiles;
    QStringList filelist;
    {
        QDirIterator it(filepath.section("/", 0, -2), QDirIterator::Subdirectories);
        while (it.hasNext()){
            auto temp = it.next();
            if (temp.contains(".hkx") && !temp.contains("/animations/", Qt::CaseInsensitive) && !temp.contains("/_1stperson/", Qt::CaseInsensitive)){
                filelist.append(temp);
            }
        }
    }
    if (!filelist.isEmpty()){
        if (newpath != ""){
            for (auto i = 0; i < filelist.size(); i++){
                if (filelist.at(i).contains("/behaviors", Qt::CaseInsensitive) || filelist.at(i).contains("/animations", Qt::CaseInsensitive) || filelist.at(i).contains("/character assets", Qt::CaseInsensitive)
                        || (filelist.at(i).contains("/character", Qt::CaseInsensitive) && filelist.at(i).count("/character") == 2))
                {
                    pathtoallfiles.append(newpath+"/"+filelist.at(i).section("/", -2, -1));
                }else{
                    pathtoallfiles.append(newpath+"/"+filelist.at(i).section("/", -1, -1));
                }
            }
        }else{
            for (auto i = 0; i < filelist.size(); i++){
                pathtoallfiles.append("");
            }
        }
        QProgressDialog progress("Converting packed hkx binaries to hkx xml...", "", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        std::vector <std::thread> threads;
        auto percent = 0;
        auto taskCount = filelist.size();
        auto previousCount = taskCount;
        auto fileIndex = 0;
        auto maxThreads = std::thread::hardware_concurrency() - 1;
        auto taskdifference = 0;
        qreal difference = ((1.0)/((qreal)(filelist.size())))*(100.0);
        std::unique_lock<std::mutex> locker(mutex);
        //Read files...
        for (uint i = 0; i < maxThreads, fileIndex < filelist.size(); i++, fileIndex++){
            threads.push_back(std::thread(&MainWindow::hkxcmd, this, filelist.at(fileIndex), pathtoallfiles.at(fileIndex), std::ref(taskCount), flags));
            threads.back().detach();
        }
        while (taskCount > 0){
            if (taskCount < filelist.size()){
                percent += difference;
                progress.setValue(percent);
            }
            taskdifference = previousCount - taskCount;
            previousCount = taskCount;
            for (; taskdifference > 0; taskdifference--){
                if (fileIndex < filelist.size()){
                    threads.push_back(std::thread(&MainWindow::hkxcmd, this, filelist.at(fileIndex), pathtoallfiles.at(fileIndex), std::ref(taskCount), flags));
                    threads.back().detach();
                    fileIndex++;
                }
            }
            conditionVar.wait(locker, [&](){return (taskCount < previousCount);});
        }
        threads.clear();
        if (newpath == ""){
            progress.setLabelText("Renaming converted files, cleaning up extras...");
            progress.setValue(10);
            //remove all hkx files then rename the rest to hkx files...
            {
                QDirIterator it(filepath.section("/", 0, -2), QDirIterator::Subdirectories);
                while (it.hasNext()){
                    auto temp = it.next();
                    if (temp.contains(".hkx") && !temp.contains("/animations/", Qt::CaseInsensitive) && !temp.contains("/_1stperson/", Qt::CaseInsensitive)){
                        if (!QDir(temp).remove(temp)){
                            LogFile::writeToLog(QString("MainWindow::convertProject(): The file "+temp+" could not be removed!!!").toLocal8Bit().data());
                        }
                    }
                }
            }
            progress.setValue(50);
            {
                QDirIterator it(filepath.section("/", 0, -2), QDirIterator::Subdirectories);
                while (it.hasNext()){
                    auto temp = it.next();
                    if (temp.contains("-out.xml")){
                        if (!QDir(temp).rename(temp, QString(temp).replace("-out.xml", ".hkx"))){
                            LogFile::writeToLog(QString("MainWindow::convertProject(): The file "+temp+" could not be renamed!!!").toLocal8Bit().data());
                        }
                    }else if (temp.contains(".xml")){
                        if (!QDir(temp).rename(temp, QString(temp).replace(".xml", ".hkx"))){
                            LogFile::writeToLog(QString("MainWindow::convertProject(): The file "+temp+" could not be renamed!!!").toLocal8Bit().data());
                        }
                    }
                }
            }
        }
        progress.setValue(100);
    }else{
        return false;
    }
    return true;
}

MainWindow::HKXCMD_RETURN MainWindow::hkxcmd(const QString &filepath, const QString &outputDirectory, int & taskcount, const QString &flags){
    auto command = "\""+hkxcmdPath+"\" convert \""+filepath+"\" \""+outputDirectory+"\" "+flags;
    command.replace("/", "\\");
    auto value = (HKXCMD_RETURN)QProcess().execute(command);
    if (value == HKXCMD_SUCCESS){
        mutex.lock();
        taskcount--;
        conditionVar.notify_one();
        mutex.unlock();
    }else{
        LogFile::writeToLog("MainWindow: hkxcmd() failed!\nThe command \""+command+"\" failed!");
    }
    return value;
}

bool MainWindow::exitProgram(){
    if (closeAll()){
        auto ret = QMessageBox::warning(this, tr("Application"), "Close the application?", QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok){
            return true;
        }else if (ret == QMessageBox::Cancel){
            return false;
        }
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event){
    writeSettings();
    (exitProgram()) ? writeSettings(), event->accept() : event->ignore();
}

void MainWindow::paintEvent(QPaintEvent *){
    QStyleOption opt;
    QPainter p(this);
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::createNewProject(){
    /*
     * (1): select skeleton
     * (2): name project
     * (3): generate project
     * (4): check if name is availible
     * (5): search directory for animation text files
     * (6): if none use from tool directory
     * (7): set up directory
     * (8): generate character and root behavior file
     * (9): connect to skeleton
     * (10): reset selected files to project file
     * (11): generate new project in animation text files
     * (12): Load UI
*/
    if (!projectFile || (projectFile && closeAll())){
        auto filename = QFileDialog::getOpenFileName(this, tr("Select hkx skeleton file..."), QDir::currentPath(), tr("hkx Files (*.hkx)"));
        if (filename != ""){
            objectDataWid->changeCurrentDataWidget(nullptr);
            lastFileSelected = filename;
            lastFileSelectedPath = filename.section("/", 0, -2);
            skeletonFile = new SkeletonFile(this, filename);
            if (!skeletonFile->parse()){
                LogFile::writeToLog(QString("MainWindow::createNewProject(): The skeleton file "+filename.section("/", -1, -1)+" could not be parsed!!!").toLocal8Bit().data());
            }
            bool ok;
            auto projectname = QInputDialog::getText(this, tr("Set project name!"), tr("Project name:"), QLineEdit::Normal, "", &ok);
            if (ok && projectname != "" && !projectname.contains(QRegExp("^[a-z-A-Z\\]+$"))){
                auto projectDirectoryPath = lastFileSelectedPath+"/"+projectname;
                {
                    QDir projectDirectory(lastFileSelectedPath);
                    if (!projectDirectory.mkdir(projectDirectoryPath) && !QDir(projectDirectoryPath).exists()){
                        LogFile::writeToLog(QString("MainWindow::createNewProject(): Attempt to create '"+projectDirectoryPath+"' directory failed!!!").toUtf8().data());
                    }
                }
                auto relativecharacterpath = "characters/"+projectname+"character.hkx";
                QDir projectDirectory(projectDirectoryPath);
                projectUI->setFilePath(projectDirectoryPath);
                if (projectDirectory.exists()){
                    QStringList list = {"animations", "behaviors", "character assets", "characters"};
                    for (auto i = 0; i < list.size(); i++){
                        if (!projectDirectory.mkdir(list.at(i)) && !QDir(projectDirectoryPath+"/"+list.at(i)).exists()){
                            LogFile::writeToLog(QString("MainWindow::createNewProject(): Attempt to create '"+list.at(i)+"' directory failed!!!").toUtf8().data());
                        }
                    }
                    if (!projectDirectory.rename(lastFileSelected, projectDirectoryPath+"/character assets/"+skeletonFile->getFileName())){
                        LogFile::writeToLog("MainWindow::createNewProject(): Attempt to move skeleton file failed!!!");
                    }
                    projectFile = new ProjectFile(this, projectDirectoryPath+"/"+projectname+".hkx", true, "characters\\"+projectname+"character.hkx");
                    {
                        std::thread thread(&ProjectFile::readAnimationSetData, projectFile, lastFileSelectedPath+"/animationsetdatasinglefile.txt");
                        if (!projectFile->readAnimationData(lastFileSelectedPath+"/animationdatasinglefile.txt", QStringList("Behaviors\\Master.hkx"))){
                            LogFile::writeToLog("MainWindow::createNewProject(): The project animation data file could not be parsed!!!");
                        }
                        thread.join();
                    }
                    if (!projectFile->isProjectNameTaken()){
                        lastFileSelected = projectFile->fileName();
                        lastFileSelectedPath = projectFile->fileName().section("/", 0, -2);
                        projectFile->character = new CharacterFile(this, projectFile, projectDirectoryPath+"/"+relativecharacterpath, true, "character assets\\"+skeletonFile->getFileName());
                        projectFile->setCharacterFile(projectFile->character);
                        projectFile->character->setSkeletonFile(skeletonFile);
                        projectFile->behaviorFiles.append(new BehaviorFile(this, projectFile, projectFile->character, projectDirectoryPath+"/behaviors/Master.hkx"));
                        projectFile->behaviorFiles.last()->generateDefaultCharacterData();
                        projectFile->addProjectToAnimData();
                        projectUI->setProject(projectFile);
                        projectUI->loadData();
                        projectUI->setDisabled(false);
                        behaviorGraphs.append(new BehaviorGraphView(objectDataWid, projectFile->behaviorFiles.last()));
                        tabs->addTab(projectUI, "Character Data");
                        tabs->addTab(behaviorGraphs.last(), projectFile->behaviorFiles.last()->getFileName());
                        if (!behaviorGraphs.last()->drawGraph(static_cast<DataIconManager *>(projectFile->behaviorFiles.last()->getBehaviorGraph()), false)){
                            LogFile::writeToLog("MainWindow::createNewProject(): The behavior graph was drawn incorrectly!");
                        }
                        tabs->setCurrentIndex(tabs->count() - 1);
                        saveProject(true);
                    }else{
                        closeAll();
                        WARNING_MESSAGE(QString("MainWindow::createNewProject(): The chosen project name is taken! Choose another project name!"));
                    }
                }else{
                    LogFile::writeToLog("MainWindow::createNewProject(): Invalid 'lastFileSelectedPath'!!!");
                }
            }else{
                closeAll();
                WARNING_MESSAGE(QString("MainWindow::createNewProject(): Invalid project name!"));
            }
        }
    }
}
