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
    void readAreaData();
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
    QStringList addId(QStringList list);
    QStringList removeId(QStringList list);
    bool addId(QString _id);
    bool removeId(QString _id);
    QStringList sameList(QStringList newList, QStringList fixList);
    QStringList diffList(QStringList newList, QStringList fixList);
};

#endif // RFIDAREA_H
