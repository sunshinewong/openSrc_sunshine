#include "loadwaitgif.h"
#include <QDebug>
LoadWaitGif::LoadWaitGif(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("报关云助手"));
    giflLabel=new QLabel(this);
    movie = new QMovie("./gif/loadwait.gif");
    giflLabel->setMovie(movie);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowModality(Qt::ApplicationModal);
    //setWindowState(Qt::WindowNoState);
    setWindowOpacity(0.5);
    setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(giflLabel);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setMargin(0);
    this->resize(124,124);
}

LoadWaitGif::~LoadWaitGif()
{
    movie->stop();
    if(movie!=NULL)
        delete movie;
}

void LoadWaitGif::showGif()
{
    movie->start();
    this->show();
}


void LoadWaitGif::hideGif()
{
    hide();
    movie->stop();
}

