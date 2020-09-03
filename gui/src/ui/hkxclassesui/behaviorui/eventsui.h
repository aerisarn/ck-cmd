#ifndef EVENTSUI_H
#define EVENTSUI_H

#include <QGroupBox>


class QVBoxLayout;
class QHBoxLayout;
class TableWidget;
class QPushButton;
class QSignalMapper;
class QStackedLayout;
class LineEdit;
class CheckBox;

namespace UI {

class HkxObject;
class hkbBehaviorGraphData;
class HkDataUI;

class EventsUI final: public QGroupBox
{
    Q_OBJECT
public:
    EventsUI(const QString & title);
    EventsUI& operator=(const EventsUI&) = delete;
    EventsUI(const EventsUI &) = delete;
    ~EventsUI() = default;
public:
    void setHkDataUI(HkDataUI *ui);
    void loadData(HkxObject *data);
    void clear();
private slots:
    void addEvent();
    void removeEvent();
    void setBoolVariableValue();
    void renameSelectedEvent();
    void viewEvent(int row, int column);
    void returnToTable();
signals:
    void eventNameChanged(const QString & newName, int index);
    void eventAdded(const QString & name);
    void eventRemoved(int index);
private:
    enum View {
        TABLE_WIDGET,
        EVENT_WIDGET
    };
private:
    static const QStringList headerLabels;
    HkDataUI *dataUI;
    QVBoxLayout *verLyt;
    hkbBehaviorGraphData *loadedData;
    TableWidget *table;
    QPushButton *addObjectPB;
    QPushButton *removeObjectPB;
    QHBoxLayout *buttonLyt;
    LineEdit *eventName;
    CheckBox *flag;
    TableWidget *eventWidget;
    QStackedLayout *stackLyt;
    QPushButton *returnPB;
};
}
#endif // EVENTSUI_H
