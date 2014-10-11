#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

struct SearchValues {
    bool div[6];
    QPair<int,int> age;
    QPair<int,int> potential;
    QPair<int,int> salary;
    QPair<int,int> dmi;
    bool countrySet;
    bool nationalitySet;
};

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = 0);
    ~SearchDialog();

    SearchValues getValues();
    void setValues(SearchValues values);

private:
    Ui::SearchDialog *ui;
};

#endif // SEARCHDIALOG_H
