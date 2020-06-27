#include "widget.h"
#include "ui_widget.h"
#include <os.h>
#include <myw.h>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    OS *os = new OS;

    delete ui;
}

Widget::~Widget()
{

}
