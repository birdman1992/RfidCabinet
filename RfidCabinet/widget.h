#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qstackedwidget.h>
#include <QVBoxLayout>
#include "rfidwidget.h"
#include "device/devicemanager.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    DeviceManager* devManager;

    RfidWidget* win_rfid;
    QStackedWidget* win_stack;
    QVBoxLayout* win_layout;
};

#endif // WIDGET_H
