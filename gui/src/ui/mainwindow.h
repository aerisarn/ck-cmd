#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QStringBuilder>
#include <QtWidgets>

#include "src/hkxclasses/hkxobject.h"

#include <thread>
#include <mutex>
#include <condition_variable>

//class FileSelectWindow;
class QVBoxLayout;
class QMenuBar;
class QMenu;
class QFile;
class QGraphicsScene;
class QGroupBox;
class QHBoxLayout;
class BehaviorGraphView;
class QPlainTextEdit;

class ProjectFile;
class BehaviorFile;
class CharacterFile;
class ProjectFile;
class SkeletonFile;


namespace UI {
	class HkDataUI;
	class BehaviorVariablesUI;
	class EventsUI;
	class ProjectUI;
}

class MainWindow final: public QWidget
{
    Q_OBJECT
public:
    static QMessageBox::StandardButton yesNoDialogue(const QString & message);
public:
    MainWindow();
    MainWindow& operator=(const MainWindow&) = delete;
    MainWindow(const MainWindow &other) = delete;
    ~MainWindow();
    void removeBehaviorGraphs(const QStringList & filenames);
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
private slots:
    void createNewProject();
    void openPackedProject();
    void openUnpackedProject();
    void openBehaviorFile(const QModelIndex & index);
    void openBehaviorFile(const QString & fileName);
    void openAnimationFile(const QString &animationname);
    void removeAnimation(int index);
    void zoomIn();
    void zoomOut();
    void expandBranches();
    void collapseBranches();
    void refocus();
    void setGameMode();
    void setPathToGameDirectory();
    void setProjectExportPath();
    void save();
    void saveProject(bool usenormalsave);
    void packAndExportProjectToSkyrimDirectory(bool exportanimdata = true);
    void exportAnimationData();
    void packAndExportFileToSkyrimDirectory(bool exportanimdata = true);
    void mergeBehaviors();
    void mergeProjects();
    void mergeFNIS();
    void viewDocumentation();
    void sendGIBS();
    void findGenerator();
    void findModifier();
    void exit();
    void changedTabs(int index);
    void closeTab(int index);
    void addNewBehavior(bool initData);
private:
    enum HKXCMD_RETURN{
        HKXCMD_SUCCESS
    };
    QString generateUniqueBehaviorName();
    BehaviorFile *openBehaviorForMerger(QString & filepath);
    void openProject(QString &filepath, bool loadui = true, bool loadanimdata = true, bool isFNIS = false, bool xml = true);
    void saveFile(int index, int &taskCount, bool usenormalsave = true);
    bool openBehavior(const QString & filename, int &taskCount, bool checkisopen = true);
    bool exitProgram();
    bool findGameDirectory(const QString &gamename, QString &gamedirectory);
    bool convertProject(const QString &filepath, const QString &newpath = "", const QString &flags = "-v:xml");
    HKXCMD_RETURN hkxcmd(const QString &filepath, const QString &outputDirectory, int &taskcount, const QString &flags = "-f SAVE_CONCISE");
    int getBehaviorGraphIndex(const QString & filename);
    void readSettings();
    void writeSettings();
    QMessageBox::StandardButton closeAllDialogue();
    QMessageBox::StandardButton closeFileDialogue();
    bool closeAll();
private:
    static const int zoomFactor = 120;
    QString hkxcmdPath;
    QString skyrimDirectory;
    QString skyrimSpecialEdtionDirectory;
    QString skyrimBehaviorUpdateToolFullPath;
    QString projectExportPath;
    QGridLayout *topLyt;
    QMenuBar *topMB;
    QMenu *fileM;
    QAction *newProjectA;
    QAction *openPackedProjectA;
    QAction *openUnpackedProjectA;
    QAction *saveA;
    QAction *exportToSkyrimDirA;
    QAction *exportCurrentFileA;
    QAction *saveProjectA;
    QAction *exitA;
    QMenu *viewM;
    QAction *zoomInA;
    QAction *zoomOutA;
    QAction *expandA;
    QAction *collapseA;
    QAction *refocusA;
    QAction *findGeneratorA;
    QAction *findModifierA;
    QMenu *mergeM;
    QAction *mergeBehaviorsA;
    QAction *mergeProjectsA;
    QAction *generateFNISPatchA;
    QMenu *settingsM;
    QAction *setPathToGameFolderA;
    QAction *setProjectExportPathA;
    QAction *setGameModeA;
    QMenu *documentationM;
    QAction *classReferenceHubA;
    QMenu *donateM;
    QAction *sendGibsA;
    QTabWidget *tabs;
    ProjectFile *projectFile;
    SkeletonFile *skeletonFile;
    QVector <BehaviorGraphView *> behaviorGraphs;
    UI::ProjectUI *projectUI;
    QGroupBox *behaviorGraphViewGB;
    QVBoxLayout *iconGBLyt;
    UI::HkDataUI *objectDataWid;
    QScrollArea *objectDataSA;
	UI::BehaviorVariablesUI *variablesWid;
	UI::EventsUI *eventsWid;
    QString lastFileSelected;
    QString lastFileSelectedPath;
    std::mutex mutex;
    std::condition_variable conditionVar;
	UI::LogFile logfile;
};

#endif // MAINWINDOW_H
