#include "countrygridwidget.h"
#include "ui_countrygridwidget.h"

#include "countrywidget.h"

#include <QHBoxLayout>

#include <QDebug>

CountryGridWidget::CountryGridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountryGridWidget)
{
    ui->setupUi(this);

    int cnum = 100;

    for (int i = 0; i < cnum; ++i) {
        hbox[i] = new QHBoxLayout;
        cnt[i] = new CountryWidget;
    }
}

CountryGridWidget::~CountryGridWidget()
{
    delete ui;
}

void CountryGridWidget::resizeEvent(QResizeEvent* event)
{
//    qDebug() << event->size();
//    qDebug() << cnt[0]->size();

    int cols = event->size().width() / cnt[0]->size().width();
    int rows = 100 / cols;

//    qDebug() << cols;
//    qDebug() << rows;

    while (!ui->countryLayout->isEmpty()) {
        ui->countryLayout->takeAt(0);
    }

    int k = 0;
    for (int i = 0; i < rows; ++i) {
        // clear layout
        while (!hbox[i]->isEmpty()) {
            hbox[i]->takeAt(0);
        }
        // add items again
        for (int j = 0; j < cols; ++j, ++k) {
            hbox[i]->addWidget(cnt[k]);
        }
        ui->countryLayout->addLayout(hbox[i]);
    }

    QWidget::resizeEvent(event);
}
