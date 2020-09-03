#ifndef PROJECTUI_H
#define PROJECTUI_H

#include <QGroupBox>

class QGridLayout;
class ProjectFile;
class QFileSystemModel;
class QListView;
class CheckBox;
class QPushButton;


namespace UI {
	
class CharacterPropertiesUI;
class SkeletonUI;
class FootIkDriverInfoUI;
class HandIkDriverInfoUI;
class AnimationsUI;
class AnimationCacheUI;

class ProjectUI final: public QGroupBox
{
    Q_OBJECT
public:
    ProjectUI(ProjectFile *file);
    ProjectUI& operator=(const ProjectUI&) = delete;
    ProjectUI(const ProjectUI &) = delete;
    ~ProjectUI() = default;
public:
    void loadData();
    void setProject(ProjectFile *file);
    void setFilePath(const QString & path);
    void setDisabled(bool disable);
    AnimationsUI *getAnimations() const;
signals:
    void openFile(const QModelIndex & index);
    void addBehavior(bool initData);
    void openAnimation(const QString & filename);
    void animationRemoved(int index);
private slots:
    void toggleFootIK(bool toggle);
    void toggleHandIK(bool toggle);
    void addNewBehaviorFile();
private:
    ProjectFile *project;
    QGridLayout *lyt;
    AnimationCacheUI *animationCacheUI;
    CharacterPropertiesUI *characterProperties;
    AnimationsUI *animations;
    SkeletonUI *skeleton;
    CheckBox *enableFootIKCB;
    CheckBox *enableHandIKCB;
    FootIkDriverInfoUI *footIK;
    HandIkDriverInfoUI *handIK;
    QFileSystemModel *fileSys;
    QListView *fileView;
    QPushButton *addBehaviorFile;
    CheckBox *initializeWithCharacterData;
    QString lastFileSelectedPath;
};
}
#endif // PROJECTUI_H
