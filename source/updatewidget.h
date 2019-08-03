#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>

namespace Ui {
class UpdateWidget;
}

class UpdateWidget : public QWidget {
	Q_OBJECT

public:
	explicit UpdateWidget(QWidget* parent = 0);
	~UpdateWidget();

public slots:
	void onStart();
	void updateProgress(qint64 received, qint64 total);

private:
	bool updateCountryData();
	bool updateBinaryData();

	Ui::UpdateWidget* ui;
};

#endif // UPDATERWIDGET_H
