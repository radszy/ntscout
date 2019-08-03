#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include "country.h"

#include <QWidget>

class QResizeEvent;
struct SearchValues;
class CountryWidget;

namespace Ui {
class GridWidget;
}

enum SortOrder {
	ID,
	Name,
	Divisions,
	Users,
};

class GridWidget : public QWidget {
	Q_OBJECT

public:
	explicit GridWidget(QWidget* parent = nullptr);
	~GridWidget();

	QAction* createShortcut(QWidget* widget, const QKeySequence& sequence, const QString& func);
	void reset();
	void updateGrid();
	void checkIfCanProceed();
	void clearMarkedWidget();

	void setCountryList(const CountryList& list);
	QList<SearchValues*> getSearchValues();

	void resizeEvent(QResizeEvent* event);

public slots:
	void sortBy(int index);
	void searchCountry(const QString& text);
	void selectAllCountry();
	void selectAllNationality();
	void unselectAll();
	void showSelected();
	void countrySelected(CountryWidget* widget);
	void countryUnselected(CountryWidget* widget);
	void updateCountryWidgets();

	void setMarkedWidget(CountryWidget* widget);
	void markRight();
	void markLeft();
	void markUp();
	void markDown();
	void selectMarkedCountry();
	void selectMarkedNationality();
	void showSearchDialog();

signals:
	void canProceed(bool);

private:
	CountryList countryList;
	QList<CountryWidget*> originalWidgets;
	QList<CountryWidget*> countryWidgets;
	QList<CountryWidget*> selectedWidgets;
	CountryWidget* markedWidget;

	Ui::GridWidget* ui;
};

#endif // GRIDWIDGET_H
