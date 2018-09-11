#ifndef LOCKCONFIG_H
#define LOCKCONFIG_H

#include <QWidget>
#include "manager/cabinetmanager.h"

//unuse class
namespace Ui {
class LockConfig;
}

class LockConfig : public QWidget
{
    Q_OBJECT

public:
    explicit LockConfig(QWidget *parent = 0);
    ~LockConfig();

private:
    Ui::LockConfig *ui;
    CabinetManager* cabManager;
    void createButtonLayout();
};

#endif // LOCKCONFIG_H
