#ifndef MARKERWINDOW_H
#define MARKERWINDOW_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class MarkerWindow;
}

class MarkerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MarkerWindow(std::map<QString, int> *dataCell, QWidget *parent = nullptr);
    ~MarkerWindow();

    int getTypeFigure();

private:
    Ui::MarkerWindow *ui;

    std::map<QString, int> *typeCell = nullptr;

   void initialColor();

private slots:
   void on_comboBox_currentIndexChanged(int index);
   void on_comboBox_2_currentIndexChanged(const QString &arg1);
   void on_comboBox_3_currentIndexChanged(int index);
   void on_pushButton_5_clicked();

   void on_pushButton_clicked();

   void on_pushButton_2_clicked();

   void on_pushButton_3_clicked();

   void on_pushButton_4_clicked();

   void on_comboBox_4_currentTextChanged(const QString &arg1);

   void update();

signals:
   void closeWindow();
   void saveMarker(int idCell);
   void colorChange(int i);
   void depthChange(const QString &i);
   void figureChange(int i);
   void typeCellChange(int i);
   void removeLastItemButton();
   void removeAllButton();
   void restoreButton();
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MARKERWINDOW_H
