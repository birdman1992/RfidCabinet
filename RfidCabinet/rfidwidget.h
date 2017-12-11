#ifndef RFIDWIDGET_H
#define RFIDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QButtonGroup>

namespace Ui {
class RfidWidget;
}

class RfidWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RfidWidget(QWidget *parent = 0);
    ~RfidWidget();

public slots:
    void setCabinetSize(int widNum, int heiNum);

protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::RfidWidget *ui;

    QButtonGroup* menu;

    void initMenu();
};

#endif // RFIDWIDGET_H
