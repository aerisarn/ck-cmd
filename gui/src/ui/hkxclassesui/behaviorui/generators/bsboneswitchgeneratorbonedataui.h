#ifndef BSBONESWITCHGENERATORBONEDATAUI_H
#define BSBONESWITCHGENERATORBONEDATAUI_H

#include <QStackedWidget>

#include "src/utility.h"


class TableWidget;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class QPushButton;
class BehaviorGraphView;
class QGroupBox;
class QGridLayout;
class CheckButtonCombo;

class GenericTableWidget;

namespace UI {
	
class HkxObject;
class hkbVariableBindingSet;
class BSBoneSwitchGeneratorBoneData;
class BoneWeightArrayUI;

class BSBoneSwitchGeneratorBoneDataUI final: public QStackedWidget
{
    Q_OBJECT
    friend class BSBoneSwitchGeneratorUI;
public:
    BSBoneSwitchGeneratorBoneDataUI();
    BSBoneSwitchGeneratorBoneDataUI& operator=(const BSBoneSwitchGeneratorBoneDataUI&) = delete;
    BSBoneSwitchGeneratorBoneDataUI(const BSBoneSwitchGeneratorBoneDataUI &) = delete;
    ~BSBoneSwitchGeneratorBoneDataUI() = default;
public:
    void loadData(HkxObject *data, int childindex);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent(bool reloadData);
private slots:
    void viewSelected(int row, int column);
    void viewBoneWeights();
    void toggleBoneWeights(bool enable);
    void returnToWidget();
    void generatorTableElementSelected(int index, const QString &name);
    void variableTableElementSelected(int index, const QString &name);
private:
    void toggleSignals(bool toggleconnections);
    void setGenerator(int index, const QString & name);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        BONE_WEIGHT_ARRAY_WIDGET
    };
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSBoneSwitchGeneratorBoneData *bsData;
    int childIndex;
    QGridLayout *topLyt;
    QGroupBox *groupBox;
    QPushButton *returnPB;
    TableWidget *table;
    CheckButtonCombo *spBoneWeight;
    BoneWeightArrayUI *boneWeightArrayUI;
};
}
#endif // BSBONESWITCHGENERATORBONEDATAUI_H
