#include "markerwindow.h"
#include "ui_markerwindow.h"

#include <QDebug>

MarkerWindow::MarkerWindow(std::map<QString, int> *dataCell, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarkerWindow),
    typeCell(dataCell)
{
    ui->setupUi(this);

    initialColor();
    ui->comboBox_2->setCurrentIndex(2);

    for(const auto &p : *typeCell)
        ui->comboBox_4->addItem(p.first);

    this->setWindowFlags(Qt::Tool);
//    this->setWindowFlag(Qt::WindowCloseButtonHint, false);

    QTimer::singleShot(200, this, SLOT(update()));
}

MarkerWindow::~MarkerWindow()
{
    delete typeCell;
    delete ui;
}

int MarkerWindow::getTypeFigure()
{
    return ui->comboBox->currentIndex();
}

void MarkerWindow::initialColor()
{
    QPixmap pm(20,20);
    pm.fill(Qt::black);
    ui->comboBox_3->addItem(QIcon(pm), QString("Черный"));
    pm.fill(Qt::white);
    ui->comboBox_3->addItem(QIcon(pm), QString("Белый"));
    pm.fill(Qt::red);
    ui->comboBox_3->addItem(QIcon(pm), QString("Красный"));
    pm.fill(Qt::green);
    ui->comboBox_3->addItem(QIcon(pm), QString("Зеленный"));
    pm.fill(Qt::blue);
    ui->comboBox_3->addItem(QIcon(pm), QString("Синий"));
    pm.fill(Qt::cyan);
    ui->comboBox_3->addItem(QIcon(pm), QString("Голубой"));
    pm.fill(Qt::magenta);
    ui->comboBox_3->addItem(QIcon(pm), QString("Фиолетовый"));
    pm.fill(Qt::yellow);
    ui->comboBox_3->addItem(QIcon(pm), QString("Желтый"));
    pm.fill(Qt::gray);
    ui->comboBox_3->addItem(QIcon(pm), QString("Серый"));
}

void MarkerWindow::on_comboBox_currentIndexChanged(int index)
{
    emit figureChange(index);
}

void MarkerWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    emit depthChange(arg1);
}

void MarkerWindow::on_comboBox_3_currentIndexChanged(int index)
{
    emit colorChange(index);
}

void MarkerWindow::closeEvent(QCloseEvent *event)
{
    emit closeWindow();
    QDialog::closeEvent(event);
}

void MarkerWindow::on_pushButton_5_clicked()
{
    emit closeWindow();
}

void MarkerWindow::on_pushButton_clicked()
{
    emit removeLastItemButton();
}

void MarkerWindow::on_pushButton_2_clicked()
{
    emit removeAllButton();
}

void MarkerWindow::on_pushButton_3_clicked()
{
    emit restoreButton();
}

void MarkerWindow::on_pushButton_4_clicked()
{
    emit saveMarker(typeCell->at(ui->comboBox_4->currentText()));
}

void MarkerWindow::on_comboBox_4_currentTextChanged(const QString &arg1)
{
    emit typeCellChange(typeCell->at(arg1));
}

void MarkerWindow::update()
{
    this->setWindowModality(Qt::WindowModal);
    this->on_comboBox_4_currentTextChanged(ui->comboBox_4->currentText());
}
