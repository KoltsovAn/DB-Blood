#include "imagesetting.h"
#include "ui_imagesetting.h"

#include "QAbstractItemModel"

ImageSetting::ImageSetting(std::map<QString, int> *dataEnlargment, std::map<QString, int> *dataColor, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ImageSetting),
    Enlargment(dataEnlargment),
    Color(dataColor)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    for(const auto &p : *Color)
        ui->comboBox_2->addItem(p.first);
    for(const auto &p : *Enlargment)
        ui->comboBox_3->addItem(p.first);
    data = new QList<PatientImageInformation *>();

    this->setWindowModality(Qt::ApplicationModal);
}

ImageSetting::ImageSetting(std::map<QString, int> *dataEnlargment, std::map<QString, int> *dataColor,
                           QList<PatientImageInformation *> *_data, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ImageSetting),
    data(_data),
    Enlargment(dataEnlargment),
    Color(dataColor)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    ui->pushButton->hide();
    for(const auto &p : *Color)
        ui->comboBox_2->addItem(p.first);
    for(const auto &p : *Enlargment)
        ui->comboBox_3->addItem(p.first);

    ui->tableWidget->setColumnCount(data->size());
    ui->tableWidget->setRowCount(1);
    QTableWidgetItem *twi;
    for(int i = 0; i < data->size(); i++)
    {
        twi = new QTableWidgetItem;
        twi->setData(Qt::DecorationRole, QPixmap(data->at(i)->fileName).scaled(200, 200));
        ui->tableWidget->setItem(0, i, twi);
    }
    scene->clear();
    scene->addPixmap(QPixmap(data->at(0)->fileName));
    ui->comboBox_2->setCurrentText(data->at(0)->color);
    ui->comboBox_3->setCurrentText(data->at(0)->enlargment);
    ui->lineEdit->setText(data->at(0)->comment);
    ui->tableWidget->setCurrentCell(0, 0);

    this->setWindowModality(Qt::ApplicationModal);
}

ImageSetting::~ImageSetting()
{
    delete data;
    delete Enlargment;
    delete Color;
    delete ui;
}

void ImageSetting::on_pushButton_2_clicked()
{
    data->at(lastItemIndex)->idColor = Color->at(ui->comboBox_2->currentText());
    data->at(lastItemIndex)->color = ui->comboBox_2->currentText();
    data->at(lastItemIndex)->idEnlargment = Enlargment->at(ui->comboBox_3->currentText());
    data->at(lastItemIndex)->enlargment = ui->comboBox_3->currentText();
    data->at(lastItemIndex)->comment = ui->lineEdit->text();
    emit ImageData(data);
}

void ImageSetting::on_pushButton_clicked()
{
    QStringList fileName = QFileDialog::getOpenFileNames(this, tr("Open File"),
                                                    "/home",
                                                    tr("Images (*.png *.xpm *.jpg *.bmp)"));
    if(fileName.empty())
        return;
    ui->tableWidget->setColumnCount(fileName.size());
    ui->tableWidget->setRowCount(1);
    QTableWidgetItem *twi;
    for(int i = 0; i < fileName.size(); i++)
    {
        twi = new QTableWidgetItem;
        twi->setData(Qt::DecorationRole, QPixmap(fileName.at(i)).scaled(200, 200));
        ui->tableWidget->setItem(0, i, twi);
        data->push_back(new PatientImageInformation(fileName.at(i)));
    }
    scene->addPixmap(QPixmap(fileName.at(0)));
    ui->tableWidget->setCurrentCell(0, 0);
}

void ImageSetting::on_pushButton_3_clicked()
{
    emit closeWindow();
}

void ImageSetting::closeEvent(QCloseEvent *event)
{
    emit closeWindow();
    QWidget::closeEvent(event);
}

void ImageSetting::on_tableWidget_cellClicked(int row, int column)
{
    Q_UNUSED(row);
    if(data->empty())
        return;
    data->at(lastItemIndex)->idColor = Color->at(ui->comboBox_2->currentText());
    data->at(lastItemIndex)->color = ui->comboBox_2->currentText();
    data->at(lastItemIndex)->idEnlargment = Enlargment->at(ui->comboBox_3->currentText());
    data->at(lastItemIndex)->enlargment = ui->comboBox_3->currentText();
    data->at(lastItemIndex)->comment = ui->lineEdit->text();
    lastItemIndex = column;
    scene->clear();
    scene->addPixmap(QPixmap(data->at(column)->fileName));
    ui->comboBox_2->setCurrentText(data->at(column)->color);
    ui->comboBox_3->setCurrentText(data->at(column)->enlargment);
    ui->lineEdit->setText(data->at(column)->comment);
}
