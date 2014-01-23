#include "http.h"
#include <QDebug>

http::http(QObject *parent) :
    QObject(parent)
{
    manger=new QNetworkAccessManager();
    reply=NULL;
}
http::~http()
{
}
//http请求
void http::get(QString urlName,int fn)
{
        Fn=fn;
        QNetworkRequest request;
        QUrl url;
        url.setUrl(urlName);
        request.setUrl(url);

        reply=manger->get(request);

        connect(reply,SIGNAL(finished()),
                this,SLOT(httpFinished()),Qt::DirectConnection);
        connect(reply,SIGNAL(readyRead()),
                this,SLOT(httpReadyRead()),Qt::DirectConnection);
}

void http::httpReadyRead() //有可用数据
{
    QByteArray bytes=reply->readAll();
    allByte+=bytes;
}
//返回完毕，进行处理数据
void http::httpFinished()
{
    QMap<QString,QString> header;
    headerList=reply->rawHeaderPairs();
    //获取sessionid会话ID 并解析头信息
    for(int i=0;i<headerList.count();i++)
    {
        if(headerList.at(i).first==tr("Set-Cookie"))
        {
           // getsession(manger->cookieJar());
        }
        header.insert(headerList.at(i).first,headerList.at(i).second);
    }

    if(header.count()==0)//网络断开
    {

    }
    else
    {
        getWeaterSucs(true,Fn,allByte);
    }
    allByte.clear();
}

