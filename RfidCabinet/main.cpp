#include "widget.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
//    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);

    Widget w;
    w.show();

    return a.exec();
}
