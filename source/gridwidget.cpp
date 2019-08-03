#include "gridwidget.h"
#include "ui_gridwidget.h"
#include "countrywidget.h"
#include "searchdialog.h"

#include <algorithm>
#include <QtMath>
#include <QMenu>
#include <QAction>
#include <QDebug>

GridWidget::GridWidget(QWidget* parent)
        : QWidget(parent)
        , mMarkedWidget(nullptr)
        , mUi(new Ui::GridWidget)
{
	mUi->setupUi(this);

	auto menu = new QMenu(mUi->selectAll);
	QAction* actCountry = menu->addAction("Country");
	QAction* actNationality = menu->addAction("Nationality");
	mUi->selectAll->setMenu(menu);

	connect(actCountry, SIGNAL(triggered()), this, SLOT(selectAllCountry()));
	connect(actNationality, SIGNAL(triggered()), this, SLOT(selectAllNationality()));

	createShortcut(mUi->searchField, Qt::CTRL + Qt::Key_F, "setFocus()");
	createShortcut(mUi->scrollArea, Qt::CTRL + Qt::Key_G, "setFocus()");
	createShortcut(this, Qt::Key_Right, "markRight()");
	createShortcut(this, Qt::Key_Left, "markLeft()");
	createShortcut(this, Qt::Key_Up, "markUp()");
	createShortcut(this, Qt::Key_Down, "markDown()");
	createShortcut(this, Qt::Key_C, "selectMarkedCountry()");
	createShortcut(this, Qt::Key_N, "selectMarkedNationality()");
	createShortcut(this, Qt::CTRL + Qt::Key_Space, "showSearchDialog()");
}

GridWidget::~GridWidget()
{
	delete mUi;
}

QAction* GridWidget::createShortcut(QWidget* widget,
                                    const QKeySequence& sequence,
                                    const QString& func)
{
	auto action = new QAction(widget);
	action->setShortcut(sequence);
	widget->addAction(action);
	connect(action, "2triggered()", widget, QString("1" + func).toLatin1());
	return action;
}

void GridWidget::reset()
{
	mUi->showSelected->setChecked(false);
	mUi->searchField->setText("");

	mCountryWidgets.clear();
	for (CountryWidget* widget : mOriginalWidgets) {
		widget->setVisible(true);
		widget->unselect();
		mCountryWidgets.append(widget);
	}

	if (mUi->sortBox->currentIndex() != ID) {
		mUi->sortBox->setCurrentIndex(ID);
	}
	else {
		sortBy(ID);
	}
}

void GridWidget::setCountries(const Countries& countries)
{
	mCountries = countries;

	for (const Country& country : mCountries) {
		auto widget = new CountryWidget;
		widget->setFlag(QPixmap("flags/flag_" + QString::number(country.id) + ".gif"));
		widget->setName(country.name_en);
		widget->setInitialToolTip(country.name);
		widget->setDivisions(country.divisions);
		widget->setUsers(country.users);
		widget->setID(country.id);

		mOriginalWidgets.append(widget);
		mCountryWidgets.append(widget);

		connect(widget,
		        SIGNAL(selected(CountryWidget*)),
		        this,
		        SLOT(countrySelected(CountryWidget*)));
		connect(widget,
		        SIGNAL(unselected(CountryWidget*)),
		        this,
		        SLOT(countryUnselected(CountryWidget*)));
		connect(widget,
		        SIGNAL(hovered(CountryWidget*)),
		        this,
		        SLOT(setMarkedWidget(CountryWidget*)));
	}

	if (!mOriginalWidgets.isEmpty()) {
		SearchDialog* dialog = CountryWidget::getSearchDialog();
		connect(dialog, SIGNAL(updateDefaultValues()), this, SLOT(updateCountryWidgets()));
	}
}

void GridWidget::emitCanProceed()
{
	bool country = false;
	bool nationality = false;

	for (CountryWidget* const widget : mSelectedWidgets) {
		country |= widget->isCountrySelected();
		nationality |= widget->isNationalitySelected();
	}

	emit canProceed(country && nationality);
}

void GridWidget::clearMarkedWidget()
{
	if (mMarkedWidget != nullptr) {
		if (!mCountryWidgets.contains(mMarkedWidget)) {
			mMarkedWidget->unmarkFrame();
			mMarkedWidget = nullptr;
		}
	}
}

QList<SearchValues*> GridWidget::getSearchValues()
{
	QList<SearchValues*> searchValues;
	for (CountryWidget* widget : mSelectedWidgets) {
		searchValues.append(widget->getSearchValues());
	}
	return searchValues;
}

void GridWidget::updateGrid()
{
	if (mCountryWidgets.isEmpty()) {
		return;
	}

	int cols = qMax(1, size().width() / mCountryWidgets.front()->width());
	int rows = qCeil(mCountryWidgets.count() / (double)cols);

	for (CountryWidget* widget : mOriginalWidgets) {
		mUi->gridLayout->removeWidget(widget);
	}

	int k = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols && k < mCountryWidgets.count(); ++j, ++k) {
			mUi->gridLayout->addWidget(mCountryWidgets.at(k), i, j);
		}
	}

	clearMarkedWidget();
}

void GridWidget::resizeEvent(QResizeEvent* event)
{
	updateGrid();

	QWidget::resizeEvent(event);
}

void GridWidget::sortBy(int index)
{
	CountryWidgets& widgets = mCountryWidgets.count() < mOriginalWidgets.count()
	                                  ? mCountryWidgets
	                                  : mOriginalWidgets;

	std::sort(widgets.begin(),
	          widgets.end(),
	          [index](const CountryWidget* a, const CountryWidget* b) -> bool {
		          switch (index) {
			          case ID:
				          return a->getID() < b->getID();
			          case Name:
				          return a->getName() < b->getName();
			          case Divisions:
				          return a->getDivisions() < b->getDivisions();
			          case Users:
			          default:
				          return a->getUsers() < b->getUsers();
		          }
	          });

	// updateGrid is based on countryWidgets
	if (mCountryWidgets.count() >= mOriginalWidgets.count()) {
		mCountryWidgets.clear();
		mCountryWidgets.append(mOriginalWidgets);
	}

	updateGrid();
}

void GridWidget::searchCountry(const QString& text)
{
	for (CountryWidget* widget : mOriginalWidgets) {
		widget->setVisible(false);
	}
	mCountryWidgets.clear();

	if (text.isEmpty()) {
		for (CountryWidget* widget : mOriginalWidgets) {
			widget->setVisible(true);
			mCountryWidgets.append(widget);
		}
	}
	else {
		for (CountryWidget* widget : mOriginalWidgets) {
			if (widget->getName().contains(text, Qt::CaseInsensitive) ||
			    widget->getNameEn().contains(text, Qt::CaseInsensitive)) {
				widget->setVisible(true);
				mCountryWidgets.append(widget);
			}
		}
	}

	updateGrid();
}

void GridWidget::selectAllCountry()
{
	for (CountryWidget* widget : mCountryWidgets) {
		widget->selectAsCountry();
	}
}

void GridWidget::selectAllNationality()
{
	for (CountryWidget* widget : mCountryWidgets) {
		widget->selectAsNationality();
	}
}

void GridWidget::unselectAll()
{
	for (CountryWidget* widget : mCountryWidgets) {
		widget->unselect();
		widget->loadNationalityValues();
		widget->loadCountryValues();
	}
}

void GridWidget::showSelected()
{
	for (CountryWidget* widget : mOriginalWidgets) {
		widget->setVisible(false);
	}
	mCountryWidgets.clear();

	if (mUi->showSelected->isChecked()) {
		for (CountryWidget* widget : mOriginalWidgets) {
			if (widget->isSelected()) {
				widget->setVisible(true);
				mCountryWidgets.append(widget);
			}
		}
	}
	else {
		searchCountry(mUi->searchField->text());
	}

	updateGrid();
}

void GridWidget::countrySelected(CountryWidget* widget)
{
	if (!mSelectedWidgets.contains(widget)) {
		mSelectedWidgets.append(widget);
	}
	emitCanProceed();
}

void GridWidget::countryUnselected(CountryWidget* widget)
{
	mSelectedWidgets.removeOne(widget);
	if (mSelectedWidgets.empty() && mUi->showSelected->isChecked()) {
		mUi->showSelected->setChecked(false);
		showSelected();
	}
	emitCanProceed();
}

void GridWidget::updateCountryWidgets()
{
	for (CountryWidget* country : mOriginalWidgets) {
		if (!country->isSelected()) {
			country->loadNationalityValues();
			country->loadCountryValues();
		}
	}
}

void GridWidget::setMarkedWidget(CountryWidget* widget)
{
	if (mMarkedWidget != nullptr) {
		mMarkedWidget->unmarkFrame();
	}
	mMarkedWidget = widget;
	mMarkedWidget->markFrame();
}

void GridWidget::markRight()
{
	if (!mUi->scrollArea->hasFocus() || mCountryWidgets.isEmpty()) {
		return;
	}

	if (mMarkedWidget != nullptr) {
		mMarkedWidget->unmarkFrame();

		const int cols = mUi->gridLayout->columnCount();
		const int index = mCountryWidgets.indexOf(mMarkedWidget);

		if ((index + 1) % cols != 0 && index < mCountryWidgets.count() - 1) {
			mMarkedWidget = mCountryWidgets.at(index + 1);
		}
	}
	else {
		mMarkedWidget = mCountryWidgets.first();
	}

	mMarkedWidget->markFrame();
	mUi->scrollArea->ensureWidgetVisible(mMarkedWidget);
}

void GridWidget::markLeft()
{
	if (!mUi->scrollArea->hasFocus() || mCountryWidgets.isEmpty()) {
		return;
	}

	if (mMarkedWidget != nullptr) {
		mMarkedWidget->unmarkFrame();

		const int cols = mUi->gridLayout->columnCount();
		const int index = mCountryWidgets.indexOf(mMarkedWidget);

		if ((index + 1) % cols != 1) {
			mMarkedWidget = mCountryWidgets.at(index - 1);
		}
	}
	else {
		mMarkedWidget = mCountryWidgets.first();
	}

	mMarkedWidget->markFrame();
	mUi->scrollArea->ensureWidgetVisible(mMarkedWidget);
}

void GridWidget::markDown()
{
	if (!mUi->scrollArea->hasFocus() || mCountryWidgets.isEmpty()) {
		return;
	}

	if (mMarkedWidget != nullptr) {
		mMarkedWidget->unmarkFrame();

		const int cols = mUi->gridLayout->columnCount();
		const int index = mCountryWidgets.indexOf(mMarkedWidget);

		if (index + cols < mCountryWidgets.count()) {
			mMarkedWidget = mCountryWidgets.at(index + cols);
		}
	}
	else {
		mMarkedWidget = mCountryWidgets.first();
	}

	mMarkedWidget->markFrame();
	mUi->scrollArea->ensureWidgetVisible(mMarkedWidget);
}

void GridWidget::markUp()
{
	if (!mUi->scrollArea->hasFocus() || mCountryWidgets.isEmpty()) {
		return;
	}

	if (mMarkedWidget != nullptr) {
		mMarkedWidget->unmarkFrame();

		const int cols = mUi->gridLayout->columnCount();
		const int index = mCountryWidgets.indexOf(mMarkedWidget);

		if (index >= cols) {
			mMarkedWidget = mCountryWidgets.at(index - cols);
		}
	}
	else {
		mMarkedWidget = mCountryWidgets.first();
	}

	mMarkedWidget->markFrame();
	mUi->scrollArea->ensureWidgetVisible(mMarkedWidget);
}

void GridWidget::selectMarkedCountry()
{
	if (mMarkedWidget == nullptr) {
		return;
	}

	const bool select = !mMarkedWidget->isCountrySelected();
	mMarkedWidget->selectAsCountry(select);
}

void GridWidget::selectMarkedNationality()
{
	if (mMarkedWidget == nullptr) {
		return;
	}

	const bool select = !mMarkedWidget->isNationalitySelected();
	mMarkedWidget->selectAsNationality(select);
}

void GridWidget::showSearchDialog()
{
	if (!mUi->scrollArea->hasFocus() || mMarkedWidget == nullptr) {
		return;
	}

	mMarkedWidget->showSearchDialog();
}
