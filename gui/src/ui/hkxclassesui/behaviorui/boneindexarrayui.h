#ifndef BONEINDEXARRAYUI_H
#define BONEINDEXARRAYUI_H

#include <QGroupBox>

#include "src/utility.h"


class BehaviorGraphView;
class GenericTableWidget;
class TableWidget;
class SpinBox;
class ComboBox;
class LineEdit;
class CheckBox;
class QGridLayout;


namespace UI {

class HkxObject;
class hkbBoneIndexArray;
class hkbVariableBindingSet;

class BoneIndexArrayUI final: public QGroupBox
{
    Q_OBJECT
public:
    BoneIndexArrayUI();
    BoneIndexArrayUI& operator=(const BoneIndexArrayUI&) = delete;
    BoneIndexArrayUI(const BoneIndexArrayUI &) = delete;
    ~BoneIndexArrayUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *ragdollBones);
signals:
    void returnToParent();
    void viewRagdollBones(int index);
private slots:
    void setRagdollBone(int index, const QString & name);
    void viewSelectedChild(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void removeRagdollBone(int index);
    void addRagdollBone();
    void loadDynamicTableRows();
    void setRowItems(int row, const QString & name, const QString & bind, const QString & value, const QString & tip1, const QString & tip2);
private:
    static const QStringList types;
    static const QStringList headerLabels;
    hkbBoneIndexArray *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    QPushButton *returnPB;
};
}
#endif // BONEINDEXARRAYUI_H
