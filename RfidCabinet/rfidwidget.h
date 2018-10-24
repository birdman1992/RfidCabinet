#ifndef RFIDWIDGET_H
#define RFIDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QModelIndex>
#include <QLabel>
#include "device/rfidant.h"
#include "RfidCell/rfidarea.h"
#include "device/rfiddevice.h"
#include "Server/httpapi.h"
#include "device/repertorymanager.h"
#include "Structs/cabinetinfo.h"
#include "RfidCell/cabinet.h"
#include "manager/cabinetmanager.h"
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
    void rfidTest(QString);
    void cabRegRst(bool);

protected:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);
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
    void on_serverAddr_editingFinished();
    void on_save_settings_clicked();
    void on_set_clicked();
    void on_cabReg_clicked();
    void on_set_server_addr_clicked();
    
signals:
    void doorStareChanged(bool isOpen);
    void rfidStoreReq(QList<rfidChangeInfo*>);
    void rfidFetchReq(QList<rfidChangeInfo*>);
    void insertRfidReq(QStringList);
    void cabRegReq();
    void test();
    void updateServerAddress();

private:
    Ui::RfidWidget *ui;
    int rowCount;
    int colCount;
    bool onSpan;
    bool needSelScreen;
    bool flagReg;
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
    CabinetManager* cabManager;

    void initMenu();
    void initAnt();//初始化天线
    void initCabType(QStringList typeList);//初始化柜子类型
    void initLockConfig();//初始化锁控配置
    void checkCabinetId();
    void setMenuPow(int _pow);//设置菜单权限等级
    void setMsgCorlor(QLabel* lab, QString color);
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
