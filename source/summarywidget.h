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

    void setResults(PlayerList p);

private:
    Ui::SummaryWidget *ui;
};

#endif // SUMMARYWIDGET_H
