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

void LockConfig::createButtonLayout(QString layout)
{
    QStringList layouts = layout.split("#", QString::SkipEmptyParts);

    if(layouts.isEmpty())
        return;

    for(i=0; i<list_temp.count(); i++)
    {
        QVBoxLayout* colLayout = new QVBoxLayout();

        for(j=0; j<list_temp[i]->list_case.count(); j++)
        {
            QString layoutStrech = list_temp.at(i)->getLayout();
            QPushButton* btn = new QPushButton(this);
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            btn->setMaximumWidth(400);
            QString str = QString("序号：%1\nIO号：%2").arg(list_temp.at(i)->list_case.at(j)->ctrlSeq).arg(list_temp.at(i)->list_case.at(j)->ctrlIndex);
            btn->setText(str);
            colLayout->addWidget(btn);
            l_lock_conf.addButton(btn,((list_temp.at(i)->getSeqNum()<<8)+j));
            colLayout->setStretch(j,QString(layoutStrech.at(j)).toInt());
        }
        ui->frame_col->layout()->addLayout(layout);
    }
}
