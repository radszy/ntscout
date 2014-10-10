#ifndef COUNTRY_H
#define COUNTRY_H

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

    void mousePressEvent(QMouseEvent* event);

private:
    Ui::CountryWidget *ui;

    QString name;
    QString name_en;
    int id;
    int divisions;
    int users;
};

#endif // COUNTRY_H
