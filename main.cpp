#include "widget.h"
#include <QApplication>
#include <os.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    a.exec();
    OS *os = new OS;
    return 0;
}
