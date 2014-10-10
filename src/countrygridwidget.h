#ifndef COUNTRIES_H
#define COUNTRIES_H

#include "countrywidget.h"

#include <QWidget>
#include <QResizeEvent>

#include <QHBoxLayout>

namespace Ui {
class CountryGridWidget;
}

class CountryGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CountryGridWidget(QWidget *parent = 0);
    ~CountryGridWidget();

    void resizeEvent(QResizeEvent* event);

private:
    Ui::CountryGridWidget *ui;

    CountryWidget* cnt[98];
};

#endif // COUNTRIES_H
