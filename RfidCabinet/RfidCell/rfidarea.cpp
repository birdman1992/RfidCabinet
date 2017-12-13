#include "rfidarea.h"
#include "ui_rfidarea.h"
#include <qgraphicseffect.h>
#include <QPainter>

RfidArea::RfidArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RfidArea)
{
    ui->setupUi(this);
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0,0);
    shadow_effect->setColor(Qt::black);
    shadow_effect->setBlurRadius(8);
    ui->label->setGraphicsEffect(shadow_effect);
}

RfidArea::~RfidArea()
{
    delete ui;
}

void RfidArea::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
