#ifndef BONEWEIGHTARRAYUI_H
#define BONEWEIGHTARRAYUI_H

#include <QGroupBox>


class QVBoxLayout;
class DoubleSpinBox;
class TableWidget;
class QPushButton;
class QGridLayout;
class QLabel;
class QCheckBox;

namespace UI {
	
class HkxObject;
class hkbBoneWeightArray;

class BoneWeightArrayUI final: public QGroupBox
{
    Q_OBJECT
public:
    BoneWeightArrayUI();
    BoneWeightArrayUI& operator=(const BoneWeightArrayUI&) = delete;
    BoneWeightArrayUI(const BoneWeightArrayUI &) = delete;
    ~BoneWeightArrayUI() = default;
public:
    void loadData(HkxObject *data, bool isRagdoll = false);
signals:
    void returnToParent();
private slots:
    void setBoneWeight();
    void loadBoneWeight(int row, int);
    void invert();
private:
    static const QStringList headerLabels;
    hkbBoneWeightArray *bsData;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *bones;
    QLabel *label;
    DoubleSpinBox *selectedBone;
    QCheckBox *setAllCB;
    QPushButton *invertAllPB;
};
}
#endif // BONEWEIGHTARRAYUI_H
