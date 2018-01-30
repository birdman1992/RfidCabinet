#ifndef CABINET_H
#define CABINET_H

#include <QTableWidget>
#include <QWidget>
#include <QString>
#include <QResizeEvent>
#include <QPoint>
#include "rfidarea.h"

class Cabinet : public QTableWidget
{
    Q_OBJECT
public:
    explicit Cabinet(QString _layout, int _colNum, QWidget *parent = 0);
    ~Cabinet();
    void setScreenPos(QPoint pos);
    void initCabinet();
    RfidArea* areaAt(int index);

signals:

public slots:

private:
    QString layout;
    int colNum;
    QPoint screenPos;
    QList<RfidArea*> list_area;

    void resizeEvent(QResizeEvent *);
    int getBaseCount(QString scale);
    void cabSplit(QString scale);
};

#endif // CABINET_H
