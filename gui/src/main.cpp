#include "src/ui/mainwindow.h"
#include <QApplication>

#include <QColor>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setStyleSheet("QWidget {background: blue;font-weight: bold; color: red}QComboBox {background: yellow}");
    w.setWindowTitle("Skyrim Behavior Tool");
    w.show();

    return a.exec();
}
