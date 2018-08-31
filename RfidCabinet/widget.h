#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qstackedwidget.h>
#include <QVBoxLayout>
#include "rfidwidget.h"
#include "device/devicemanager.h"
#include "Server/httpapi.h"
#include "cabinetserver.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void initServer();
private:
    Ui::Widget *ui;
    DeviceManager* devManager;
    CabinetServer* cabServer;

    HttpApi* serverHttp;
    RfidWidget* win_rfid;
    QStackedWidget* win_stack;
    QVBoxLayout* win_layout;
};

#endif // WIDGET_H
