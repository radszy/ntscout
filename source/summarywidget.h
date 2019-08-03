#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H

#include "player.h"

#include <QWidget>

namespace Ui {
class SummaryWidget;
}

class SummaryWidget : public QWidget {
	Q_OBJECT

public:
	explicit SummaryWidget(QWidget* parent = nullptr);
	~SummaryWidget();

	void reset();
	void setResults(const PlayerList& playerList);

public slots:
	void openFile();
	void openDirectory();

private:
	QString toCentimeters(int value);
	QString toMeters(int value);
	QString toInches(int value);
	QString toFeet(int value);

	Ui::SummaryWidget* ui;
};

#endif // SUMMARYWIDGET_H
