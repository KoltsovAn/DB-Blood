#ifndef CHARACTERISTICSSETTING_H
#define CHARACTERISTICSSETTING_H

#include <QDialog>

#include <QList>
#include <QTreeWidgetItem>

namespace Ui {
class CharacteristicsSetting;
}

class CharacteristicsSetting : public QDialog
{
    Q_OBJECT

public:
    explicit CharacteristicsSetting(QList<QTreeWidgetItem *> *_characteristics, QWidget *parent = nullptr);
    ~CharacteristicsSetting();

signals:
    void closeWindow();
    void setCharacteristics(QList<QString> &data);

private:
    Ui::CharacteristicsSetting *ui;

    QList<QTreeWidgetItem *> *characteristics = new QList<QTreeWidgetItem *>();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_2_clicked();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_pushButton_clicked();
};

#endif // CHARACTERISTICSSETTING_H
