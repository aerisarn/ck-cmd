#ifndef HANDIKDRIVERINFOUI_H
#define HANDIKDRIVERINFOUI_H

#include <QStackedWidget>

class QuadVariableWidget;
class TableWidget;
class QGridLayout;
class DoubleSpinBox;
class CheckBox;
class QStackedLayout;
class SpinBox;
class QPushButton;
class QGroupBox;
class ComboBox;

namespace UI {

class HkxObject;
class hkbHandIkDriverInfo;
class HandIkDriverInfoHandUI;

class HandIkDriverInfoUI final: public QStackedWidget
{
    Q_OBJECT
public:
    HandIkDriverInfoUI();
    HandIkDriverInfoUI& operator=(const HandIkDriverInfoUI&) = delete;
    HandIkDriverInfoUI(const HandIkDriverInfoUI &) = delete;
    ~HandIkDriverInfoUI() = default;
public:
    void loadData(HkxObject *data);
    void loadBoneList(QStringList &bones);
private slots:
    void setFadeInOutCurve(int index);
    void addHand();
    void removeSelectedHand();
    void viewSelectedHand(int row, int column);
    void returnToWidget();
private:
    enum ACTIVE_WIDGET {
        HAND_IK_DRIVER_INFO,
        HAND_IK_DRIVER_INFO_HAND
    };
private:
    static const QStringList headerLabels1;
    hkbHandIkDriverInfo *bsData;
    QGroupBox *handDriverGB;
    HandIkDriverInfoHandUI *handUI;
    QGridLayout *footDriverLyt;
    QPushButton *addHandPB;
    QPushButton *removeHandPB;
    TableWidget *table;
    ComboBox *fadeInOutCurve;
};
}
#endif // HANDIKDRIVERINFOUI_H
