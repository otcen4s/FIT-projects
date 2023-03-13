/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda je vlastna graficka scena pre vykreslenie mapoveho podkladu a zvysnych objektov, ktore k mape patria
 * @file mapscene.cpp
 *
 *
 */

#include "mapscene.h"

/**
 * @brief Konstruktor pre mapovu scenu
 * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
 */
MapScene::MapScene(QObject *parent) : QGraphicsScene(parent)
{

}

/**
 * @brief Konstruktor pre mapovu scenu
 * @param mapScene Graficka scena mapy
 * @param transportScene Graficka scena pre zobrazovanie grafikonu
 * @param appTime Vnutorny cas aplikacie
 */
MapScene::MapScene(QGraphicsScene *mapScene, TransportScene *transportScene, QTime* appTime) : QGraphicsScene()
{
    this->transportScene = transportScene;
    this->appTime = appTime;
    createMap(mapScene);
}

/**
 * @brief Vytvori mapovy podklad spolu so vsetkymi potrebnymi objektami
 * @param mapScene Graficka scena pre mapu
 */
void MapScene::createMap(QGraphicsScene *mapScene)
{
    Files mapFiles;

    auto map = mapFiles.readCSVmap(); // nacitanie ciar

    mStreetsAttributes = map;  // ulozenie do clenskej premennej atributy ciar (suradnice osi)

    auto streetNames = mapFiles.readTXTstreetNames();  // nacitanie nazvov ulic

    auto streetStops = mapFiles.readCSVstops();  // nacitanie zastavok
    QMap<int, QList<QString>>::iterator itStops = streetStops.begin();  // vytvorenie iteratora pre pristup k listu

    auto lines = mapFiles.readCSVlines();  // nacitanie dopravnych liniek
    QMap<int, QList<QString>>::iterator itLines = lines.begin();  // vytvorenie iteratora pre pristup k listu
    auto linesCount = lines.size();

    if(map.empty() || streetNames.empty() || streetStops.empty() || lines.empty()) exit(1);  // otestovanie ci boli ciary spravne nacitane

    auto i = 0;
    auto iterateAgain = 0;
    auto ii = 0;

    for(auto const& it : map){   // iterovanie cez vsetky polozky v mape pre vytvorenie mapoveho podkladu
         QList<int> values = it;
         int x1, x2, y1, y2; x1 = x2 = y1 = y2 = 0;

         // podmienka len aby nedoslo k siahnutiu mimo pamati
        if(values.size() == 4){ x1 = values.at(0); y1 = values.at(1); x2 = values.at(2); y2 = values.at(3);}

        if(streetStops.size() <= i){
            itStops.value().clear();
            itStops.value().insert(0, ""); itStops.value().insert(1, ""); itStops.value().insert(2, "");
        }

        QString street = "";
        if(streetNames.size() > i)  street = streetNames.at(i);

        Street* str = new Street(Q_NULLPTR, x1, y1, x2, y2, mapScene, street, this->transportScene);  // volanie konstruktora pre kazdu liniu

        this->streets.push_back(str);  // ukladanie objektov kazdej ulice

        if(linesCount > ii){
            Vehicle *veh = new Vehicle(Q_NULLPTR, mapScene, itLines.value(), str, this->transportScene, this->appTime);

            this->vehicles.push_back(veh);
            veh->vehicleColor = this->setVehicleColor(itLines.value().at(0));
            veh->pathColor = this->setVehicleColor(itLines.value().at(0));
            veh->trafficLevel = &str->trafficLevel;
        }

        qreal stopPos = 0.1;
        for(int i = 0; i < itStops.value().size(); i++){
            Stop *stop = new Stop(Q_NULLPTR, str->line(), mapScene, stopPos);
            this->stops.push_back(stop);
            stopPos += 0.4;
        }


        if(streetStops.size() - 1 > i) itStops++;

        if(iterateAgain < lines.size() - 1){ itLines++; iterateAgain++; }
        else { itLines = lines.begin(); iterateAgain = 0; }

        i++;
        ii++;
    }


    // v tejto casti sa ku kazdemu vozidlu danej linky (napr. linka 10, 42 , ...)
    // ulozia casy premavania (teda napriklad cas prichodu k prvej zastavke, druhej a tretej)
    // vnoreny cyklus je urceny pre zistenie, o ktoru linku sa jedna
    auto linesSchedule = mapFiles.readCSVlinesSchedule();
    QMap<QString, QVector<QList<QString>>>::iterator itLinesSchedules = linesSchedule.begin();

    for(; itLinesSchedules != linesSchedule.end(); itLinesSchedules++){
        for(auto const& vehicle : this->vehicles){
            if(vehicle->lines.at(0) == itLinesSchedules.key()) {
                vehicle->lineSchedules = itLinesSchedules.value();
            }
        }
    }

    for(auto const& vehicle : this->vehicles){
        vehicle->totalLinesCount = vehicle->lineSchedules.size();
    }


}

/**
 * @brief Nastavi farbu linky
 * @param Cislo linky vo forme retazca
 * @return Farbu v hexadeximalnej hodnote
 */
QString MapScene::setVehicleColor(QString line)
{
    if(line == "10")      return "#ccccff";
    else if(line == "24") return "#ccff66";
    else if(line == "42") return "#00ffcc";
    else if(line == "30") return "#3399ff";
    else if(line == "12") return "#FFCCDD";
    else if(line == "15") return "#ffff66";
    else if(line == "1")  return "#660066";
    else if(line == "7")  return "#80dfff";
    else if(line == "21") return "#cc3300";
    else if(line == "5")  return "#0000ff";
    else if(line == "44") return "#ffffcc";
    else if(line == "3")  return "#00b3b3";
    else if(line == "70") return "#003366";
    else if(line == "112")return "#996633";
    else if(line == "11") return "#ff9900";
    else if(line == "14") return "#b3ecff";
    else if(line == "99") return "#66ff33";
    else if(line == "6")  return "#660033";
    else if(line == "8")  return "#ff66ff";
    else if(line == "93") return "#a19fff";
    else if(line == "61") return "#ccffff";
    else return "#ffffff";
}

/**
 * @brief Destruktor
 */
MapScene::~MapScene()
{

}

