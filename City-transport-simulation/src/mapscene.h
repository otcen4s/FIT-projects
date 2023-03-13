/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda je vlastna graficka scena pre vykreslenie mapoveho podkladu a zvysnych objektov, ktore k mape patria
 * @file mapscene.h
 *
 *
 */

#ifndef MYMAPSCENE_H
#define MYMAPSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

#include "files.h"
#include "street.h"
#include "stop.h"
#include "vehicle.h"
#include "transportscene.h"

class MapScene : public QGraphicsScene
{
    Q_OBJECT
public:

    /**
     * @brief Konstruktor pre mapovu scenu
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     */
    explicit MapScene(QObject *parent = nullptr);

    /**
     * @brief Konstruktor pre mapovu scenu
     * @param mapScene Graficka scena mapy
     * @param transportScene Graficka scena pre zobrazovanie grafikonu
     * @param appTime Vnutorny cas aplikacie
     */
    MapScene(QGraphicsScene *mapScene = nullptr, TransportScene *transportScene = nullptr, QTime* appTime = nullptr);

    /**
     * @brief Destruktor
     */
    ~MapScene();

    /**
     * @brief Vytvori mapovy podklad spolu so vsetkymi potrebnymi objektami
     * @param mapScene Graficka scena pre mapu
     */
    void createMap(QGraphicsScene* mapScene);

    std::vector <Street*> streets;
    std::vector <Vehicle*> vehicles;
    std::vector <Stop*> stops;
    QMap<int, QList<int>> mStreetsAttributes;
    QMap<int, QList<QString>> mStreetStops;

    TransportScene *transportScene;

    QTime* appTime;

private:
    /**
     * @brief Nastavi farbu linky
     * @param Cislo linky vo forme retazca
     * @return Farbu v hexadeximalnej hodnote
     */
    QString setVehicleColor(QString line);
};

#endif // MYMAPSCENE_H
