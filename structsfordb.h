#ifndef STRUCTSFORDB_H
#define STRUCTSFORDB_H

#include <QString>
#include <QList>
#include <QTreeWidgetItem>
#include <QDate>
#include <map>


struct PatientImageInformation
{
    PatientImageInformation(){}
    PatientImageInformation(QString _fileName) : fileName(_fileName){}
    PatientImageInformation(int _id, int _idPatient, int _idEnlargment, int _idColor,
                            QString _fileName, QString _enlargment, QString _color, QString _comment,
                            QList<QTreeWidgetItem *> *_itemsCell,
                            std::map<int, QList<QTreeWidgetItem *> *> *_cellCharacteristics) :
        id(_id), idPatient(_idPatient), idEnlargment(_idEnlargment), idColor(_idColor),
        fileName(_fileName), enlargment(_enlargment), color(_color), comment(_comment),
        itemsCell(_itemsCell), cellCharacteristics(_cellCharacteristics){}

    int id = 0;
    int idPatient = 0;
    int idEnlargment = 0;
    int idColor = 0;
    QString fileName = "";
    QString enlargment = "";
    QString color = "";
    QString comment = "";

    QList<QTreeWidgetItem *> *itemsCell = new QList<QTreeWidgetItem *>();
    std::map<int, QList<QTreeWidgetItem *> *> *cellCharacteristics =
            new std::map<int, QList<QTreeWidgetItem *> *>;
};

struct PatientInformation
{
    PatientInformation(){}
    PatientInformation(int _id, int _id_diagnos, QString _NumberCard,
                       QString _Name, QString _Diagnos, QDate _DateMaterial) :
        id(_id), id_diagnos(_id_diagnos), NumberCard(_NumberCard),
        Name(_Name), Diagnos(_Diagnos), DateMaterial(_DateMaterial){}
    int id = 0;
    int id_diagnos = 0;
    QString NumberCard = "";
    QString Name = "";
    QString Diagnos = "";
    QDate DateMaterial = QDate();
};

#endif // STRUCTSFORDB_H
