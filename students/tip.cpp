#include "tip.h"
#include "ui_tip.h"

tip::tip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tip)
{
    ui->setupUi(this);

}

tip::~tip()
{
    delete ui;
}

void tip::setText(QString text)
{
    ui->label->setText(text);
}
