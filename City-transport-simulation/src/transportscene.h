/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda je vlastna graficka scena na ktorej sa vykresluje grafikon linky
 * @file transportscene.h
 *
 *
 */

#ifndef MYTRANSPORTSCENE_H
#define MYTRANSPORTSCENE_H

#include <QObject>
#include <QGraphicsScene>

#include "graphicon.h"


class TransportScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor pre vytvorenie sceny
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     */
    explicit TransportScene(QObject *parent = nullptr);

    /**
     * @brief Konstruktor sceny pre uz predtym inicializovanu scenu
     * @param transportScene Ukazatel na scenu
     */
    TransportScene(QGraphicsScene *transportScene = nullptr);

    /**
     * @brief Destruktor
     */
    ~TransportScene();

    Graphicon *graphicon;
    QGraphicsScene *transportScene;

    /**
     * @brief Vytvori grafikon pre linku a ulozi ho do transportnej sceny
     * @param lines Zoznam informacii o linke
     * @param lineSchedules Zoznam casov danej linky
     * @param endPoint Obsahuje aktualnu poziciu vozidla
     * @param endReached True, ak bola dosiahnuta konecna zastavka, inak False
     * @param vehicleColor Farba vozidla
     * @param delay Obsahuje spozdenie spoja
     * @param middleReached True, ak bola dosiahnuta prostredna zastavka, inak False
     * @param startReached True, ak bola dosiahnuta zaciatocna zastavka, inak False
     */
    void createGrafficon(QList<QString> lines,  QList<QString> lineSchedules, qreal endPoint, bool endReached, QString vehicleColor, int delay, bool middleReached, bool startReached);

    /**
     * @brief Premaze scenu, koli updatu linky
     */
    void clearScene();

};

#endif // MYTRANSPORTSCENE_H
