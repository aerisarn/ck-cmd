#ifndef ANIMATIONSUI_H
#define ANIMATIONSUI_H

#include <QGroupBox>
//#include "src/animData/skyrimanimdata.h"


class QVBoxLayout;
class QHBoxLayout;
class TableWidget;
class QPushButton;
class QSignalMapper;
class CheckBox;
class QStackedLayout;
class ProjectAnimData;

namespace UI {

class HkxObject;
class hkbCharacterStringData;
class HkDataUI;
class SkyrimAnimationDataUI;

class AnimationsUI final: public QGroupBox
{
    Q_OBJECT
public:
    AnimationsUI(const QString & title);
    AnimationsUI& operator=(const AnimationsUI&) = delete;
    AnimationsUI(const AnimationsUI &) = delete;
    ~AnimationsUI() = default;
public:
    void setHkDataUI(HkDataUI *ui);
    void loadData(HkxObject *data, ProjectAnimData *animdata);
    void clear();
signals:
    void openAnimationFile(const QString & filename);
    void animationNameChanged(const QString & newName, int index);
    void animationAdded(const QString & name);
    void animationRemoved(int index);
private slots:
    void addAnimation();
    void removeAnimation();
    void renameSelectedAnimation();
    void viewAnimation(int row, int column);
    void returnToTable();
private:
    void toggleSignals(bool toggleconnections);
private:
    enum View {
        TABLE_WIDGET,
        ANIMATION_WIDGET
    };
private:
    static const QStringList headerLabels;
    HkDataUI *dataUI;
    QVBoxLayout *verLyt;
    ProjectAnimData *animData;
    hkbCharacterStringData *loadedData;
    TableWidget *table;
    QPushButton *addObjectPB;
    QHBoxLayout *buttonLyt;
    QPushButton *removeObjectPB;
    QStackedLayout *stackLyt;
    SkyrimAnimationDataUI *animationUI;
};
}
#endif // ANIMATIONSUI_H
