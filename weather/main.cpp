#include "widget.h"
#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setLibraryPaths(QStringList(QCoreApplication::applicationDirPath()+"/plugins/"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    Widget w;
    w.myShow();
    
    return a.exec();
}
