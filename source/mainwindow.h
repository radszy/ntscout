#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "country.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class LoginWidget;
class GridWidget;
class ProgressWidget;
class SummaryWidget;
class UpdateWidget;
class SettingsDialog;

class MainWindow : public QMainWindow {
	Q_OBJECT

	enum Page {
		Login,
		Country,
		Progress,
		Summary,
		Update,
	};

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

	void proceedToCountryWidget();
	void proceedToProgressWidget();
	void proceedToSummaryWidget();
	void goBackToCountryWidget();

	void closeEvent(QCloseEvent* event);

public slots:
	void nextClicked();
	void backClicked();
	void updateTriggered();
	void settingsTriggered();
	void reportTriggered();
	void aboutTriggered();
	void enableNextButton(bool enabled);

private:
	void readDataFile(CountryList& list);

	LoginWidget* loginWidget;
	GridWidget* gridWidget;
	ProgressWidget* progressWidget;
	SummaryWidget* summaryWidget;
	UpdateWidget* updateWidget;
	SettingsDialog* settingsDialog;
	Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
