#include "patientsetting.h"
#include "ui_patientsetting.h"

PatientSetting::PatientSetting(std::map<QString, int> *dataDiagnosis, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientSetting),
    Diagnosis(dataDiagnosis)
{
    ui->setupUi(this);

    //ui->comboBox->setModel(QueryModel);
    for(const auto &p : *Diagnosis)
        ui->comboBox->addItem(p.first);

    this->setWindowModality(Qt::ApplicationModal);
}

PatientSetting::PatientSetting(std::map<QString, int> *dataDiagnosis, PatientInformation *dataPatient, QWidget *parent):
    QDialog(parent),
    ui(new Ui::PatientSetting),
    data(dataPatient),
    Diagnosis(dataDiagnosis)
{
    ui->setupUi(this);

    //ui->comboBox->setModel(QueryModel);
    for(const auto &p : *Diagnosis)
        ui->comboBox->addItem(p.first);

    ui->lineEdit->setText(dataPatient->NumberCard);
    ui->lineEdit_4->setText(dataPatient->Name);
    ui->comboBox->setCurrentText(dataPatient->Diagnos);
    ui->dateEdit_2->setDate(dataPatient->DateMaterial);

    this->setWindowModality(Qt::ApplicationModal);
}

PatientSetting::~PatientSetting()
{
    delete data;
    delete Diagnosis;
    delete ui;
}

void PatientSetting::on_pushButton_2_clicked()
{
    emit closeWindow();
}

void PatientSetting::on_pushButton_clicked()
{
    data->NumberCard = ui->lineEdit->text();
    data->Name = ui->lineEdit_4->text();
    data->id_diagnos = Diagnosis->at(ui->comboBox->currentText());
    data->DateMaterial = ui->dateEdit_2->date();
    emit PatientData(data);
}

void PatientSetting::closeEvent(QCloseEvent *event)
{
    emit closeWindow();
    QDialog::closeEvent(event);
}
