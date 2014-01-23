/*
 *目的：等待动画
 *作用：服务器与本地客户端的通信，构造等待动画
 *时间：2013.05
 */

#ifndef LOADWAITGIF_H
#define LOADWAITGIF_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QMovie>
#include <QVBoxLayout>
#include <QThread>
class LoadWaitGif : public QDialog
{
    Q_OBJECT
public:
    explicit LoadWaitGif(QWidget *parent = 0);
    ~LoadWaitGif();
    void showGif();
    void hideGif();
signals:
    
public slots:
private:
    QLabel *giflLabel;
    QMovie *movie;
    QThread *thread;
};

#endif // LOADWAITGIF_H
