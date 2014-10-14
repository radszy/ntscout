#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H

#include "player.h"

#include <QWidget>

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
    void setResults(const PlayerList& p);

public slots:
    void openFile();
    void openDirectory();

private:
    Ui::SummaryWidget *ui;
};

#endif // SUMMARYWIDGET_H
