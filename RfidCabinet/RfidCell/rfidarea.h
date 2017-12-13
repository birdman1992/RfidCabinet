#ifndef RFIDAREA_H
#define RFIDAREA_H

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class RfidArea;
}

class RfidArea : public QWidget
{
    Q_OBJECT

public:
    explicit RfidArea(QWidget *parent = 0);
    ~RfidArea();

protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::RfidArea *ui;
};

#endif // RFIDAREA_H
