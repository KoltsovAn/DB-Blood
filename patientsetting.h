#ifndef PATIENTSETTING_H
#define PATIENTSETTING_H

#include <QDialog>
#include <QtSql/QSqlQueryModel>
#include <QDebug>

#include <map>
#include <utility>
#include <iterator>

#include "structsfordb.h"

namespace Ui {
class PatientSetting;
}

class PatientSetting : public QDialog
{
    Q_OBJECT

public:
    explicit PatientSetting(std::map<QString, int> *dataDiagnosis, QWidget *parent = nullptr);
    explicit PatientSetting(std::map<QString, int> *dataDiagnosis, PatientInformation *dataPatient, QWidget *parent = nullptr);
    ~PatientSetting();

private:
    Ui::PatientSetting *ui;

    PatientInformation *data = new PatientInformation();
    std::map<QString, int> *Diagnosis = nullptr;

signals:
    void PatientData(const PatientInformation *data);
    void closeWindow();
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // PATIENTSETTING_H
