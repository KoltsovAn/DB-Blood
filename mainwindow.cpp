#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = new Base();
    db->connectToDataBase();

    sceneImage = new PaintScene();
    ui->graphicsView->setScene(sceneImage);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu); //настройки дерева для отклика на првую кнопку мыши
    this->setupModel();
    this->createUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModel()
{
    /* Производим инициализацию модели представления данных
         * с установкой имени таблицы в базе данных, по которому
         * будет производится обращение в таблице
         * */
    model = new QSqlRelationalTableModel(this);
    model->setTable(TABLE_PATIENTS);

    model->setRelation(3, QSqlRelation(TABLE_DIAGNOSIS, "id", "Diagnosis"));

    model->setHeaderData(1, Qt::Horizontal, "№ карты");
    model->setHeaderData(2, Qt::Horizontal, "ФИО");
    model->setHeaderData(3, Qt::Horizontal, "Диагноз");
    model->setHeaderData(4, Qt::Horizontal, "Дата взятия материала");
    model->setHeaderData(5, Qt::Horizontal, "Количество изображений");

    // Устанавливаем сортировку по возрастанию данных по нулевой колонке
    model->setSort(0, Qt::AscendingOrder);

    model->select(); // Делаем выборку данных из таблицы
}

void MainWindow::createUI()
{
    ui->tableView->setModel(model);     // Устанавливаем модель на TableView
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    // Разрешаем выделение строк
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(PS != nullptr){
            PS->closeWindow();
            delete PS;
    }
    if(IS != nullptr){
        IS->closeWindow();
        delete IS;
    }
    if(Marker != nullptr){
        Marker->closeWindow();
        delete Marker;
    }
    if(TS != nullptr){
        TS->closeWindow();
        delete TS;
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::on_pushButton_clicked()
{
    if(PS != nullptr)
        return;
    PS = new PatientSetting(db->getQueryMap(TABLE_DIAGNOSIS), this);
    connect(PS, SIGNAL(PatientData(const PatientInformation *)), this, SLOT(setNewPatientData(const PatientInformation *)));
    connect(PS, SIGNAL(closeWindow()), this, SLOT(closePatientSetting()));
    PS->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    if(PS != nullptr)
        return;
    if(id_Patient == -1)
        return;
    PS = new PatientSetting(db->getQueryMap(TABLE_DIAGNOSIS), db->GetPatientInformation(id_Patient), this);
    connect(PS, SIGNAL(PatientData(const PatientInformation *)), this, SLOT(UpdatePatient(const PatientInformation *)));
    connect(PS, SIGNAL(closeWindow()), this, SLOT(closePatientSetting()));
    PS->show();
}

void MainWindow::on_pushButton_3_clicked()
{
    switch (QMessageBox::question(this, "", "Вы действительно хотите удалить пациента?", "Да", "Нет")) {
    case 0:
        // Удалить пациента
        db->removePatient(id_Patient);
        id_Patient = -1;
        model->select();
        break;
    default:
        // ничего не делать
        break;
    }
}

void MainWindow::setNewPatientData(const PatientInformation *data)
{
    if(!db->addNewPatient(data)){
        PS->show();
        return;
    }
    model->select();
    this->closePatientSetting();
}

void MainWindow::UpdatePatient(const PatientInformation *data)
{
//    qDebug() << data.id << ";" << data.NumberCard << ";" << data.Name << ";" << data.id_diagnos << ";" << data.DateMaterial << "" << data.NumImages;
    if(!db->updatePatient(data)){
        PS->show();
        return;
    }
    model->select();
    this->closePatientSetting();
}

void MainWindow::closePatientSetting()
{
    delete PS;
    PS = nullptr;
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    id_Patient = model->data(model->index(index.row(), 0)).toInt();
    delete patientImage;
    patientImage = nullptr;
    id_Image = 0;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        model->select();
        break;
    case 1:
        if(patientImage == nullptr){
            patientImage = db->GetImageInformation(id_Patient);
            if(patientImage->empty()){
                id_Image = -1;
                sceneImage->clear();
                pmInScene = nullptr;
                ui->tableWidget->clear();
                ui->tableWidget->setRowCount(0);
                ui->tableWidget->setColumnCount(0);
                ui->treeWidget->clear();
                ui->treeWidget_2->clear();
                ui->label_5->clear();
                ui->label_6->clear();
                ui->label_8->clear();
                break;
            }
            QTableWidgetItem *TWI;
            ui->tableWidget->setRowCount(1);
            ui->tableWidget->setColumnCount(patientImage->size());
            int id = 0;
            for(PatientImageInformation *PIV : *patientImage){
                TWI = new QTableWidgetItem();
                TWI->setData(Qt::DecorationRole, QPixmap(PIV->fileName).scaled(200, 200));
                ui->tableWidget->setItem(0, id++,TWI);
                qApp->processEvents();
            }
            ui->tableWidget->setCurrentCell(0, id_Image);
            this->on_tableWidget_cellClicked(0, id_Image);
        } else{
            sceneImage->clear();
            ui->tableWidget->clear();
            pmInScene = nullptr;
            ui->tableWidget->setRowCount(0);
            ui->tableWidget->setColumnCount(0);
            ui->treeWidget->clear();
            ui->treeWidget_2->clear();
            ui->label_5->clear();
            ui->label_6->clear();
            ui->label_8->clear();
        }
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(id_Patient == -1)
        return;
    if(IS != nullptr || PS != nullptr)
        return;
    IS = new ImageSetting(db->getQueryMap(TABLE_ENLARGMENT), db->getQueryMap(TABLE_COLOR), this);
    connect(IS, SIGNAL(ImageData(QList<PatientImageInformation *> *)),
                       this, SLOT(setNewImage(QList<PatientImageInformation *> *)));
    connect(IS, SIGNAL(closeWindow()), this, SLOT(closeImageSetting()));
    IS->show();
}

void MainWindow::on_pushButton_5_clicked()
{
    if(id_Image == -1 || id_Patient == -1)
        return;
    IS = new ImageSetting(db->getQueryMap(TABLE_ENLARGMENT), db->getQueryMap(TABLE_COLOR), db->GetImageInformation(id_Patient), this);
    connect(IS, SIGNAL(ImageData(QList<PatientImageInformation *> *)),
            this, SLOT(updateImage(QList<PatientImageInformation *> *)));
    connect(IS, SIGNAL(closeWindow()), this, SLOT(closeImageSetting()));
    IS->show();
}

void MainWindow::on_pushButton_6_clicked()
{
    if(id_Image == -1 || id_Patient == -1)
        return;
    switch (QMessageBox::question(this, "", "Вы действительно хотите удалить изображение?", "Да", "Нет")) {
    case 0:
        // Удалить изображение
        db->removeImage(id_Patient, patientImage->at(id_Image)->id);
        delete patientImage;
        patientImage = nullptr;
        id_Image = 0;
        this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
        break;
    default:
        // ничего не делать
        break;
    }
}

void MainWindow::setNewImage(QList<PatientImageInformation *> *data)
{
    if(!db->addNewImage(id_Patient, data)){
        IS->show();
        return;
    }
    this->closeImageSetting();
}

void MainWindow::updateImage(QList<PatientImageInformation *> *data)
{
    if(!db->updateImage(data)){
        IS->show();
        return;
    }
    this->closeImageSetting();
}

void MainWindow::closeImageSetting()
{
    id_Image = 0;
    delete patientImage;
    patientImage = nullptr;
    this->on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
    delete IS;
    IS = nullptr;
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    Q_UNUSED(row);
    id_Image = column;
    sceneImage->clear();
    pmInScene = new QGraphicsPixmapItem(patientImage->at(id_Image)->fileName);
    sceneImage->addItem(pmInScene);
    while (ui->treeWidget->topLevelItemCount())
        ui->treeWidget->takeTopLevelItem(0);
    ui->treeWidget->insertTopLevelItems(0, *patientImage->at(column)->itemsCell);
    ui->label_5->setText(patientImage->at(column)->enlargment);
    ui->label_6->setText(patientImage->at(column)->color);
    ui->label_8->setText(patientImage->at(column)->comment);
}

void MainWindow::on_pushButton_7_clicked()
{
    if(Marker != nullptr || CW != nullptr || id_Image == -1 || id_Patient == -1)
        return;
    Marker = new MarkerWindow(db->getQueryMap(TABLE_TYPECELL), this);
    connect(Marker, SIGNAL(closeWindow()), this, SLOT(closeMarkerWindow()));
    connect(Marker, SIGNAL(saveMarker(int)), this, SLOT(saveMarker(int)));
    connect(Marker, SIGNAL(colorChange(int)), this, SLOT(setColorMarker(int)));
    connect(Marker, SIGNAL(depthChange(const QString &)), this, SLOT(setDepthMarker(const QString &)));
    connect(Marker, SIGNAL(figureChange(int)), this, SLOT(setFigureMarker(int)));
    connect(Marker, SIGNAL(typeCellChange(int)), this, SLOT(typeCellChange(int)));
    connect(Marker, SIGNAL(removeLastItemButton()), this, SLOT(removeLastItem()));
    connect(Marker, SIGNAL(removeAllButton()), this, SLOT(removeAll()));
    connect(Marker, SIGNAL(restoreButton()), this, SLOT(restore()));
    Marker->show();
    sceneImage->setDeffoltSetting();
}

void MainWindow::closeMarkerWindow()
{
    delete Marker;
    Marker = nullptr;
    sceneImage->setTypeFigure(PaintScene::none);
}

void MainWindow::saveMarker(int idCell)
{
    sceneImage->removeItem(pmInScene);
    QImage image(sceneImage->width(), sceneImage->height(), QImage::Format_ARGB32_Premultiplied);
    image.fill(0x00FFFFFF);
    QPainter painter(&image);
    sceneImage->render(&painter);
    /*int idMarker = */db->saveCellMarker(image, patientImage->at(id_Image)->id, idCell);
    for(QGraphicsItem *item : sceneImage->items())
        sceneImage->removeItem(item);
    sceneImage->addItem(pmInScene);
    ui->treeWidget->clear();
    delete patientImage->at(id_Image)->itemsCell;
    patientImage->at(id_Image)->itemsCell = db->getCellsOnImage(QString::number(patientImage->at(id_Image)->id));
    ui->treeWidget->insertTopLevelItems(0, *patientImage->at(id_Image)->itemsCell);
    while (ui->treeWidget_2->topLevelItemCount())
        ui->treeWidget_2->takeTopLevelItem(0);
    delete patientImage->at(id_Image)->cellCharacteristics;
    patientImage->at(id_Image)->cellCharacteristics =
            db->getAllCellCharacteristics(QString::number(patientImage->at(id_Image)->id));
//    sceneImage->addPixmap(QPixmap("CellMarkersDir/" + QString::number(idMarker) + ".png"));
}

void MainWindow::setColorMarker(int i)
{
    switch(i){
    case 0:
        sceneImage->setColorFigure(Qt::black);
        break;
    case 1:
        sceneImage->setColorFigure(Qt::white);
        break;
    case 2:
        sceneImage->setColorFigure(Qt::red);
        break;
    case 3:
        sceneImage->setColorFigure(Qt::green);
        break;
    case 4:
        sceneImage->setColorFigure(Qt::blue);
        break;
    case 5:
        sceneImage->setColorFigure(Qt::cyan);
        break;
    case 6:
        sceneImage->setColorFigure(Qt::magenta);
        break;
    case 7:
        sceneImage->setColorFigure(Qt::yellow);
        break;
    case 8:
        sceneImage->setColorFigure(Qt::gray);
        break;
    default:
        break;
    }
}

void MainWindow::setDepthMarker(const QString &arg)
{
    sceneImage->setDepthFigure(arg.toInt());
}

void MainWindow::setFigureMarker(int i)
{
    switch (i) {
    case 0:
        sceneImage->setTypeFigure(PaintScene::SquareType);
        break;
    case 1:
        sceneImage->setTypeFigure(PaintScene::TriangleType);
        break;
    case 2:
        sceneImage->setTypeFigure(PaintScene::RombType);
        break;
    default:
        break;
    }
}

void MainWindow::typeCellChange(int i)
{
    Q_UNUSED(i);
    for(QGraphicsItem *item: sceneImage->items())
        sceneImage->removeItem(item);
    sceneImage->addItem(pmInScene);
    qDebug() << i;
    if(i == 1){
        sceneImage->setTypeFigure(PaintScene::none);
        return;
    }
    switch (Marker->getTypeFigure()) {
    case 0:
        sceneImage->setTypeFigure(PaintScene::SquareType);
        break;
    case 1:
        sceneImage->setTypeFigure(PaintScene::TriangleType);
        break;
    case 2:
        sceneImage->setTypeFigure(PaintScene::RombType);
        break;
    default:
        sceneImage->setTypeFigure(PaintScene::none);
        break;
    }

//    int idMarker = db->getNameMarkerCell(patientImage->at(id_Image)->id, i);
//    if(idMarker != -1)
//        sceneImage->addPixmap("CellMarkersDir/" + QString::number(idMarker) + ".png");
}

void MainWindow::removeLastItem()
{
    sceneImage->removeLastFigure();
}

void MainWindow::removeAll()
{
    sceneImage->removeALlFigure();
}

void MainWindow::restore()
{
    sceneImage->restoreFigure();
}

void MainWindow::closeTableSetting()
{
    delete TS;
    TS = nullptr;
}

bool MainWindow::addInTable(const QString &table, const QString &val)
{
    bool result = db->insertInTable(table, val);
    if(result)
        TS->updateTableInformation(db->getQueryMap(table));
    return result;
}

bool MainWindow::addCharacteristics(const QString &table, const QString &idField, const QString &id, const QString &val)
{
    bool result = db->insertCharacteristics(table, idField, id, val);
    if(result)
        TS->updateCharacteristics(db->getCharacteristics(table));
    return result;
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    for(QGraphicsItem *item : sceneImage->items())
        sceneImage->removeItem(item);
    sceneImage->addItem(pmInScene);
    sceneImage->addPixmap("CellMarkersDir/" + item->data(column + 1, 0).toString() + ".png");
    while (ui->treeWidget_2->topLevelItemCount())
        ui->treeWidget_2->takeTopLevelItem(0);
    ui->treeWidget_2->insertTopLevelItems(0, *patientImage->at(id_Image)->cellCharacteristics->at(item->data(column+1, 0).toInt()));
    ui->treeWidget_2->expandAll();
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    this->on_treeWidget_itemClicked(ui->treeWidget->currentItem(), 0);
    switch (QMessageBox::question(this, "", "Вы действительно хотите удалить эту клетку?", "Да", "Нет")) {
    case 0:
        // Удалить клетку
        db->removeCellMarker(ui->treeWidget->currentItem()->data(1, 0).toString());
        ui->treeWidget->clear();
        delete patientImage->at(id_Image)->itemsCell;
        patientImage->at(id_Image)->itemsCell = db->getCellsOnImage(QString::number(patientImage->at(id_Image)->id));
        ui->treeWidget->insertTopLevelItems(0, *patientImage->at(id_Image)->itemsCell);
        for(QGraphicsItem *item : sceneImage->items())
            sceneImage->removeItem(item);
        sceneImage->addItem(pmInScene);
        break;
    default:
        // ничего не делать
        break;
    }

}

void MainWindow::on_action_triggered()
{
    if(PS != nullptr || IS != nullptr || TS != nullptr)
        return;
    TS = new TableSetting(db->getQueryMap(TABLE_DIAGNOSIS), db->getQueryMap(TABLE_COLOR),
                          db->getQueryMap(TABLE_ENLARGMENT), db->getQueryMap(TABLE_TYPECELL),
                          db->getQueryMap(TABLE_GROUPCHARACTERISTICS),
                          db->getCharacteristics(TABLE_SUBCHARACTERISTICS),
                          db->getCharacteristics(TABLE_CHARACTERISTICS), this);
    connect(TS, SIGNAL(closeWindow()), this, SLOT(closeTableSetting()));
    connect(TS, SIGNAL(addInTable(const QString &, const QString &)),
            this, SLOT(addInTable(const QString &, const QString &)));
    connect(TS, SIGNAL(addCharacteristics(const QString, const QString, const QString, const QString)),
            this, SLOT(addCharacteristics(const QString, const QString, const QString, const QString)));
    connect(TS, SIGNAL(remove(const QString &, int)),
            db, SLOT(removeFromTable(const QString &, int)));
    TS->show();
}

void MainWindow::on_pushButton_8_clicked()
{
    if(CW != nullptr || Marker != nullptr || ui->treeWidget->currentIndex().row() == -1)
        return;
    ui->treeWidget->setEnabled(false);
    CW = new CharacteristicsSetting(db->getCharacteristicsTree(), this);
    connect(CW, SIGNAL(closeWindow()), this, SLOT(closeCharacteristicsWindow()));
    connect(CW, SIGNAL(setCharacteristics(QList<QString> &)),
            this, SLOT(setCharacteristics(QList<QString> &)));
    CW->show();
}

void MainWindow::closeCharacteristicsWindow()
{
    delete CW;
    CW = nullptr;
    ui->treeWidget->setEnabled(true);
}

void MainWindow::setCharacteristics(QList<QString> &characteristics)
{
    if(ui->treeWidget->currentIndex().row() == -1){
        QMessageBox::information(this, QString(), "Все пизда рулю ты все сломал!\n"
                                                  "Закрывай все к чертям собачим и не подходи к компьютеру больше никогда!");
        return;
    }
    if(db->saveCellCharacteristics(ui->treeWidget->currentItem()->data(1, 0).toString(), characteristics)){
        delete patientImage->at(id_Image)->cellCharacteristics->at(ui->treeWidget->currentItem()->data(1, 0).toInt());
        patientImage->at(id_Image)->cellCharacteristics->at(ui->treeWidget->currentItem()->data(1, 0).toInt()) =
                db->getCharacteristicsTree(ui->treeWidget->currentItem()->data(1, 0).toString());
        while (ui->treeWidget_2->topLevelItemCount())
            ui->treeWidget_2->takeTopLevelItem(0);
        ui->treeWidget_2->insertTopLevelItems(0, *patientImage->at(id_Image)->cellCharacteristics->at(
                                                  ui->treeWidget->currentItem()->data(1, 0).toInt()));
        this->closeCharacteristicsWindow();
    }
}
