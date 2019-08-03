#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

struct SearchValues;

class SearchDialog : public QDialog {
	Q_OBJECT

public:
	explicit SearchDialog(QWidget* parent = 0);
	~SearchDialog();

	void updateValues();
	void setValues(SearchValues* values);

signals:
	void updateDefaultValues();

private:
	SearchValues* searchValues;
	Ui::SearchDialog* ui;
};

#endif // SEARCHDIALOG_H
