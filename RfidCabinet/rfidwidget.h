#ifndef RFIDWIDGET_H
#define RFIDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QModelIndex>
#include "RfidCell/rfidarea.h"
#include <QList>
#include <QRect>

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
private slots:
    void on_row_add_clicked();

    void on_row_red_clicked();

    void on_col_add_clicked();

    void on_col_red_clicked();

    void on_save_clicked();

    void on_span_toggled(bool checked);

    void on_rfidPanel_clicked(const QModelIndex &index);

    void on_span_cancel_clicked();

    void on_back_clicked();

    void on_cell_config_clicked();

    void on_clear_config_clicked();

private:
    Ui::RfidWidget *ui;
    int rowCount;
    int colCount;
    bool onSpan;
    int spanX;
    int spanY;

    QButtonGroup* menu;
    QList<RfidArea*> listCells;
    QList<QRectF*> listSpans;

    void initMenu();
    void creatRfidCells();
    bool pointIsInSpan(int row, int col);
    void rfidCellClear();
    void menuLock();
    void menuUnlock();
    void setCellsLayout(QByteArray cellsLayout);
    QByteArray getCellsLayout();
    void saveCellsInfo();
    void readCellsInfo();
};

#endif // RFIDWIDGET_H
