#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <list>
#include "figure.h"

class PaintScene : public QGraphicsScene
{
    Q_OBJECT
    // Свойство текущего типа используемой фигуры
    Q_PROPERTY(int typeFigure
               READ typeFigure WRITE setTypeFigure
               NOTIFY typeFigureChanged)

public:
    explicit PaintScene(QObject *parent = nullptr);
    ~PaintScene();

    int typeFigure() const;                 // Возвращение текщего типа
    void setTypeFigure(const int type);     // Установка текущего типа

    void setDepthFigure(const int newDepth);
    void setColorFigure(const QColor newColor);
    void removeLastFigure();
    void removeALlFigure();
    void restoreFigure();

    void setDeffoltSetting();

    // Перечисление типов используемых фигур
    enum FigureTypes {
        SquareType,
        RombType,
        TriangleType,
        none
    };

signals:
    void typeFigureChanged();               // Сигнал об изменении типа текущей фигуры

private:
    /* Объект для временного хранения рисуемой фигуры
     * Является объектом базового класса для всех трёх типов фигур в примере
     * */
    Figure *tempFigure = nullptr;
    QColor color = Qt::black;
    int depth = 3;
    int m_typeFigure = PaintScene::none;   // текущий тип фигуры
    std::list< Figure*> FigureList;

private:
    // Для рисования используем события мыши
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

};

#endif // PAINTSCENE_H
