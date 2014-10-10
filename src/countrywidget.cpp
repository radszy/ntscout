#include "countrywidget.h"
#include "ui_countrywidget.h"

#include <QDebug>

CountryWidget::CountryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountryWidget)
{
    ui->setupUi(this);
}

CountryWidget::~CountryWidget()
{
    delete ui;
}

void CountryWidget::mousePressEvent(QMouseEvent* event)
{

}
