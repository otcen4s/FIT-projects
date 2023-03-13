/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda vykresluje grafikon linky do grafickej sceny TransportScene
 * @file graphicon.h
 *
 *
 */

#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QPushButton>
#include <QStyle>
#include <QMenu>

class Graphicon : public QGraphicsLineItem
{
public:
    /**
     * @brief Konstruktor grafikonu pre konkretnu linku
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     * @param transportScene Ukazatel na graficku scenu
     * @param position Pozicia vozidla na trase
     * @param lines Zoznam informacii danej linky
     * @param lineSchedules Zoznam casovych odchodov
     * @param vehicleColor Farba vozidla
     * @param endReached True, ak bola dosiahnuta konecna zastavka, inak False
     * @param delay Pocet minut meskania
     * @param middleReached True, ak bola dosiahnuta prostredna zastavka, inak False
     * @param startReached True, ak bola dosiahnuta zaciatocna zastavka, inak False
     */
    Graphicon(QGraphicsItem *parent = Q_NULLPTR, QGraphicsScene *transportScene = nullptr,  qreal position = 0.0,
            QList<QString> lines = {}, QList<QString> lineSchedules = {}, QString vehicleColor = "000000",  bool endReached = false,
            uint delay = 0, bool middleReached = false, bool startReached = false);
    /**
     * @brief Destruktor
     */
    ~Graphicon();

    QGraphicsScene *transportScene;


protected:
    /**
     * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
     * @param painter Vykresluje zadane parametre do sceny
     * @param option Nepouzita premenna, potrebna pri prepisani funkcie
     * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

private:
     qreal position;
     QList<QString> lines;
     QList<QString> lineSchedules;
     QString vehicleColor;
     bool endReached;
     bool startReached;
     bool middleReached;
     int delay;

     /**
      * @brief V pripade, ze dana linka ide z pociatocneho smeru, tak sa vola funkcia na spravne vykreslenie grafikonu
      * @param painter Vykresluje zadane parametre do sceny
      */
     void startDirection(QPainter *painter);

     /**
      * @brief V pripade, ze sa dana linka vracia z druheho smeru, tak sa vola funkcia na spravne vykreslenie grafikonu
      * @param painter Vykresluje zadane parametre do sceny
      */
     void endDirection(QPainter *painter);
};

#endif // TRAFFIC_H
