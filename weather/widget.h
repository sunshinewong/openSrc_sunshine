#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QDesktopWidget>
#include "http.h"
#include "loadwaitgif.h"

typedef QMap<QString,QString> StringMap;
typedef QMultiMap<QString,QString> StringMultiMap;
typedef QList<StringMap> ListStringMap;
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void myShow();
    bool eventFilter(QObject *target, QEvent *e);
private:
    Ui::Widget *ui;
    QMap<int,QByteArray> contents;
    StringMultiMap location;
    StringMap codeMap;
    http *weatherHttp;
    LoadWaitGif *waitGif;
    bool isInit;
    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    QAction *quitAction;
    struct Type
    {
        bool f1;
        bool f2;
    } type;
    //窗体移动记录
    QPoint windowPos,mousePos,dPos;
    QTimer *timer;
    void initCityMap();
    void initLocation();
    void showGif();
    QString findIconPath(QString index,int type);
    QString getWeeks(int wek);
private slots:
    void slotInitCurTemp();
    void getWeather(int fn);
    void slotGetWeaterSucs(bool,int,QByteArray&);
    void slotMangerInfo();
    void on_closeBtn_clicked();
    void on_provinceComboBox_currentIndexChanged(const QString &arg1);
    void on_refreshBtn_clicked();
    void on_cityComboBox_currentIndexChanged(int index);
    void slotSysTray(QSystemTrayIcon::ActivationReason);
    void on_minBtn_clicked();
    void slotTimeOut();
};

#endif // WIDGET_H
