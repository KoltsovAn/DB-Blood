#ifndef TABLESETTING_H
#define TABLESETTING_H

#include <QDialog>
#include <QInputDialog>
#include <QFile>
#include <map>

namespace Ui {
class TableSetting;
}

class TableSetting : public QDialog
{
    Q_OBJECT

public:
    explicit TableSetting(std::map<QString, int> *_diagnos, std::map<QString, int> *_color, std::map<QString, int> *_enlargment,
                          std::map<QString, int> *_typeCell, std::map<QString, int> *_groupCharacteristics,
                          std::map<int, std::map<QString, int> *> *_subgroupCharacteristics,
                          std::map<int, std::map<QString, int> *> *_characteristics,
                          QWidget *parent = nullptr);
    ~TableSetting();

    void updateTableInformation(std::map<QString, int> *newItem);
    void updateCharacteristics(std::map<int, std::map<QString, int> *> *newItem);

    enum Tables{ Diagnosis,
               Color,
               Enlargment,
               TypeCell,
               GroupCharacteristics,
               SubroupCharacteristics,
               Characteristics};

signals:
    void closeWindow();

    bool addInTable(const QString &table, const QString &val);
    bool addCharacteristics(const QString &table, const QString &idField, const QString &id, const QString &val);
    bool remove(const QString &table, int id);

private:
    Ui::TableSetting *ui;

    std::map<QString, int> *diagnos = nullptr;
    std::map<QString, int> *color = nullptr;
    std::map<QString, int> *enlargment = nullptr;
    std::map<QString, int> *typeCell = nullptr;
    std::map<QString, int> *groupCharacteristics = nullptr;
    std::map<int, std::map<QString, int> *> *subgroupCharacteristics = nullptr;
    std::map<int, std::map<QString, int> *> *characteristics = nullptr;

    QString tableName = "Diagnosis";
    int table = Tables::Diagnosis;
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_comboBox_2_currentIndexChanged(const QString &arg1);
    void on_comboBox_3_currentIndexChanged(const QString &arg1);
};

#endif // TABLESETTING_H
