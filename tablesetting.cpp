#include "tablesetting.h"
#include "ui_tablesetting.h"

#include <QDebug>

TableSetting::TableSetting(std::map<QString, int> *_diagnos, std::map<QString, int> *_color, std::map<QString, int> *_enlargment,
                           std::map<QString, int> *_typeCell, std::map<QString, int> *_groupCharacteristics,
                           std::map<int, std::map<QString, int> *> *_subgroupCharacteristics,
                           std::map<int, std::map<QString, int> *> *_characteristics,
                           QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableSetting),
    diagnos(_diagnos),
    color(_color),
    enlargment(_enlargment),
    typeCell(_typeCell),
    groupCharacteristics(_groupCharacteristics),
    subgroupCharacteristics(_subgroupCharacteristics),
    characteristics(_characteristics)
{
    ui->setupUi(this);

    for(const auto &p : *diagnos)
        ui->comboBox_2->addItem(p.first);
    ui->label_3->hide();
    ui->label_4->hide();
    ui->comboBox_3->hide();
    ui->comboBox_4->hide();

    this->setWindowModality(Qt::ApplicationModal);
}

TableSetting::~TableSetting()
{
    delete diagnos;
    delete color;
    delete enlargment;
    delete typeCell;
    delete groupCharacteristics;
    delete subgroupCharacteristics;
    delete characteristics;
    delete ui;
}

void TableSetting::updateTableInformation(std::map<QString, int> *newItem)
{
    switch (table) {
    case Tables::Diagnosis :
        delete diagnos;
        diagnos = newItem;
        ui->comboBox_2->clear();
        for(const auto &p : *diagnos)
            ui->comboBox_2->addItem(p.first);
        break;
    case Tables::Color :
        delete color;
        color = newItem;
        ui->comboBox_2->clear();
        for(const auto &p : *color)
            ui->comboBox_2->addItem(p.first);
        break;
    case Tables::Enlargment :
        delete enlargment;
        enlargment = newItem;
        ui->comboBox_2->clear();
        for(const auto &p : *enlargment)
            ui->comboBox_2->addItem(p.first);
        break;
    case Tables::TypeCell :
        delete typeCell;
        typeCell = newItem;
        ui->comboBox_2->clear();
        for(const auto &p : *typeCell)
            ui->comboBox_2->addItem(p.first);
        break;
    case Tables::GroupCharacteristics :
        delete groupCharacteristics;
        groupCharacteristics = newItem;
        ui->comboBox_2->clear();
        for(const auto &p : *groupCharacteristics)
            ui->comboBox_2->addItem(p.first);
        break;
    default:
        break;
    }
}

void TableSetting::updateCharacteristics(std::map<int, std::map<QString, int> *> *newItem)
{
    switch (table) {
    case Tables::SubroupCharacteristics :
        for(auto &p : *subgroupCharacteristics)
            delete p.second;
        delete subgroupCharacteristics;
        subgroupCharacteristics = newItem;
        ui->comboBox_3->clear();
        if(!subgroupCharacteristics->empty())
            for(const auto &p : *subgroupCharacteristics->at(groupCharacteristics->at(ui->comboBox_2->currentText())))
                ui->comboBox_3->addItem(p.first);
        break;
    case Tables::Characteristics :
        for(auto &p : *characteristics)
            delete p.second;
        delete characteristics;
        characteristics = newItem;
        ui->comboBox_4->clear();
        if(!characteristics->empty())
            for(const auto &p : *characteristics->at(subgroupCharacteristics->at(groupCharacteristics->at(ui->comboBox_2->currentText()))->at(ui->comboBox_3->currentText())))
                ui->comboBox_4->addItem(p.first);
        break;
    default:
        break;
    }
    QFile file("Characteristics.txt");                         //выбираем настроечный файл
    file.open(QIODevice::WriteOnly | QIODevice::Text);             //открываем его
    QTextStream out(&file);
    for(const auto &p : *groupCharacteristics){
        out << p.first << "\n";
        const auto itrSub = subgroupCharacteristics->find(p.second);
        if(itrSub != subgroupCharacteristics->cend()){
            for(const auto &pSub : *itrSub->second){
                out << "  " << pSub.first << "\n";
                const auto itrCharact = characteristics->find(pSub.second);
                if(itrCharact != characteristics->cend()){
                    for(const auto &pCharacter : *itrCharact->second){
                        out << "    " << pCharacter.first << "\n";
                    }
                }
            }
        }
    }
    file.close();
}

void TableSetting::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closeWindow();
}

void TableSetting::on_comboBox_currentIndexChanged(int index)
{
    ui->label_3->hide();
    ui->label_4->hide();
    ui->comboBox_3->hide();
    ui->comboBox_4->hide();

    switch (index) {
    case 0:
        ui->comboBox_2->clear();
        tableName = "Diagnosis";
        table = Tables::Diagnosis;
        for(const auto &p : *diagnos)
            ui->comboBox_2->addItem(p.first);
        break;
    case 1:
        ui->comboBox_2->clear();
        tableName = "Color";
        table = Tables::Color;
        for(const auto &p : *color)
            ui->comboBox_2->addItem(p.first);
        break;
    case 2:
        ui->comboBox_2->clear();
        tableName = "Enlargment";
        table = Tables::Enlargment;
        for(const auto &p : *enlargment)
            ui->comboBox_2->addItem(p.first);
        break;
    case 3:
        ui->comboBox_2->clear();
        tableName = "Cell";
        table = Tables::TypeCell;
        ui->comboBox_2->clear();
        for(const auto &p : *typeCell)
            ui->comboBox_2->addItem(p.first);
        break;
    case 4:
        ui->comboBox_2->clear();
        tableName = "GroupCharacteristics";
        table = Tables::GroupCharacteristics;
        ui->comboBox_2->clear();
        for(const auto &p : *groupCharacteristics)
            ui->comboBox_2->addItem(p.first);
        break;
    case 5:
        ui->comboBox_2->clear();
        ui->comboBox_3->clear();
        tableName = "SubgroupCharacteristics";
        table = Tables::SubroupCharacteristics;
        ui->comboBox_2->clear();
        ui->comboBox_3->clear();
        ui->label_3->show();
        ui->comboBox_3->show();
        for(const auto &p : *groupCharacteristics)
            ui->comboBox_2->addItem(p.first);
        break;
    case 6:
        ui->comboBox_2->clear();
        ui->comboBox_3->clear();
        ui->comboBox_4->clear();
        tableName = "Characteristics";
        table = Tables::Characteristics;
        ui->comboBox_2->clear();
        ui->comboBox_3->clear();
        ui->comboBox_4->clear();
        ui->label_3->show();
        ui->label_4->show();
        ui->comboBox_3->show();
        ui->comboBox_4->show();
        for(const auto &p : *groupCharacteristics)
            ui->comboBox_2->addItem(p.first);
        break;
    default:
        break;
    }
}

void TableSetting::on_pushButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Добавление"), tr("Введите новое поле:"), QLineEdit::Normal, QString(), &ok);
    if(text == "")
        return;
    if (ok && !text.isEmpty()){
        if(table == Tables::SubroupCharacteristics){
            if(ui->comboBox_2->currentText() == "")
                return;
            QString idField = "id_GroupCharacteristics";
            emit addCharacteristics(tableName, idField,
                                    QString::number(groupCharacteristics->at(ui->comboBox_2->currentText())), text);
        } else {
            if(table == Tables::Characteristics){
                if(ui->comboBox_2->currentText() == "")
                    return;
                QString idField = "id_SubgroupCharacteristics";
                const auto itrSub = subgroupCharacteristics->find(groupCharacteristics->at(ui->comboBox_2->currentText()));
                if(itrSub != subgroupCharacteristics->cend()){
                    const auto itrSubSecond = itrSub->second->find(ui->comboBox_3->currentText());
                    if(itrSubSecond != itrSub->second->cend()){
                        emit addCharacteristics(tableName, idField,
                                                QString::number(itrSubSecond->second), text);
                    }
                }
            } else
                if(emit addInTable(tableName, text)){
                    QFile file(tableName + ".txt");                         //выбираем настроечный файл
                    file.open(QIODevice::Append | QIODevice::Text);         //открываем его
                    QTextStream out(&file);
                    out << text << "\n";
                    file.close();
                }
        }
    }
}

void TableSetting::on_pushButton_2_clicked()
{
    switch (table) {
    case Tables::Diagnosis :
        if(emit remove(tableName, diagnos->at(ui->comboBox_2->currentText()))){
            diagnos->erase(ui->comboBox_2->currentText());
            ui->comboBox_2->clear();
            for(const auto &p : *diagnos)
                ui->comboBox_2->addItem(p.first);
        }
        break;
    case Tables::Color :
        if(emit remove(tableName, color->at(ui->comboBox_2->currentText()))){
            color->erase(ui->comboBox_2->currentText());
            ui->comboBox_2->clear();
            for(const auto &p : *color)
                ui->comboBox_2->addItem(p.first);
        }
        break;
    case Tables::Enlargment :
        if(emit remove(tableName, enlargment->at(ui->comboBox_2->currentText()))){
            enlargment->erase(ui->comboBox_2->currentText());
            ui->comboBox_2->clear();
            for(const auto &p : *enlargment)
                ui->comboBox_2->addItem(p.first);
        }
        break;
    case Tables::TypeCell :
        if(emit remove(tableName, typeCell->at(ui->comboBox_2->currentText()))){
            typeCell->erase(ui->comboBox_2->currentText());
            ui->comboBox_2->clear();
            for(const auto &p : *typeCell)
                ui->comboBox_2->addItem(p.first);
        }
        break;
    case Tables::GroupCharacteristics :
        if(emit remove(tableName, groupCharacteristics->at(ui->comboBox_2->currentText()))){
            groupCharacteristics->erase(ui->comboBox_2->currentText());
            ui->comboBox_2->clear();
            for(const auto &p : *groupCharacteristics)
                ui->comboBox_2->addItem(p.first);
        }
        break;
    case Tables::SubroupCharacteristics : {
        const auto itrGroup = groupCharacteristics->find(ui->comboBox_2->currentText());
        if(itrGroup != groupCharacteristics->cend()){
            const auto itrSub = subgroupCharacteristics->find(itrGroup->second);
            if(itrSub != subgroupCharacteristics->cend()){
                auto itrSubId = itrSub->second->find(ui->comboBox_3->currentText());
                if(itrSubId != itrSub->second->end()){
                    if(emit remove(tableName, itrSubId->second)){
                        itrSub->second->erase(ui->comboBox_3->currentText());
                        ui->comboBox_3->clear();
                        for(const auto &p : *itrSub->second)
                            ui->comboBox_3->addItem(p.first);
                    }
                }
            }
        }
        break;
    }
    case Tables::Characteristics : {
        const auto itrGroup = groupCharacteristics->find(ui->comboBox_2->currentText());
        if(itrGroup != groupCharacteristics->cend()){
            const auto itrSub = subgroupCharacteristics->find(itrGroup->second);
            if(itrSub != subgroupCharacteristics->cend()){
                const auto itrSubId = itrSub->second->find(ui->comboBox_3->currentText());
                if(itrSubId != itrSub->second->cend()){
                    const auto itrCharacter = characteristics->find(itrSubId->second);
                    if(itrCharacter != characteristics->cend()){
                        auto itrCharactId = itrCharacter->second->find(ui->comboBox_4->currentText());
                        if(itrCharactId != itrCharacter->second->end()){
                            if(emit remove(tableName, itrCharactId->second)){
                                itrCharacter->second->erase(ui->comboBox_4->currentText());
                                ui->comboBox_4->clear();
                                for(const auto &p : *itrCharacter->second)
                                    ui->comboBox_4->addItem(p.first);
                            }
                        }
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

void TableSetting::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    ui->comboBox_3->clear();
    const auto itrGroup = groupCharacteristics->find(arg1);
    if(itrGroup != groupCharacteristics->cend()){
        const auto itrSub = subgroupCharacteristics->find(itrGroup->second);
        if(itrSub != subgroupCharacteristics->cend()){
            for(const auto &p : *itrSub->second)
                ui->comboBox_3->addItem(p.first);
        }
    }
}

void TableSetting::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    ui->comboBox_4->clear();
    const auto itrGroup = groupCharacteristics->find(ui->comboBox_2->currentText());
    if(itrGroup != groupCharacteristics->cend()){
        const auto itrSub = subgroupCharacteristics->find(itrGroup->second);
        if(itrSub != subgroupCharacteristics->cend()){
            const auto itrCharacter = characteristics->find(itrSub->second->find(arg1)->second);
            if(itrCharacter != characteristics->end()){
                for(const auto &p : *itrCharacter->second)
                    ui->comboBox_4->addItem(p.first);
            }
        }
    }
}
