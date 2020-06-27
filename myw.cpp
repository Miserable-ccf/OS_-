#include "myw.h"
#include <qpushbutton.h>
#include <qdebug.h>
myw::myw(QWidget *parent) : QWidget(parent)
{
    QPushButton *btn = new QPushButton(this);
    this->show();
    btn->setText("点老子！");
    connect(btn,QPushButton::clicked,[=](){
        exit(-1);
    });
}
