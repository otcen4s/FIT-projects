/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda je vlastna graficka scena na ktorej sa vykresluje grafikon linky
 * @file transportscene.cpp
 *
 *
 */

#include "transportscene.h"


/**
 * @brief Konstruktor pre vytvorenie sceny
 * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
 */
TransportScene::TransportScene(QObject *parent) : QGraphicsScene(parent)
{

}

/**
 * @brief Konstruktor sceny pre uz predtym inicializovanu scenu
 * @param transportScene Ukazatel na scenu
 */
TransportScene::TransportScene(QGraphicsScene *transportScene) : QGraphicsScene()
{
    this->transportScene = transportScene;
    this->graphicon = nullptr;
}

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
void TransportScene::createGrafficon(QList<QString> lines,  QList<QString> lineSchedules,
                                       qreal endPoint, bool endReached, QString vehicleColor, int delay, bool middleReached, bool startReached)
{
    this->graphicon = new Graphicon(Q_NULLPTR, this->transportScene, endPoint, lines, lineSchedules, vehicleColor, endReached, delay, middleReached, startReached);

}

/**
 * @brief Premaze scenu, koli updatu linky
 */
void TransportScene::clearScene(){
    if(this->graphicon == nullptr) return;

    delete this->graphicon;
}


/**
 * @brief Destruktor
 */
TransportScene::~TransportScene()
{
}
