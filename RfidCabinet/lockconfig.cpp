#include "lockconfig.h"
#include "ui_lockconfig.h"

LockConfig::LockConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LockConfig)
{
    ui->setupUi(this);
    cabManager = CabinetManager::manager();

}

LockConfig::~LockConfig()
{
    delete ui;
}
