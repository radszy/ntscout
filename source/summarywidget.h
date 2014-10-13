#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H

#include <QWidget>

#include "player.h"

namespace Ui {
class SummaryWidget;
}

class SummaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryWidget(QWidget *parent = 0);
    ~SummaryWidget();

    void reset();
    void setResults(PlayerList p);

public slots:
    void openFile();
    void openDirectory();

private:
    Ui::SummaryWidget *ui;
};

#endif // SUMMARYWIDGET_H
