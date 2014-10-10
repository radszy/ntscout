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

void CountryWidget::setFlag(const QPixmap pixmap)
{
    ui->flagLabel->setPixmap(pixmap);
}

void CountryWidget::setName(const QString name)
{
    QString shortened = name;
    int max = 14;
    if (name.count() > max) {
        shortened = name.mid(0, max) + "...";
    }
    ui->nameLabel->setText(shortened);
}

void CountryWidget::setDivisions(const QString divisions)
{
    ui->divisionsLabel->setText("Divisions: " + divisions);
}

void CountryWidget::setUsers(const QString users)
{
    ui->usersLabel->setText("Users: " + users);
}

void CountryWidget::mousePressEvent(QMouseEvent* event)
{

}
