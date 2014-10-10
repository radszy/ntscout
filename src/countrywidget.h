#ifndef COUNTRYWIDGET_H
#define COUNTRYWIDGET_H

#include "country.h"

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class CountryWidget;
}

class CountryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CountryWidget(QWidget *parent = 0);
    ~CountryWidget();

    void setFlag(const QPixmap pixmap);
    void setName(const QString name);
    void setDivisions(const QString divisions);
    void setUsers(const QString users);

    void mousePressEvent(QMouseEvent* event);

private:
    Ui::CountryWidget *ui;
};

#endif // COUNTRYWIDGET_H
