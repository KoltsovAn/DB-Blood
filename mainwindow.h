#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlRelationalDelegate>
#include <QMessageBox>
#include <QModelIndex>

#include "base.h"
#include "patientsetting.h"
#include "imagesetting.h"
#include "markerwindow.h"
#include "paintscene.h"
#include "tablesetting.h"
#include "characteristicssetting.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //кнопки для работы с пациентом
    void on_pushButton_clicked();       //добавить пациента

    void on_pushButton_2_clicked();     //редактировать пациента

    void on_pushButton_3_clicked();     //удалить пациента

    //работа с данными пациента
    void setNewPatientData(const PatientInformation *data);
    void UpdatePatient(const PatientInformation *data);
    void closePatientSetting();

    //кнопки для работы с изображением
    void on_pushButton_4_clicked();     //добавить изображения

    //работа с изображениями
    void setNewImage(QList<PatientImageInformation *> *data);
    void updateImage(QList<PatientImageInformation *> *data);
    void closeImageSetting();

    //настройки разметки
    void closeMarkerWindow();
    void saveMarker(int idCell);
    void setColorMarker(int i);
    void setDepthMarker(const QString &arg);
    void setFigureMarker(int i);
    void typeCellChange(int i);
    void removeLastItem();
    void removeAll();
    void restore();

    //настройка таболица
    void closeTableSetting();
    bool addInTable(const QString &table, const QString &val);
    bool addCharacteristics(const QString &table, const QString &idField, const QString &id, const QString &val);

    //характеристики
    void closeCharacteristicsWindow();
    void setCharacteristics(QList<QString> &characteristics);

    void on_tableView_clicked(const QModelIndex &index);

    void on_tabWidget_currentChanged(int index);

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_action_triggered();

    void on_pushButton_8_clicked();

private:
    Ui::MainWindow *ui;

    Base *db;
    QSqlRelationalTableModel *model;

    PatientSetting *PS = nullptr;
    ImageSetting *IS = nullptr;
    MarkerWindow *Marker = nullptr;
    TableSetting *TS = nullptr;
    CharacteristicsSetting *CW = nullptr;

    int id_Patient = -1;
    int id_Image = -1;


    QList<PatientImageInformation *> *patientImage = new QList<PatientImageInformation *>();

    PaintScene *sceneImage;
    QGraphicsPixmapItem *pmInScene = nullptr;

    void setupModel();
    void createUI();
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
