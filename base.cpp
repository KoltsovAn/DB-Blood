#include "base.h"
#include <QDebug>

Base::Base(QObject *parent) : QObject(parent)
{

}

Base::~Base()
{

}

void Base::connectToDataBase()
{
    /* Перед подключением к базе данных производим проверку на её существование.
     * В зависимости от результата производим открытие базы данных или её восстановление
     * */
    if(!QFile(".\\" DATABASE_NAME).exists()){
        this->restoreDataBase();
    } else {
        this->openDataBase();
    }
}

std::map<QString, int> *Base::getQueryMap(const QString &TableName)
{
    QSqlQuery query;
    std::map<QString, int> *data = new std::map<QString, int>();
    if(!query.exec("SELECT * FROM " + TableName))
        query.lastError();
    else
        while(query.next())
            data->emplace(std::make_pair(query.value(1).toString(), query.value(0).toInt()));
    return data;
}

std::map<int, std::map<QString, int> *> *Base::getCharacteristics(const QString &TableName)
{
    QSqlQuery query;
    std::map<int, std::map<QString, int> *> *data = new std::map<int, std::map<QString, int> *>();

    if(!query.exec("SELECT * FROM " + TableName))
        query.lastError();
    else
        while (query.next()) {
            auto itr (data->find(query.value(1).toInt()));
            if(itr != data->end()){
                itr->second->emplace(std::make_pair(query.value(2).toString(),query.value(0).toInt()));
            } else {
                std::map<QString, int> *dataCharacteristics = new std::map<QString, int>();
                dataCharacteristics->emplace(std::make_pair(query.value(2).toString(),query.value(0).toInt()));
                data->emplace(std::make_pair(query.value(1).toInt(), dataCharacteristics));
            }
        }
    return data;
}

bool Base::addNewPatient(const PatientInformation *data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO " TABLE_PATIENTS " (NumberCard, Name, id_" TABLE_DIAGNOSIS ", DateMaterial, NumImages)"
                  "VALUES (:NumberCard, :Name, :id_diagnos, :DateMaterial, :NumImages)");
    //связываем значения которые будем добавлять с условным обозначением
    query.bindValue(":NumberCard", data->NumberCard);
    query.bindValue(":Name", data->Name);
    query.bindValue(":id_diagnos", data->id_diagnos);
    query.bindValue(":DateMaterial", data->DateMaterial.toString("yyyy-MM-dd"));
    query.bindValue(":NumImages", 0);
    //добавление нового пациента
    if(!query.exec()){
        qDebug() << "Error insert new patient";
        qDebug() << query.lastError();
        return false;
    } else
        return true;
}

bool Base::updatePatient(const PatientInformation *data)
{
    QSqlQuery query;

    query.prepare("UPDATE " TABLE_PATIENTS " SET NumberCard = :NumberCard, Name = :Name,id_" TABLE_DIAGNOSIS " = :id_diagnos,"
                                           " DateMaterial = :DateMaterial WHERE id = :id");
    query.bindValue(":id", data->id);
    query.bindValue(":NumberCard", data->NumberCard);
    query.bindValue(":Name", data->Name);
    query.bindValue(":id_diagnos", data->id_diagnos);
    query.bindValue(":DateMaterial", data->DateMaterial.toString("yyyy-MM-dd"));
    if(!query.exec()){
        qDebug() << "Error update patient";
        qDebug() << query.lastError();
        return false;
    } else{
        qDebug() << "Update patient compleate";
        return true;
    }
}

bool Base::removePatient(int id)
{
    QSqlQuery query;

    query.prepare("DELETE FROM " TABLE_PATIENTS " WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << "Error remove patient";
        qDebug() << query.lastError();
        return false;
    }
    query.prepare("SELECT id FROM " TABLE_IMAGES " WHERE id_" TABLE_PATIENTS " = :id");
    query.bindValue(":id", id);
    query.exec();
    while(query.next())
        this->removeImage(id, query.value(0).toInt());
    return true;
}

PatientInformation *Base::GetPatientInformation(int id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM " TABLE_PATIENTS " "
                  "LEFT JOIN " TABLE_DIAGNOSIS " ON " TABLE_PATIENTS ".id_" TABLE_DIAGNOSIS " = " TABLE_DIAGNOSIS ".id "
                  "WHERE " TABLE_PATIENTS ".id = :id ");
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << "Error get information about patient";
        qDebug() << query.lastError();
    } else
        while(query.next()){
//            qDebug() << query.value(0).toInt();
            return new PatientInformation(query.value(0).toInt(), query.value(3).toInt(),
                                          query.value(1).toString(), query.value(2).toString(),
                                          query.value(7).toString(), query.value(4).toDate());
        }
    return new PatientInformation();
}

bool Base::addNewImage(int id, QList<PatientImageInformation *> *data)//добавление изображения пациенту
{
    QSqlQuery query;
    //количество имеющийся изображений пациента
    query.prepare("SELECT NumImages FROM " TABLE_PATIENTS " WHERE id = :id" );
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << "Error insert new image";
        qDebug() << query.lastError();
        return false;
    }
    int number = 0;
    while (query.next())
        number = query.value(0).toInt();
    number += data->size();
    //обновляем количество изображений у пациента
    query.prepare("UPDATE " TABLE_PATIENTS " SET NumImages = :number WHERE id = :id");
    query.bindValue(":number", number);
    query.bindValue(":id", id);
    if(!query.exec()){
        qDebug() << "Error insert new image";
        qDebug() << query.lastError();
        return false;
    }
    //добавляем информацию об изображение в таблицу изображений
    for(PatientImageInformation* PII: *data){
        query.prepare("INSERT INTO " TABLE_IMAGES " (id_" TABLE_PATIENTS ",  id_" TABLE_ENLARGMENT ", "
                      "id_" TABLE_COLOR ", Comment)"
                      "VALUES (:id_patient,  :id_enlargment, :id_color, :comment)");
        //связываем данные об изображение с условным обозначением
        query.bindValue(":id_patient", id);
        query.bindValue(":id_color", PII->idColor);
        query.bindValue(":id_enlargment", PII->idEnlargment);
        query.bindValue(":comment", PII->comment);
        if(!query.exec()){
            qDebug() << "Error insert new image";
            qDebug() << query.lastError();
            return false;
        } else{
            qDebug() << "Insert new image compleate";
        }
        //получаем id добавленной информации изображения
        query.exec("SELECT id FROM " TABLE_IMAGES " ORDER BY id DESC LIMIT 1");
        int id_image = 0;
        while (query.next())
            id_image = query.value(0).toInt();
        QDir dir;
        if(!dir.exists("ImageDir"))
            dir.mkdir("ImageDir");
        QPixmap pm = QPixmap(PII->fileName);
        //сохраняем изображение в папку ImageDir имя файла в формате "IDпациента_IDизображения.bmp"
        pm.save("ImageDir/" + QString::number(id) + "_" + QString::number(id_image) + ".bmp");
        //обновляем имя файла
        query.exec("UPDATE " TABLE_IMAGES " SET Image = 'ImageDir/" + QString::number(id) + "_" + QString::number(id_image) + ".bmp' "
                   "WHERE id = " + QString::number(id_image));
        //добавляем симптомы изображения в таблицу "Признаки изображения"
//        for(QPair<int, int> pair : dataSymptoms){
//            query.prepare("INSERT INTO " TABLE_IMAGESYMPTOMS " (id_" TABLE_IMAGES ", "
//                          "id_" TABLE_GROUPSYMPTOMS ", id_" TABLE_SYMPTOMS ") "
//                          "VALUES (:id_image, :id_group, :id_symptom)");
//            query.bindValue(":id_image", number);
//            query.bindValue(":id_group", pair.first);
//            query.bindValue(":id_symptom", pair.second);
//            qDebug() << query.exec();
//        }
    }
    return true;
}

bool Base::updateImage(QList<PatientImageInformation *> *data)
{
    QSqlQuery query;
    query.prepare("UPDATE " TABLE_IMAGES " SET id_" TABLE_ENLARGMENT " = :id_enlargment, id_" TABLE_COLOR " = :id_color, Comment = :comment "
                                          "WHERE id = :id");
    for(PatientImageInformation *PIV : *data){
        query.bindValue(":id", PIV->id);
        query.bindValue(":id_color", PIV->idColor);
        query.bindValue(":id_enlargment", PIV->idEnlargment);
        query.bindValue(":comment", PIV->comment);
        query.exec();
    }
//    query.exec("SELECT id, id_" TABLE_SYMPTOMS " FROM " TABLE_IMAGESYMPTOMS " "
//               "WHERE id_" TABLE_IMAGES " = " + QString::number(id));
//    std::list<QPair<int, int> > dataSymptoms = DS.toStdList();
//    while(query.next()){
//        bool isFind = true;
//        for(QPair<int, int> pair: dataSymptoms)
//            if(pair.second == query.value(1).toInt()){
//                dataSymptoms.remove(pair);
//                isFind = false;
//            }
//        if(isFind)
//            removeImageSymptom(query.value(0).toInt());
//    }
//    for(QPair<int, int> pair: dataSymptoms){
//        query.prepare("INSERT INTO " TABLE_IMAGESYMPTOMS " (id_" TABLE_IMAGES ", "
//                            "id_" TABLE_GROUPSYMPTOMS ", id_" TABLE_SYMPTOMS ") "
//                            "VALUES (:id_image, :id_group, :id_symptom)");
//        query.bindValue(":id_image", id);
//        query.bindValue(":id_group", pair.first);
//        query.bindValue(":id_symptom", pair.second);
//        qDebug() << query.exec();
//    }
    return true;
}

bool Base::removeImage(int id_patient, int id_image)
{
    QSqlQuery query;

    if(!query.exec("DELETE FROM " TABLE_IMAGES " WHERE id = " + QString::number(id_image))){
        qDebug() << "Error remove image";
        qDebug() << query.lastError();
        return false;
    }
    QDir dir;
    dir.remove("ImageDir/" + QString::number(id_patient) + "_" + QString::number(id_image) + ".bmp");
    qDebug() << "Image remove";
    int number = 0;
    if(!query.exec("SELECT NumImages FROM " TABLE_PATIENTS " WHERE id = " + QString::number(id_patient))){
        qDebug() << "Error get num of images";
        qDebug() << query.lastError();
    } else
        while(query.next())
            number = query.value(0).toInt();

    query.prepare("UPDATE " TABLE_PATIENTS " SET NumImages = :number WHERE id = :id");
    query.bindValue(":number", --number);
    query.bindValue(":id", id_patient);
    if(!query.exec()){
        qDebug() << "Error set num of images";
        qDebug() << query.lastError();
        return false;
    }
    if(!query.exec("SELECT id FROM " TABLE_CELLINIMAGE " WHERE id_" TABLE_IMAGES " = " + QString::number(id_image))){
        query.lastError();
        return false;
    } else
        while (query.next())
            this->removeCellMarker(query.value(0).toString());
    return true;
}

QList<PatientImageInformation *> *Base::GetImageInformation(int id)
{
    QSqlQuery query, queryCell;
    QList<PatientImageInformation *> *data = new QList<PatientImageInformation *>();
    if(!query.exec("SELECT " TABLE_IMAGES ".id, " TABLE_ENLARGMENT ".id, " TABLE_COLOR ".id, " TABLE_IMAGES ".Image, "
                   TABLE_ENLARGMENT ".Enlargment, " TABLE_COLOR ".Color, " TABLE_IMAGES ".Comment "
                   "FROM " TABLE_IMAGES " LEFT JOIN " TABLE_ENLARGMENT " ON " TABLE_IMAGES ".id_Enlargment = " TABLE_ENLARGMENT ".id "
                   "LEFT JOIN " TABLE_COLOR " ON " TABLE_IMAGES ".id_Color = " TABLE_COLOR ".id "
                   "WHERE 'Images'.id_" TABLE_PATIENTS " = " + QString::number(id))){
        qDebug() << "Error get information of image";
        qDebug() << query.lastError();
    } else
        while(query.next())
            data->push_back(new PatientImageInformation(query.value(0).toInt(), id, query.value(1).toInt(), query.value(2).toInt(),
                                                        query.value(3).toString(), query.value(4).toString(),
                                                        query.value(5).toString(), query.value(6).toString(),
                                                        this->getCellsOnImage(query.value(0).toString()),
                                                        this->getAllCellCharacteristics(query.value(0).toString())));
    return data;
}

QList<QTreeWidgetItem *> *Base::getCellsOnImage(QString idImage)
{
    QSqlQuery query;

    QList<QTreeWidgetItem *> *dataTree = new QList<QTreeWidgetItem *>();
    if(!query.exec("SELECT " TABLE_TYPECELL ".Cell, " TABLE_CELLINIMAGE ".id FROM 'CellInImage' "
                       "LEFT JOIN " TABLE_TYPECELL " ON " TABLE_CELLINIMAGE ".id_Cell = " TABLE_TYPECELL ".id "
                       "WHERE " TABLE_CELLINIMAGE ".id_Images = " + idImage + " ORDER BY " TABLE_TYPECELL ".Cell ASC"))
        query.lastError();
    else
        while (query.next()) {
            dataTree->push_back(new QTreeWidgetItem(QStringList() << query.value(0).toString() << query.value(1).toString()));
        }
    return dataTree;
}

std::map<int, QList<QTreeWidgetItem *> *> *Base::getAllCellCharacteristics(QString idImage)
{
    QSqlQuery query;
    std::map<int, QList<QTreeWidgetItem *> *> *data = new std::map<int, QList<QTreeWidgetItem *> *>();

    if(!query.exec("SELECT id FROM " TABLE_CELLINIMAGE " WHERE id_" TABLE_IMAGES " = " + idImage)){
        query.lastError();
    } else
        while (query.next()) {
            data->emplace(std::make_pair(query.value(0).toInt(), this->getCharacteristicsTree(query.value(0).toString())));
        }

    return data;
}

int Base::saveCellMarker(const QImage image, int idImage, int idCell)
{
    QSqlQuery query;
//    query.prepare("SELECT id FROM " TABLE_CELLINIMAGE " WHERE id_" TABLE_IMAGES " = :idImage AND id_" TABLE_TYPECELL " = :idCell");
//    query.bindValue(":idImage", idImage);
//    query.bindValue(":idCell", idCell);
//    if(!query.exec()){
//        query.lastError();
//        return false;
//    }
    int idMarker = -1;
//    while (query.next()) {
//        idMarker = query.value(0).toInt();
//    }
    if(idMarker == -1){
        query.prepare("INSERT INTO " TABLE_CELLINIMAGE " (id_" TABLE_IMAGES ", id_" TABLE_TYPECELL ") "
                      "VALUES (:idImage, :idCell)");
        query.bindValue(":idImage", idImage);
        query.bindValue(":idCell", idCell);
        if(!query.exec()){
            query.lastError();
            return false;
        }
        if(!query.exec("SELECT id FROM " TABLE_CELLINIMAGE " ORDER BY id DESC LIMIT 1")){
            query.lastError();
            return false;
        }
        while (query.next()) {
            idMarker = query.value(0).toInt();
        }
    }
    QDir dir;
    if(!dir.exists("CellMarkersDir"))
        dir.mkdir("CellMarkersDir");
    image.save("CellMarkersDir/" + QString::number(idMarker) + ".png");
    return idMarker;
}

int Base::getNameMarkerCell(int idImage, int idCell)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM " TABLE_CELLINIMAGE " WHERE id_" TABLE_IMAGES " = :idImage AND id_" TABLE_TYPECELL " = :idCell");
    query.bindValue(":idImage", idImage);
    query.bindValue(":idCell", idCell);
    if(!query.exec()){
        query.lastError();
        return false;
    }
    int idMarker = -1;
    while (query.next()) {
        idMarker = query.value(0).toInt();
    }
    return idMarker;
}

bool Base::removeCellMarker(QString idMarker)
{
    QSqlQuery query;

    QDir dir;
    dir.remove("CellMarkersDir/" + idMarker + ".png");

    if(!query.exec("DELETE FROM " TABLE_CELLINIMAGE " WHERE id = " + idMarker)){
        qDebug() << query.lastError();
        return false;
    }
    if(this->removeCellCharacteristics(idMarker))
        return false;
    return true;
}

QList<QTreeWidgetItem *> *Base::getCharacteristicsTree()
{
    QSqlQuery queryGroup, querySubgroup, queryCharact;

    QList<QTreeWidgetItem *> *tree = new QList<QTreeWidgetItem *>();

    if(!queryGroup.exec("SELECT * FROM " TABLE_GROUPCHARACTERISTICS )){
        queryGroup.lastError();
    } else {
        while (queryGroup.next()) {
            tree->push_back( new QTreeWidgetItem(QStringList() << queryGroup.value(1).toString()));
            if(!querySubgroup.exec("SELECT id, " TABLE_SUBCHARACTERISTICS " FROM " TABLE_SUBCHARACTERISTICS " "
                                   "WHERE id_" TABLE_GROUPCHARACTERISTICS " = " + queryGroup.value(0).toString())){
                querySubgroup.lastError();
            } else {
                while (querySubgroup.next()) {
                    tree->last()->addChild(new QTreeWidgetItem(tree->last(), QStringList() << querySubgroup.value(1).toString()));
                    if(!queryCharact.exec("SELECT id, " TABLE_CHARACTERISTICS " FROM " TABLE_CHARACTERISTICS " "
                                          "WHERE id_" TABLE_SUBCHARACTERISTICS " = " + querySubgroup.value(0).toString())){
                        queryCharact.lastError();
                    } else {
                        while (queryCharact.next()) {
                            QTreeWidgetItem * item = new QTreeWidgetItem(tree->last()->child(tree->last()->childCount()-1),
                                                QStringList() <<
                                                queryCharact.value(1).toString() <<
                                                queryCharact.value(0).toString());
                            item->setIcon(0, QIcon());
                            tree->last()->child(tree->last()->childCount()-1)->addChild(item);
                        }
                    }
                }
            }
        }
    }
    return tree;
}

QList<QTreeWidgetItem *> *Base::getCharacteristicsTree(QString idCellInImage)
{
    QSqlQuery query;

    QList<QTreeWidgetItem *> *tree = new QList<QTreeWidgetItem *>();

    if(!query.exec("SELECT " TABLE_CELLCHARACTERISTICS ".id_" TABLE_CHARACTERISTICS ", " TABLE_CHARACTERISTICS "." TABLE_CHARACTERISTICS ", "
                   TABLE_SUBCHARACTERISTICS "." TABLE_SUBCHARACTERISTICS ", " TABLE_GROUPCHARACTERISTICS "." TABLE_GROUPCHARACTERISTICS " "
                   "FROM " TABLE_CELLCHARACTERISTICS " LEFT JOIN " TABLE_CHARACTERISTICS " ON " TABLE_CELLCHARACTERISTICS ".id_" TABLE_CHARACTERISTICS
                   " = " TABLE_CHARACTERISTICS ".id LEFT JOIN " TABLE_SUBCHARACTERISTICS " ON " TABLE_CHARACTERISTICS ".id_" TABLE_SUBCHARACTERISTICS
                   " = " TABLE_SUBCHARACTERISTICS ".id LEFT JOIN " TABLE_GROUPCHARACTERISTICS " ON " TABLE_SUBCHARACTERISTICS ".id_" TABLE_GROUPCHARACTERISTICS
                   " = " TABLE_GROUPCHARACTERISTICS ".id WHERE " TABLE_CELLCHARACTERISTICS ".id_" TABLE_CELLINIMAGE " = " + idCellInImage)){
        query.lastError();
    } else {
        while (query.next()) {
            if(tree->empty()){
                tree->push_back(new QTreeWidgetItem(QStringList() << query.value(3).toString()));
                tree->last()->addChild(new QTreeWidgetItem(tree->last(), QStringList() << query.value(2).toString()));
                tree->last()->child(tree->last()->childCount()-1)->addChild(
                            new QTreeWidgetItem(tree->last()->child(tree->last()->childCount()-1),
                                                QStringList() << query.value(1).toString() << query.value(0).toString()));
            } else {
                if(tree->last()->data(0, 0).toString() == query.value(3).toString()){
                    if(tree->last()->child(tree->last()->childCount()-1)->data(0, 0) == query.value(2).toString()){
                        tree->last()->child(tree->last()->childCount()-1)->addChild(
                                    new QTreeWidgetItem(tree->last()->child(tree->last()->childCount()-1),
                                                        QStringList() << query.value(1).toString() << query.value(0).toString()));
                    } else {
                        tree->last()->addChild(new QTreeWidgetItem(tree->last(),
                                                                   QStringList() << query.value(2).toString()));
                        tree->last()->child(tree->last()->childCount()-1)->addChild(
                                    new QTreeWidgetItem(tree->last()->child(tree->last()->childCount()-1),
                                                        QStringList() << query.value(1).toString() << query.value(0).toString()));
                    }
                } else {
                    tree->push_back(new QTreeWidgetItem(QStringList() << query.value(3).toString()));
                    tree->last()->addChild(new QTreeWidgetItem(tree->last(), QStringList() << query.value(2).toString()));
                    tree->last()->child(tree->last()->childCount()-1)->addChild(
                                new QTreeWidgetItem(tree->last()->child(tree->last()->childCount()-1),
                                                    QStringList() << query.value(1).toString() << query.value(0).toString()));
                }
            }
        }
    }
    return tree;
}

bool Base::saveCellCharacteristics(QString id, QList<QString> &data)
{
    QSqlQuery query;
    for(auto idCharact : data)
        if(!query.exec("INSERT INTO " TABLE_CELLCHARACTERISTICS " (id_" TABLE_CELLINIMAGE ", id_" TABLE_CHARACTERISTICS ") "
                       "VALUES ('" + id + "', '" + idCharact + "')")){
            query.lastError();
            return false;
        }
    return true;
}

bool Base::removeCellCharacteristics(QString idCellInImage)
{
    QSqlQuery query;
    if(!query.exec("DELETE FROM " TABLE_CELLCHARACTERISTICS " WHERE id_" TABLE_CELLINIMAGE " = " +idCellInImage)){
        query.lastError();
        return false;
    }
    return true;
}

bool Base::insertInTable(const QString &table, const QString &val)
{
    QSqlQuery query;

    if(!query.exec("INSERT INTO " + table + " (" + table + ") VALUES ('" + val + "')")){
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

bool Base::insertCharacteristics(const QString &table, const QString &idField, const QString &id, const QString &val)
{
    QSqlQuery query;

    if(!query.exec("INSERT INTO " + table + " (" + idField + ", " + table+ ") VALUES ('" + id + "', '" + val + "')")){
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

bool Base::removeFromTable(const QString &table, int id)
{
    if(table != TABLE_CHARACTERISTICS && table != TABLE_SUBCHARACTERISTICS &&
            table != TABLE_GROUPCHARACTERISTICS)
    if( id == 1)
        return false;
    QSqlQuery query;

    if(!query.exec("DELETE FROM " + table + " WHERE id = " + QString::number(id))){
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

bool Base::openDataBase()
{
    /* База данных открывается по заданному пути
     * и имени базы данных, если она существует
     * */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DATABASE_HOSTNAME);
    db.setDatabaseName(".\\" DATABASE_NAME);
    if(db.open()){
        return true;
    } else {
        return false;
    }
}

bool Base::restoreDataBase()
{
    if(this->openDataBase()){
           if(!this->createTable()){
               return false;
           } else {
               return true;
           }
       } else {
           qDebug() << "Не удалось восстановить базу данных";
           return false;
       }
}

void Base::closeDataBase()
{
    db.close();
}

bool Base::createTable()
{
    QSqlQuery query;
    if(!query.exec( "CREATE TABLE " TABLE_PATIENTS " ("                     //таблица пациента
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "NumberCard VARCHAR(64) NOT NULL,"
                    "Name VARCHAR(128) NOT NULL,"
                    "id_" TABLE_DIAGNOSIS " INTEGER NOT NULL,"
                    "DateMaterial DATE NOT NULL,"
                    "NumImages INTEGER NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_PATIENTS;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_DIAGNOSIS " ("                    //таблица диагноза
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    TABLE_DIAGNOSIS " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_DIAGNOSIS;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_IMAGES " ("                       //таблица изображений
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "id_" TABLE_PATIENTS " INTEGER NOT NULL,"
                    "id_" TABLE_ENLARGMENT " INTEGER NOT NULL,"
                    "id_" TABLE_COLOR " INTEGER NOT NULL,"
                    "Image VARCHAR(255),"
                    "Comment VARCHAR(255)"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_IMAGES;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_COLOR " ("                        //таблица цвета изображений
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    TABLE_COLOR " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_COLOR;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_ENLARGMENT " ("                   //таблица увеличения изображения
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    TABLE_ENLARGMENT " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_ENLARGMENT;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_CELLINIMAGE " ("                  //таблица клетки на изображение
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "id_" TABLE_IMAGES " INTEGER NOT NULL,"
                    "id_" TABLE_TYPECELL " INTEGER NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_CELLINIMAGE;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_TYPECELL " ("                     //таблица тип клетки
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    TABLE_TYPECELL " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_TYPECELL;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_GROUPCHARACTERISTICS " ("         //таблица Группа характеристик
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    TABLE_GROUPCHARACTERISTICS " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_TYPECELL;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_SUBCHARACTERISTICS " ("           //таблица Подгруппа характеристик
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "id_" TABLE_GROUPCHARACTERISTICS " INTEGER NOT NULL,"
                    TABLE_SUBCHARACTERISTICS " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_TYPECELL;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_CHARACTERISTICS " ("              //таблица Характеристики
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "id_" TABLE_SUBCHARACTERISTICS " INTEGER NOT NULL,"
                    TABLE_CHARACTERISTICS " VARCHAR(255) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_TYPECELL;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_CELLCHARACTERISTICS " ("          //таблица Характеристики
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "id_" TABLE_CELLINIMAGE " INTEGER NOT NULL,"
                    "id_" TABLE_CHARACTERISTICS " INTEGER NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_TYPECELL;
        qDebug() << query.lastError().text();
        return false;
    }
    this->initialTable();
    return true;
}

void Base::initialTable()
{
    QFile file;
    file.setFileName(TABLE_DIAGNOSIS ".txt");                       //выбираем настроечный файл "Диагноз"
    if(!file.exists()){                                             //проверка существует ли уже файл
        if(!file.open(QIODevice::Append | QIODevice::Text))         //открываем его
            return;
        QTextStream out(&file);                                     //настраиваем поток
        out << "" << "\n";                                          //сохраняем пустую ячейку
        this->insertInTable(TABLE_DIAGNOSIS , "");                  //вставляем в таблицу
    } else {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))      //открываем его
            return;

        QTextStream in(&file);                                      //настраиваем поток
        while (!in.atEnd())                                         //пока файл не закончился
            this->insertInTable(TABLE_DIAGNOSIS , in.readLine());   //читаем из файла строку и сразу вставляем в таблицу
    }
    file.close();                                                   //закрываем файл

    file.setFileName(TABLE_COLOR ".txt");                           //выбираем настроечный файл "Окраска"
    if(!file.exists()){                                             //проверка существует ли уже файл
        if(!file.open(QIODevice::Append | QIODevice::Text))         //открываем его
            return;
        QTextStream out(&file);                                     //настраиваем поток
        out << "" << "\n";                                          //сохраняем пустую ячейку
        this->insertInTable(TABLE_COLOR , "");                      //вставляем в таблицу
    } else {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))      //открываем его
            return;

        QTextStream in(&file);                                      //настраиваем поток
        while (!in.atEnd())                                         //пока файл не закончился
            this->insertInTable(TABLE_COLOR , in.readLine());       //читаем из файла строку и сразу вставляем в таблицу
    }
    file.close();                                                   //закрываем файл

    file.setFileName(TABLE_ENLARGMENT ".txt");                      //выбираем настроечный файл "Увеличение"
    if(!file.exists()){                                             //проверка существует ли уже файл
        if(!file.open(QIODevice::Append | QIODevice::Text))         //открываем его
            return;
        QTextStream out(&file);                                     //настраиваем поток
        out << "" << "\n";                                          //сохраняем пустую ячейку
        this->insertInTable(TABLE_ENLARGMENT , "");                 //вставляем в таблицу
    } else {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))      //открываем его
            return;

        QTextStream in(&file);                                      //настраиваем поток
        while (!in.atEnd())                                         //пока файл не закончился
            this->insertInTable(TABLE_ENLARGMENT , in.readLine());  //читаем из файла строку и сразу вставляем в таблицу
    }
    file.close();                                                   //закрываем файл

    file.setFileName(TABLE_TYPECELL ".txt");                        //выбираем настроечный файл "Тип клетки"
    if(!file.exists()){                                             //проверка существует ли уже файл
        if(!file.open(QIODevice::Append | QIODevice::Text))         //открываем его
            return;
        QTextStream out(&file);                                     //настраиваем поток
        out << "" << "\n";                                          //сохраняем пустую ячейку
        this->insertInTable(TABLE_TYPECELL , "");                   //вставляем в таблицу
    } else {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))      //открываем его
            return;

        QTextStream in(&file);                                      //настраиваем поток
        while (!in.atEnd())                                         //пока файл не закончился
            this->insertInTable(TABLE_TYPECELL , in.readLine());    //читаем из файла строку и сразу вставляем в таблицу
    }
    file.close();                                                   //закрываем файл

    QSqlQuery query;

    file.setFileName(TABLE_CHARACTERISTICS ".txt");                 //выбираем настроечный файл "Характеристики"
    if(file.exists()){                                              //проверка существует ли уже файл
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))         //открываем его
            return;
        QTextStream in(&file);                                      //настраиваем поток
        int idGroup = 0, idSubgroup = 0;
        while (!in.atEnd()) {                                       //пока файл не закончился
            QString line = in.readLine();
            if(line.contains("    ", Qt::CaseSensitive)){
                this->insertCharacteristics(TABLE_CHARACTERISTICS, "id_" TABLE_SUBCHARACTERISTICS,
                                            QString::number(idSubgroup), line.remove(0, 4));
            } else {
                if(line.contains("  ", Qt::CaseSensitive)){
                    this->insertCharacteristics(TABLE_SUBCHARACTERISTICS, "id_" TABLE_GROUPCHARACTERISTICS,
                                                QString::number(idGroup), line.remove(0, 2));
                    if(!query.exec("SELECT MAX(id) FROM " TABLE_SUBCHARACTERISTICS ))
                        query.lastError();
                    else
                        while (query.next())
                            idSubgroup = query.value(0).toInt();
                } else {
                    this->insertInTable(TABLE_GROUPCHARACTERISTICS, line);
                    if(!query.exec("SELECT MAX(id) FROM " TABLE_GROUPCHARACTERISTICS ))
                        query.lastError();
                    else
                        while (query.next())
                            idGroup = query.value(0).toInt();
                }
            }
        }
    }
}
