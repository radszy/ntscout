#ifndef COUNTRYGRIDWIDGET_H
#define COUNTRYGRIDWIDGET_H

#include "country.h"
#include "countrywidget.h"

#include <QWidget>
#include <QResizeEvent>

namespace Ui {
class CountryGridWidget;
}

class CountryGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CountryGridWidget(QWidget *parent = 0);
    ~CountryGridWidget();

    void setCountryList(CountryList clist);

    void resizeEvent(QResizeEvent* event);

private:
    CountryList countryList;
    QList<CountryWidget*> countryWidgets;

    Ui::CountryGridWidget *ui;
};

#endif // COUNTRYGRIDWIDGET_H
