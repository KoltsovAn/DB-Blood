#ifndef IMAGESETTING_H
#define IMAGESETTING_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QFileDialog>
#include <QPixmap>
#include <QBuffer>
#include <QFile>
#include <QTreeWidgetItem>
#include <QPair>
#include <QGraphicsPixmapItem>
#include <QTableWidgetItem>

#include "structsfordb.h"


#include <QDebug>

namespace Ui {
class ImageSetting;
}

class ImageSetting : public QDialog
{
    Q_OBJECT

public:
    explicit ImageSetting(std::map<QString, int> *dataEnlargment, std::map<QString, int> *dataColor, QWidget *parent = nullptr);
    explicit ImageSetting(std::map<QString, int> *dataEnlargment, std::map<QString, int> *dataColor,
                          QList<PatientImageInformation *> *_data, QWidget *parent = nullptr);
    ~ImageSetting();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();

    void on_tableWidget_cellClicked(int row, int column);

signals:
    void ImageData(QList<PatientImageInformation *> *data);
    void closeWindow();

private:
    Ui::ImageSetting *ui;

    QList<PatientImageInformation *> *data = nullptr;
    int lastItemIndex = 0;
    std::map<QString, int> *Enlargment = nullptr;
    std::map<QString, int> *Color = nullptr;

    QGraphicsScene *scene;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // IMAGESETTING_H

