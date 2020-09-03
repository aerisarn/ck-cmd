#ifndef CLIPGENERATORUI_H
#define CLIPGENERATORUI_H

#include <QStackedWidget>

#include "src/utility.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class SpinBox;

class QGroupBox;
class MainWindow;

namespace UI {

class HkxObject;
class hkbVariableBindingSet;
class ClipTriggerUI;

class ClipGeneratorUI final: public QStackedWidget
{
    Q_OBJECT
public:
    ClipGeneratorUI();
    ClipGeneratorUI& operator=(const ClipGeneratorUI&) = delete;
    ClipGeneratorUI(const ClipGeneratorUI &) = delete;
    ~ClipGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events, GenericTableWidget *animations);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewAnimations(const QString & name);
    void generatorNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString & newname);
    void setAnimationName(int index, const QString & name);
    void setCropStartAmountLocalTime();
    void setCropEndAmountLocalTime();
    void setStartTime();
    void setPlaybackSpeed();
    void setEnforcedDuration();
    void setUserControlledTimeFraction();
    void setAnimationBindingIndex();
    void setMode(int index);
    void setFlagContinueMotionAtEnd();
    void setFlagSyncHalfCycleInPingPongMode();
    void setFlagMirror();
    void setFlagForceDensePose();
    void setFlagDontConvertAnnotationsToTriggers();
    void setFlagIgnoreMotion();
    void viewSelectedChild(int row, int column);
    void setBindingVariable(int index, const QString & name);
    void returnToWidget();
private:
    void toggleSignals(bool toggleconnections);
    void addTrigger();
    void removeTrigger(int index);
    void loadDynamicTableRows();
    void selectTableToView(bool viewproperties, const QString & path);
    void setFlag(CheckBox *flagcheckbox, hkbClipGenerator::ClipFlag flagtoset);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        CHILD_WIDGET
    };
private:
    static const QStringList headerLabels;
    hkbClipGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QGroupBox *groupBox;
    ClipTriggerUI *triggerUI;
    LineEdit *name;
    DoubleSpinBox *cropStartAmountLocalTime;
    DoubleSpinBox *cropEndAmountLocalTime;
    DoubleSpinBox *startTime;
    DoubleSpinBox *playbackSpeed;
    DoubleSpinBox *enforcedDuration;
    DoubleSpinBox *userControlledTimeFraction;
    SpinBox *animationBindingIndex;
    ComboBox *mode;
    CheckBox *flagContinueMotionAtEnd;
    CheckBox *flagSyncHalfCycleInPingPongMode;
    CheckBox *flagMirror;
    CheckBox *flagForceDensePose;
    CheckBox *flagDontConvertAnnotationsToTriggers;
    CheckBox *flagIgnoreMotion;
};
}
#endif // CLIPGENERATORUI_H
