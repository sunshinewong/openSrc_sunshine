#ifndef HTTP_H
#define HTTP_H

#include <QObject>
#include <QtNetwork>
class http : public QObject
{
    Q_OBJECT
public:
    explicit http(QObject *parent = 0);
    ~http();
    //fn=1:获取天气详细信息
    //fn=2获取天气基础信息
    void get(QString urlName, int fn);
signals:
    void getWeaterSucs(bool isSucs,int fn,QByteArray &bytes);
private slots:
    void httpReadyRead();
    void httpFinished();
private:
    void put();
    int Fn;
    QNetworkAccessManager *manger;
    QNetworkReply *reply;
    QByteArray allByte;
    QList<QNetworkReply::RawHeaderPair> headerList;
};

#endif // HTTP_H
