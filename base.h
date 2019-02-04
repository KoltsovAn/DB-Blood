#ifndef BASE_H
#define BASE_H

#include <QObject>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QFile>
#include <QDir>
#include <QImage>

#include <map>
#include <utility>

#include "structsfordb.h"

#define DATABASE_HOSTNAME           "Blood"
#define DATABASE_NAME               "Blood.db"

#define TABLE_PATIENTS              "Patients"                      //+
#define TABLE_DIAGNOSIS             "Diagnosis"                     //manual input std::map<QString, int> *
#define TABLE_IMAGES                "Images"                        //+
#define TABLE_ENLARGMENT            "Enlargment"                    //manual input std::map<QString, int> *
#define TABLE_COLOR                 "Color"                         //manual input std::map<QString, int> *
#define TABLE_TYPECELL              "Cell"                          //manual input std::map<QString, int> *
#define TABLE_CELLINIMAGE           "CellInImage"                   //
#define TABLE_CELLCHARACTERISTICS   "CellCharacteristics"           //
#define TABLE_CHARACTERISTICS       "Characteristics"               //manual input std::map<int, std::map<QString, int> *> *
#define TABLE_SUBCHARACTERISTICS    "SubgroupCharacteristics"       //manual input std::map<int, std::map<QString, int> *> *
#define TABLE_GROUPCHARACTERISTICS  "GroupCharacteristics"          //manual input std::map<QString, int> *

class Base : public QObject
{
    Q_OBJECT
public:
    explicit Base(QObject *parent = nullptr);
    ~Base();

    void connectToDataBase();

    std::map<QString, int> *getQueryMap(const QString &TableName);
    std::map<int, std::map<QString, int> *> *getCharacteristics(const QString &TableName);

    //Работа с данными о пациенте
    bool addNewPatient(const PatientInformation *data);
    bool updatePatient(const PatientInformation *data);
    bool removePatient(int id);
    PatientInformation *GetPatientInformation(int id);

    //работа с данными изображения
    bool addNewImage(int id, QList<PatientImageInformation *> *data);
    bool updateImage(QList<PatientImageInformation *> *data);
    bool removeImage(int id_patient, int id_image);
    QList<PatientImageInformation *> *GetImageInformation(int id);
    QList<QTreeWidgetItem *> *getCellsOnImage(QString idImage);
    std::map<int, QList<QTreeWidgetItem *> *> *getAllCellCharacteristics(QString idImage);

    //работа с разметкой
    int saveCellMarker(const QImage image, int idImage, int idCell);
    int getNameMarkerCell(int idImage, int idCell);
    bool removeCellMarker(QString idMarker);

    // работа с признаками
    QList<QTreeWidgetItem *> *getCharacteristicsTree();
    QList<QTreeWidgetItem *> *getCharacteristicsTree(QString idCellInImage);
    bool saveCellCharacteristics(QString id, QList<QString> &data);
    bool removeCellCharacteristics(QString idCellInImage);

    //вставка в таблицы
    bool insertInTable(const QString &table, const QString &val);
    bool insertCharacteristics(const QString &table, const QString &idField, const QString &id,  const QString &val);

public slots:
    bool removeFromTable(const QString &table, int id);

private:
    // Сам объект базы данных, с которым будет производиться работа
    QSqlDatabase    db;

    // Внутренние методы для работы с базой данных
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createTable();

    void initialTable();


};

#endif // BASE_H
