#ifndef RFIDAREA_H
#define RFIDAREA_H

#include <QWidget>
#include <QPaintEvent>
#include <QFont>
#include <QPoint>
#include "device/repertorymanager.h"

namespace Ui {
class RfidArea;
}

class RfidArea : public QWidget
{
    Q_OBJECT

public:
    explicit RfidArea(QPoint pos, QWidget *parent = 0);
    ~RfidArea();
    QPoint getPos();
    void setAntId(int id);
    void setBackColor(QColor c);
    void updateInfo();
    QString getShowStr();

    QStringList list_id;

protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::RfidArea *ui;
    QFont* font;
    QPoint areaPos;
    int antId;

    QString getRepFile();
};

#endif // RFIDAREA_H
