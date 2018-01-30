#ifndef RFIDAREA_H
#define RFIDAREA_H

#include <QWidget>
#include <QPaintEvent>
#include <QFont>
#include "device/repertorymanager.h"

namespace Ui {
class RfidArea;
}

class RfidArea : public QWidget
{
    Q_OBJECT

public:
    explicit RfidArea(QWidget *parent = 0);
    ~RfidArea();
    void setAntId(int id);
    void setBackColor(QColor c);
    void updateInfo();
    QString getShowStr();

protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::RfidArea *ui;
    QFont* font;
    int antId;

    QString getRepFile(int antId);
};

#endif // RFIDAREA_H
