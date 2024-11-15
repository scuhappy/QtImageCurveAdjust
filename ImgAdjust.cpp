#include "ImgAdjust.h"
#include "ui_ImgAdjust.h"
#include "ImageCureAdjustControl/ImageCureAdjustControl.h"
#include "ImageCureAdjustControl/ImageTools.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>

ImgAdjust::ImgAdjust(QWidget *parent) :
    QMainWindow(parent),adjCtr(nullptr),
    ui(new Ui::ImgAdjust)
{
    ui->setupUi(this);
    connect(ui->actionCurve_Adjust,&QAction::triggered,this,&ImgAdjust::OnAdjust);
    connect(ui->actionOpen,&QAction::triggered,this,&ImgAdjust::OnOpen);
    connect(ui->actionSave,&QAction::triggered,this,&ImgAdjust::OnSave);
    connect(ui->actionSaveLUT,&QAction::triggered,this,&ImgAdjust::OnSaveLUT);
}

ImgAdjust::~ImgAdjust()
{
    delete ui;
}
void ImgAdjust::OnSaveLUT(){
    QUrl filePath = QFileDialog::getSaveFileUrl(this,tr("Image"),QDir::homePath(),tr("Images (*.png *.jpg *.jpeg)"));
    qDebug()<<filePath.path();
    QImage lut(255,1,QImage::Format_RGBA8888);
    std::vector<unsigned char> lookUpTable = adjCtr->GetColorLookUpTable(ICAChannel::eChannelAll);
    QColor color;
    for(int i=0;i<255;i++){
        qDebug()<<"lookUpTable[i] "<<lookUpTable[i];
        color.setRed(lookUpTable[i]);
        color.setGreen(lookUpTable[i]);
        color.setBlue(lookUpTable[i]);
        color.setAlpha(255);
        lut.setPixelColor(i,0,color);
    }
    lut.save(filePath.path());
}
void ImgAdjust::OnSave(){
    QUrl filePath = QFileDialog::getSaveFileUrl(this,tr("Image"),QDir::homePath(),tr("Images (*.png *.jpg *.jpeg)"));
    qDebug()<<filePath.path();
    showImg.save(filePath.path());
}
void ImgAdjust::OnAdjust()
{
    ui->label->setPixmap(QPixmap::fromImage(m_img));
    if(adjCtr == nullptr){
        adjCtr = new ImageCureAdjustControl(this);
        adjCtr->setWindowFlag(Qt::Tool);
        adjCtr->setFixedSize(430, 365);
        adjCtr->setAttribute(Qt::WA_DeleteOnClose);

        if(adjCtr->Init(m_img))
        {
            adjCtr->show();
            connect(adjCtr,&ImageCureAdjustControl::SigCurveChanged,this,&ImgAdjust::OnRefresh);
            connect(adjCtr,&ImageCureAdjustControl::SigChannelModeChanged,this,&ImgAdjust::OnRefresh);
            connect(adjCtr,&ImageCureAdjustControl::SigChannelChanged,this,&ImgAdjust::OnRefresh);
        }
    }
    else
    {
       adjCtr->show();
    }
}

void ImgAdjust::OnRefresh()
{
    ImageCureAdjustControl *adjCtr = static_cast<ImageCureAdjustControl*>(sender());
    showImg = m_img;
    ICAChannelMode mode = adjCtr->GetAdjustChannelMode();


    switch (mode)
    {
    case ICAChannelMode::eChannelModeAll:
    {
        std::vector<unsigned char> lookUpTable = adjCtr->GetColorLookUpTable(ICAChannel::eChannelAll);
        ImageTools::ImgCurveAdjust(showImg,lookUpTable,ICAChannel::eChannelAll);
        break;
    }
    case ICAChannelMode::eChannelModeSingle:
    {
        ICAChannel channel = adjCtr->GetCurrentChannel();
        std::vector<unsigned char> lookUpTable = adjCtr->GetColorLookUpTable(channel);
        ImageTools::ImgCurveAdjust(showImg,lookUpTable,channel);
        break;
    }
    case ICAChannelMode::eChannelModeMulti:
    {
        std::vector<unsigned char> lookUpTable = adjCtr->GetColorLookUpTable(ICAChannel::eChannelR);
        ImageTools::ImgCurveAdjust(showImg,lookUpTable,ICAChannel::eChannelR);
        lookUpTable = adjCtr->GetColorLookUpTable(ICAChannel::eChannelG);
        ImageTools::ImgCurveAdjust(showImg,lookUpTable,ICAChannel::eChannelG);
        lookUpTable = adjCtr->GetColorLookUpTable(ICAChannel::eChannelB);
        ImageTools::ImgCurveAdjust(showImg,lookUpTable,ICAChannel::eChannelB);
    }
    }
    ui->label->setPixmap(QPixmap::fromImage(showImg));

}

void ImgAdjust::OnOpen()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Image"));
    QFile file(filePath);
    if(file.open(QFile::ReadOnly))
    {
        QByteArray buf = file.readAll();
        if(m_img.loadFromData(buf))
        {
            if(m_img.format() == QImage::Format_Indexed8)
            {
                m_img = m_img.convertToFormat(QImage::Format_Grayscale8);
            }

            ui->label->setPixmap(QPixmap::fromImage(m_img));
        }
        else
        {
            ui->label->clear();
        }
    }
}
