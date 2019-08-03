#ifndef COUNTRYWIDGET_H
#define COUNTRYWIDGET_H

#include "country.h"
#include "searchvalues.h"

#include <QWidget>

class SearchDialog;

namespace Ui {
class CountryWidget;
}

class CountryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CountryWidget(QWidget *parent = 0);
    ~CountryWidget();

    void loadNationalityValues();
    void loadCountryValues();
    void setFlag(const QPixmap& pixmap);
    void setName(const QString& value);
    void setDivisions(int value);
    void setUsers(int value);
    void setID(int value);
    void setInitialToolTip(const QString& tooltip);

    QString getName() const {return name;}
    QString getNameEn() const {return nameEn;}
    int getID() const {return id;}
    int getDivisions() const {return divisions;}
    int getUsers() const {return users;}
    SearchValues* getSearchValues() {return &searchValues;}

    void selectAsCountry(bool value = true);
    void selectAsNationality(bool value = true);
    void unselect();
    bool isSelected() const;
    bool isCountrySelected() const;
    bool isNationalitySelected() const;

    void markFrame();
    void unmarkFrame();

    void showSearchDialog();
    static SearchDialog* getSearchDialog() {
        return searchDialog;
    }

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

signals:
    void selected(CountryWidget*);
    void unselected(CountryWidget*);
    void hovered(CountryWidget*);

private:
    void updateFrame();

    SearchValues searchValues;
    QString name;
    QString nameEn;
    int id;
    int divisions;
    int users;

    static SearchDialog* searchDialog;
    Ui::CountryWidget *ui;
};

#endif // COUNTRYWIDGET_H
