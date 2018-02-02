#ifndef RFIDWIDGET_H
#define RFIDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QModelIndex>
#include "device/rfidant.h"
#include "RfidCell/rfidarea.h"
#include "device/rfiddevice.h"
#include "Server/httpapi.h"
#include "device/repertorymanager.h"
#include "Structs/cabinetinfo.h"
#include "RfidCell/cabinet.h"
#include <QList>
#include <QRect>
#include <qmap.h>
#include <QStringList>
#include <QTableWidget>
#include <QPoint>

namespace Ui {
class RfidWidget;
}

class RfidWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RfidWidget(QWidget *parent = 0);
    ~RfidWidget();
    void initData();

public slots:
    void goodsIn(QList<GoodsInfo*>);
    void goodsOut(QList<GoodsInfo*>);
    void rfidIn(QList<rfidChangeInfo*>);
    void rfidOut(QList<rfidChangeInfo*>);

protected:
    void paintEvent(QPaintEvent *);
private slots:
    void on_save_clicked();
    void on_back_clicked();
    void on_cell_config_clicked();
    void on_clear_config_clicked();
    void on_test_open_clicked(bool checked);

    void on_preCab_clicked(const QModelIndex &index);

    void on_addCab_clicked();

    void on_delCab_clicked();

    void on_cabType_currentIndexChanged(const QString &arg1);

signals:
    void doorStareChanged(bool isOpen);
    void rfidStoreReq(QList<rfidChangeInfo*>);
    void rfidFetchReq(QList<rfidChangeInfo*>);
    void insertRfidReq(QStringList);
    void test();

private:
    Ui::RfidWidget *ui;
    int rowCount;
    int colCount;
    bool onSpan;
    bool needSelScreen;
    QPoint screenPos;
    int spanX;
    int spanY;

    QButtonGroup* menu;
    QList<RfidAnt*> listAnt;
    QList<Cabinet*> listCabinet;
    QList<QTableWidget*> listTab;
    QStringList listLayout;
    QMap<int, RfidAnt*> antsMap;
    RepertoryManager* repManager;

    void initMenu();
    void initAnt();//初始化天线
    void initCabType(QStringList typeList);//初始化柜子类型
    void setMenuPow(int _pow);//设置菜单权限等级
    void creatRfidCells();
    void creatDeafultAnts();
    void creatAntsMap();
    void rfidCellClear();
    void menuLock();
    void menuUnlock();
    void saveCellsInfo();
    void readCellsInfo();
    void cabSplit(QString scale, QTableWidget *table);
    void cabGridVisible(bool show);
    int getBaseCount(QString scale);
    QString cellStyle(QColor rgb);
    bool checkPos(QPoint pos);
    void readCellsData();
};

#endif // RFIDWIDGET_H
