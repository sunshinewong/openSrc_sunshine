#include "widget.h"
#include "ui_widget.h"
#include <QJSon/qjson.h>
#include <QJSon/serializer.h>
//http://www.weather.com.cn/data/sk/101010100.html
//"http://m.weather.com.cn/data/"+code+".html"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initCityMap();
    initLocation();
    //缓冲gif
    waitGif=new LoadWaitGif(this);
    waitGif->hide();
    //标志初始化
    type={false,false};
    //安装事件过滤器
    ui->groupBox->installEventFilter(this);
    //设置窗体表现形式
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool | Qt::WindowStaysOnTopHint );
    //http对象初始化
    weatherHttp=new http(this);
    connect(weatherHttp,SIGNAL(getWeaterSucs(bool,int,QByteArray&)),this,SLOT(slotGetWeaterSucs(bool,int,QByteArray&)));
    trayMenu=new QMenu(this);
    quitAction=new QAction(QIcon(":/icon/close.png"),tr("退出"),this);
    trayMenu->addAction(quitAction);
    connect(quitAction,SIGNAL(triggered()),this,SLOT(on_closeBtn_clicked()));
    //托盘图标初始化
    tray=new QSystemTrayIcon(this);
    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotSysTray(QSystemTrayIcon::ActivationReason)));
    tray->setToolTip(tr("天气预报"));
    tray->setIcon(QIcon(":/icon/tray.png"));
    tray->setContextMenu(trayMenu);
    tray->show();
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    timer->start(1000);
    ui->curDateTimeLabel->setText(QDateTime::currentDateTime().toString(tr("yyyy-MM-dd hh:mm:ss")));
    //开始请求默认地区的天气
    QTimer::singleShot(10,this,SLOT(slotInitCurTemp()));
}
Widget::~Widget()
{
    QString filename;
    filename=QDir::currentPath();
    filename.append(tr("/data/setting.ini"));
    QSettings settings(filename,QSettings::IniFormat);
    QTextCodec *codec=QTextCodec ::codecForName("utf-8");
    settings.setIniCodec(codec);
    settings.beginGroup("Location");
    settings.setValue("province",ui->provinceComboBox->currentText());
    settings.setValue("city",ui->cityComboBox->currentText());
    delete ui;
}
void Widget::initCityMap()
{
    //河北
    QString str,tStr;
    QStringList strlist, provinceList,list,tList;
    provinceList<<tr("北京")<<tr("上海")<<tr("天津")<<tr("河北")<<tr("河南")<<tr("安徽")<<tr("浙江")<<tr("重庆")<<tr("福建")
                <<tr("甘肃")<<tr("广东")<<tr("广西")<<tr("贵州")<<tr("云南")<<tr("内蒙古")<<tr("江西")<<tr("湖北")<<tr("四川")<<tr("宁夏")
                <<tr("青海")<<tr("山东")<<tr("陕西")<<tr("山西")<<tr("新疆")<<tr("西藏")<<tr("台湾")<<tr("海南")<<tr("湖南")<<tr("江苏")
                  <<tr("黑龙江")<<tr("吉林")<<tr("辽宁");
    strlist<<tr("北京:101010100;")
           <<tr("上海:101020100;")
           <<tr("天津:101030100;")
           <<tr("石家庄:101090101;张家口:101090301;承德:101090402;唐山:101090501;秦皇岛:101091101;沧州:101090701;衡水:101090801;邢台:101090901;邯郸:101091001;保定:101090201;廊坊:101090601;")
           <<tr("郑州:101180101;新乡:101180301;许昌:101180401;平顶山:101180501;信阳:101180601;南阳:101180701;开封:101180801;洛阳:101180901;商丘:101181001;焦作:101181101;鹤壁:101181201;濮阳:101181301;周口:101181401;漯河:101181501;驻马店:101181601;三门峡:101181701;济源:101181801;安阳:101180201;")
           <<tr("合肥:101220101;芜湖:101220301;淮南:101220401;马鞍山:101220501;安庆:101220601;宿州:101220701;阜阳:101220801;亳州:101220901黄山:101221001;滁州:101221101;淮北:101221201;铜陵:101221301;宣城:101221401;六安:101221501;巢湖:101221601;池州:101221701;蚌埠:101220201;")
           <<tr("杭州:101210101;舟山:101211101;湖州:101210201;嘉兴:101210301;金华:101210901;绍兴:101210501;台州:101210601;温州:101210701;丽水:101210801;衢州:101211001;宁波:101210401;")
           <<tr("重庆:101040100;")
           <<tr("福州:101230101;泉州:101230501;漳州:101230601;龙岩:101230701;晋江:101230509;南平:101230901;厦门:101230201;宁德:101230301;莆田:101230401;三明:101230801;")
           <<tr("兰州:101160101;平凉:101160301;庆阳:101160401;武威:101160501;金昌:101160601;嘉峪关:101161401;酒泉:101160801;天水:101160901;武都:101161001;临夏:101161101;合作:101161201;白银:101161301;定西:101160201;张掖:101160701;")
           <<tr("广州:101280101;惠州:101280301;梅州:101280401;汕头:101280501;深圳:101280601;珠海:101280701;佛山:101280800;肇庆:101280901;湛江:101281001;江门:101281101;河源:101281201;清远:101281301;云浮:101281401;潮州:101281501;东莞:101281601;中山:101281701;阳江:101281801;揭阳:101281901;茂名:101282001;汕尾:101282101;韶关:101280201;")
           <<tr("南宁:101300101;柳州:101300301;来宾:101300401;桂林:101300501;梧州:101300601;防城港:101301401;贵港:101300801;玉林:101300901;百色:101301001;钦州:101301101;河池:101301201;北海:101301301;崇左:101300201;贺州:101300701;")
           <<tr("贵阳:101260101;安顺:101260301;都匀:101260401;兴义:101260906;铜仁:101260601;毕节:101260701;六盘水:101260801;遵义:101260201;凯里:101260501;")
           <<tr("昆明:101290101;红河:101290301;文山:101290601;玉溪:101290701;楚雄:101290801;普洱:101290901;昭通:101291001;临沧:101291101;怒江:101291201;香格里拉:101291301;丽江:101291401;德宏:101291501;景洪:101291601;大理:101290201;曲靖:101290401;保山:101290501;")
           <<tr("呼和浩特:101080101;乌海:101080301;集宁:101080401;通辽:101080501;阿拉善左旗:101081201;鄂尔多斯:101080701;临河:101080801;锡林浩特:101080901;呼伦贝尔:101081000;乌兰浩特:101081101;包头:101080201;赤峰:101080601;")
           <<tr("南昌:101240101;上饶:101240301;抚州:101240401;宜春:101240501;鹰潭:101241101;赣州:101240701;景德镇:101240801;萍乡:101240901;新余:101241001;九江:101240201;吉安:101240601;")
           <<tr("武汉:101200101;黄冈:101200501;荆州:101200801;宜昌:101200901;恩施:101201001;十堰:101201101;神农架:101201201;随州:101201301;荆门:101201401;天门:101201501;仙桃:101201601;潜江:101201701;襄樊:101200201;鄂州:101200301;孝感:101200401;黄石:101200601;咸宁:101200701;")
           <<tr("成都:101270101;自贡:101270301;绵阳:101270401;南充:101270501;达州:101270601;遂宁:101270701;广安:101270801;巴中:101270901;泸州:101271001;宜宾:101271101;内江:101271201;资阳:101271301;乐山:101271401;眉山:101271501;凉山:101271601;雅安:101271701;甘孜:101271801;阿坝:101271901;德阳:101272001;广元:101272101;攀枝花:101270201;")
           <<tr("银川:101170101;中卫:101170501;固原:101170401;石嘴山:101170201;吴忠:101170301;")
           <<tr("西宁:101150101;黄南:101150301;海北:101150801;果洛:101150501;玉树:101150601;海西:101150701;海东:101150201;海南:101150401;")
           <<tr("济南:101120101;潍坊:101120601;临沂:101120901;菏泽:101121001;滨州:101121101;东营:101121201;威海:101121301;枣庄:101121401;日照:101121501;莱芜:101121601;聊城:101121701;青岛:101120201;淄博:101120301;德州:101120401;烟台:101120501;济宁:101120701;泰安:101120801;")
           <<tr("西安:101110101;延安:101110300;榆林:101110401;铜川:101111001;商洛:101110601;安康:101110701;汉中:101110801;宝鸡:101110901;咸阳:101110200;渭南:101110501;")
           <<tr("太原:101100101;临汾:101100701;运城:101100801;朔州:101100901;忻州:101101001;长治:101100501;大同:101100201;阳泉:101100301;晋中:101100401;晋城:101100601;吕梁:101101100;")
           <<tr("乌鲁木齐:101130101;石河子:101130301;昌吉:101130401;吐鲁番:101130501;库尔勒:101130601;阿拉尔:101130701;阿克苏:101130801;喀什:101130901;伊宁:101131001;塔城:101131101;哈密:101131201;和田:101131301;阿勒泰:101131401;阿图什:101131501;博乐:101131601;克拉玛依:101130201;")
           <<tr("拉萨:101140101;山南:101140301;阿里:101140701;昌都:101140501;那曲:101140601;日喀则:101140201;林芝:101140401;")
           <<tr("台北县:101340101;高雄:101340201;台中:101340401;")
           <<tr("海口:101310101;三亚:101310201;东方:101310202;临高:101310203;澄迈:101310204;儋州:101310205;昌江:101310206;白沙:101310207;琼中:101310208;定安:101310209;屯昌:101310210琼海:101310211;文昌:101310212;保亭:101310214;万宁:101310215;陵水:101310216;西沙:101310217;南沙岛:101310220;乐东:101310221;五指山:101310222;琼山:101310102;")
           <<tr("长沙:101250101;株洲:101250301;衡阳:101250401;郴州:101250501;常德:101250601;益阳:101250700;娄底:101250801;邵阳:101250901;岳阳:101251001;张家界:101251101;怀化:101251201;黔阳:101251301;永州:101251401;吉首:101251501;湘潭:101250201;")
           <<tr("南京:101190101;镇江:101190301;苏州:101190401;南通:101190501;扬州:101190601;宿迁:101191301;徐州:101190801;淮安:101190901;连云港:101191001;常州:101191101;泰州:101191201;无锡:101190201;盐城:101190701;")
           <<tr("哈尔滨:101050101;牡丹江:101050301;佳木斯:101050401;绥化:101050501;黑河:101050601;双鸭山:101051301;伊春:101050801;大庆:101050901;七台河:101051002;鸡西:101051101;鹤岗:101051201;齐齐哈尔:101050201;大兴安岭:101050701;")
           <<tr("长春:101060101;延吉:101060301;四平:101060401;白山:101060901;白城:101060601;辽源:101060701;松原:101060801;吉林:101060201;通化:101060501;")
           <<tr("沈阳:101070101;鞍山:101070301;抚顺:101070401;本溪:101070501;丹东:101070601;葫芦岛:101071401;营口:101070801;阜新:101070901;辽阳:101071001;铁岭:101071101;朝阳:101071201;盘锦:101071301;大连:101070201;锦州:101070701;");

    for(int i=0;i<provinceList.count();i++)
    {
        str=strlist.at(i);
        list=str.split(tr(";"));
        for(int j=0;j<list.count();j++)
        {
            tStr=list.at(j);
            tList=tStr.split(tr(":"));
            if(tList.count()==2)
            {
                codeMap.insert(tList.at(0),tList.at(1));
                location.insert(provinceList.at(i),tList.at(0));
            }
        }
    }
}
void Widget::initLocation()
{
    isInit=true;
    QString filename;
    filename=QDir::currentPath();
    filename.append(tr("/data/setting.ini"));
    QSettings settings(filename,QSettings::IniFormat);
    QTextCodec *codec=QTextCodec ::codecForName("utf-8");
    settings.setIniCodec(codec);
    settings.beginGroup("Location");
    QString province=settings.value("province").toString();
    QString city=settings.value("city").toString();
    QStringList provinceList,cityList;
    provinceList<<tr("北京")<<tr("上海")<<tr("天津")<<tr("河北")<<tr("河南")<<tr("安徽")<<tr("浙江")<<tr("重庆")<<tr("福建")
                <<tr("甘肃")<<tr("广东")<<tr("广西")<<tr("贵州")<<tr("云南")<<tr("内蒙古")<<tr("江西")<<tr("湖北")<<tr("四川")<<tr("宁夏")
                <<tr("青海")<<tr("山东")<<tr("陕西")<<tr("山西")<<tr("新疆")<<tr("西藏")<<tr("台湾")<<tr("海南")<<tr("湖南")<<tr("江苏")
                  <<tr("黑龙江")<<tr("吉林")<<tr("辽宁");
    ui->provinceComboBox->addItems(provinceList);
    int index=provinceList.indexOf(province);
    qDebug()<<tr("aaaa")<<index<<province;
    if(index>=0)
        ui->provinceComboBox->setCurrentIndex(index);
    else
        ui->provinceComboBox->setCurrentIndex(0);
    cityList=location.values(province);
    if(cityList.count()>0)
        ui->cityComboBox->addItems(cityList);
    index=cityList.indexOf(city);
    if(index>=0)
        ui->cityComboBox->setCurrentIndex(index);
    else
        ui->cityComboBox->setCurrentIndex(0);
    isInit=false;

}
void Widget::slotInitCurTemp()
{
    //获取位置
    getWeather(2);
}
void Widget::getWeather(int fn)
{
    QString code=codeMap.value(ui->cityComboBox->currentText());
    if(fn==1)
    {
        weatherHttp->get("http://m.weather.com.cn/data/"+code+".html",1);
        showGif();
    }
    else
       weatherHttp->get("http://www.weather.com.cn/data/sk/"+code+".html",2);
}
void Widget::slotGetWeaterSucs(bool isSucs,int fn,QByteArray &bytes)
{
    if(isSucs)
    {
        if(fn==1)
            type.f1=true;
        else if(fn==2)
            type.f2=true;
        contents.insert(fn,bytes);
        QTimer::singleShot(10,this,SLOT(slotMangerInfo()));
    }
}
void Widget::slotMangerInfo()
{
    QJson::QJson parser;
    bool ok;
    if(type.f1)
    {
        QVariant result = parser.parse(contents.value(1), &ok);
        qDebug()<<tr("aaa")<<contents.value(1);
        if(ok)
        {
           QVariantMap mylist = result.toMap();
           foreach (QVariant plugin, mylist)
           {
              QVariantMap mymap = plugin.toMap();
              ui->qzTempLabel->setText(mymap["temp1"].toString());
              ui->dayTitleLabel->setText(mymap["img_title1"].toString());
              ui->neightTitleLabel->setText(mymap["img_title2"].toString());
              ui->curTitleLabel->setText(mymap["img_title_single"].toString());
              int t;
              QTime curTime=QTime::currentTime();
              if(curTime>=QTime(6,0,0)&&curTime<=QTime(18,0,0))
                  t=1;
              else
                  t=2;
              QString pathName;
              pathName=findIconPath(mymap["img_single"].toString(),t);
              ui->curIconLabel->setPixmap(QPixmap(pathName));
              pathName=findIconPath(mymap["img1"].toString(),1);
              ui->dayIconLabel->setPixmap(QPixmap(pathName));
              pathName=findIconPath(mymap["img2"].toString(),2);
              ui->neightIconLabel->setPixmap(QPixmap(pathName));
              QGroupBox *groupBox[6]={ui->groupBox_1,ui->groupBox_2,ui->groupBox_3,ui->groupBox_4,ui->groupBox_5,ui->groupBox_6};
              QLabel *dateLabelList[6]={ui->dateLabel,ui->dateLabel_2,ui->dateLabel_3,ui->dateLabel_4,ui->dateLabel_5,ui->dateLabel_6};
              QLabel *tempLabelList[6]={ui->tempLabel,ui->tempLabel_2,ui->tempLabel_3,ui->tempLabel_4,ui->tempLabel_5,ui->tempLabel_6};
              QLabel *weatherLabelList[6]={ui->weatherLabel,ui->weatherLabel_2,ui->weatherLabel_3,ui->weatherLabel_4,ui->weatherLabel_5,ui->weatherLabel_6};
              QLabel *windLabelList[6]={ui->winLabel,ui->winLabel_2,ui->winLabel_3,ui->winLabel_4,ui->winLabel_5,ui->winLabel_6};
              QDate date=QDate::currentDate();
              for(int i=0;i<6;i++)
              {
                  groupBox[i]->setTitle(getWeeks(date.dayOfWeek()));
                  dateLabelList[i]->setText(date.toString(tr("yyyy年MM月dd日")));
                  tempLabelList[i]->setText(mymap[tr("temp%1").arg(i+1)].toString());
                  weatherLabelList[i]->setText(mymap[tr("weather%1").arg(i+1)].toString());
                  windLabelList[i]->setText(mymap[tr("wind%1").arg(i+1)].toString());
                  date=date.addDays(1);
              }
              QLabel *indexLabelList[9]={ui->indexLabel,ui->indexLabel_2,ui->indexLabel_3,ui->indexLabel_4,ui->indexLabel_5,ui->indexLabel_6,ui->indexLabel_7,ui->indexLabel_8,ui->indexLabel_9};
              QStringList list;
              list<<"index_d"<<"index48_d"<<"index_uv"<<"index_xc"<<"index_tr"<<"index_co"<<"index_cl"<<"index_ls"<<"index_ag";
              for(int i=0;i<9;i++)
              {
                  indexLabelList[i]->setText(mymap[list.at(i)].toString());
              }
           }
        }
        type.f1=false;
        waitGif->hideGif();
    }
    if(type.f2)
    {
        QVariant result = parser.parse(contents.value(2), &ok);
        if(ok)
        {
           QVariantMap mylist = result.toMap();
           foreach (QVariant plugin, mylist)
           {
             QVariantMap mymap = plugin.toMap();
             ui->cityLabel->setText(mymap["city"].toString());
             ui->curTempLabel->setText(mymap["temp"].toString()+"℃");
             ui->windLabel->setText(mymap["WD"].toString()+mymap["WS"].toString());
             ui->sdLabel->setText(tr("湿度")+mymap["SD"].toString());
           }
        }
        type.f2=false;
        //获取详细信息
        getWeather(1);
    }
}
void Widget::showGif()
{
    int x,y;
    x=(this->width()-waitGif->width())/2;
    y=(this->height()-waitGif->height())/2;
    waitGif->move(x,y);
    waitGif->showGif();
}
void Widget::on_provinceComboBox_currentIndexChanged(const QString &arg1)
{
    QStringList cityList=location.values(arg1);
    ui->cityComboBox->clear();
    if(cityList.count()>0)
        ui->cityComboBox->addItems(cityList);
    ui->cityComboBox->setCurrentIndex(0);
}
//切换了城市后获取最新的天气
void Widget::on_cityComboBox_currentIndexChanged(int index)
{
    index;
    if(!isInit)
        getWeather(2);
}
//设置天气图标 返回图标的路径(type=1:白天 type=2:夜间)
QString Widget::findIconPath(QString ind,int type)
{
    QString path;
    int index=ind.toInt();
    if((index>=0&&index<=4)||index==13)
    {
       if(type==1)
           path=tr(":/icon/d%1.png").arg(index);
       else if(type==2)
           path=tr(":/icon/n%1.png").arg(index);
    }
    else if((index>=5&&index<=12)||(index>=14&&index<=20)||index==31)
        path=tr(":/icon/b%1.png").arg(index);
    else  if(index>=21&&index<=25)
        path=tr(":/icon/b%1.png").arg(index-13);
    else if(index>=26&&index<=28)
        path=tr(":/icon/b%1.png").arg(index-11);
    else if(index>=29&&index<=30)
        path=tr(":/icon/b29.png");
    else
        path=tr(":/icon/na.png");
    return path;
}
//托盘图标响应
void Widget::slotSysTray(QSystemTrayIcon::ActivationReason activation)
{
    if(activation==QSystemTrayIcon::DoubleClick||activation==QSystemTrayIcon::DoubleClick)
    {
        if(this->isHidden())
            this->show();
        else
            this->hide();
    }
}
//刷新最新的天气
void Widget::on_refreshBtn_clicked()
{
    getWeather(2);
}

void Widget::on_closeBtn_clicked()
{
    this->close();
    QApplication::quit();
}
//最小化即隐藏
void Widget::on_minBtn_clicked()
{
    this->hide();
}
//自定义显示函数
void Widget::myShow()
{
    QDesktopWidget desktopWidget;
    QRect rect=desktopWidget.availableGeometry ();
    move(rect.width()-this->width(),0);
    this->show();
}
QString Widget::getWeeks(int wek)
{
    QString weekday;
    switch(wek)
    {
    case 1:weekday=tr("星期一");
        break;
    case 2:weekday=tr("星期二");
        break;
    case 3:weekday=tr("星期三");
        break;
    case 4:weekday=tr("星期四");
        break;
    case 5:weekday=tr("星期五");
        break;
    case 6:weekday=tr("星期六");
        break;
    case 7:weekday=tr("星期天");
        break;
    default:break;
    }
    return weekday;
}
void Widget::slotTimeOut()
{
    ui->curDateTimeLabel->setText(QDateTime::currentDateTime().toString(tr("yyyy-MM-dd hh:mm:ss")));
    timer->start(1000);
}
//事件过滤器
bool Widget::eventFilter(QObject *target, QEvent *e)
{
    QMouseEvent *mouseEvent=(QMouseEvent *)e;
    if(e->type()==QEvent::MouseButtonPress)
    {
        if(target==ui->groupBox)
        {
            // pos()是相对于窗口的，以窗口左上角为原点（去除边框）。即pos()给出的是一个相对位置坐标。而globalPos(),给出的是一个绝对坐标
            this->windowPos =this->frameGeometry().topLeft();
            //globalpos()给出的坐标信息是相对于桌面的，即以桌面左上角为原点
            this->mousePos=mouseEvent->globalPos();
            this->dPos =mousePos-windowPos;
        }
        return true;
    }
    else if(e->type()==QEvent::MouseMove)
    {
        if(target==ui->groupBox)
        {
            this->move(mouseEvent->globalPos()-this->dPos);
        }
        return true;
    }
    return QWidget::eventFilter(target, e);
}

