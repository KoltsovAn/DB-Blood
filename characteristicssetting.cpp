#include "characteristicssetting.h"
#include "ui_characteristicssetting.h"

#include <QDebug>

CharacteristicsSetting::CharacteristicsSetting(QList<QTreeWidgetItem *> *_characteristics, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CharacteristicsSetting),
    characteristics(_characteristics)
{
    ui->setupUi(this);

    ui->treeWidget->addTopLevelItems(*characteristics);
    ui->treeWidget->expandAll();

    this->setWindowFlags(Qt::Tool);
}

CharacteristicsSetting::~CharacteristicsSetting()
{
    delete characteristics;
    delete ui;
}

void CharacteristicsSetting::closeEvent(QCloseEvent *event)
{
    emit closeWindow();
    QDialog::closeEvent(event);
}

void CharacteristicsSetting::on_pushButton_2_clicked()
{
    emit closeWindow();
}

void CharacteristicsSetting::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(!item->childCount()){
        if(item->icon(column).cacheKey() == QIcon().cacheKey())
            item->setIcon(column, QIcon(QPixmap("check.bmp")));
        else
            item->setIcon(column, QIcon());
    }
}

void CharacteristicsSetting::on_pushButton_clicked()
{
    QList<QString> data;
    for(auto *group : *characteristics){
        for(int subIndex = 0; subIndex < group->childCount(); subIndex++){
            for(int charactIndex = 0; charactIndex < group->child(subIndex)->childCount(); charactIndex++){
                if(group->child(subIndex)->child(charactIndex)->icon(0).cacheKey() != QIcon().cacheKey())
                    data << group->child(subIndex)->child(charactIndex)->data(1, 0).toString();
            }
        }
    }
    emit setCharacteristics(data);
}
