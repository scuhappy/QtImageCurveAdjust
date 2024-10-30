#ifndef IMGADJUST_H
#define IMGADJUST_H

#include <QMainWindow>
#include <QImage>

namespace Ui {
class ImgAdjust;
}
class ImageCureAdjustControl;
class ImgAdjust : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImgAdjust(QWidget *parent = nullptr);
    ~ImgAdjust();

private slots:
    void OnAdjust();
    void OnRefresh();
    void OnOpen();
    void OnSave();
    void OnSaveLUT();
private:
    Ui::ImgAdjust *ui;

    QImage m_img;
    QImage showImg;
    ImageCureAdjustControl *adjCtr;
};

#endif // IMGADJUST_H
