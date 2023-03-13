/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda obsluhuje simulaciu vozidla na trati
 * @file vehicle.cpp
 *
 *
 */

#include "vehicle.h"

/**
 * @brief Konstruktor inicializuje vsetky potrebne premenne pre vozidlo, vlozi ho do sceny aj s potrebnymi zvysnymi parametrami
 * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
 * @param mapScene Ukazatel na graficku scenu pre mapu
 * @param lines Zoznam, ktory uchovava data o linke v forme stringu
 * @param itemStreet Ukazatel na ulice, na ktorej sa vozdilo nachadza
 * @param transportScene Ukazatel na scenu, urcenu pre grafikon linky
 * @param appTime Vnutorny cas aplikacie
 */

Vehicle::Vehicle(QGraphicsItem *parent, QGraphicsScene *mapScene, QList<QString> lines, QGraphicsLineItem *itemStreet,
                     TransportScene *transportScene, QTime* appTime) : QObject(), QGraphicsEllipseItem(parent)
{
    /* inicializacia premennych potrebnych pre casovu synchronizaciu */
    this->appTime = appTime;
    this->moveFromStop = false;
    this->speed = 0;
    this->noDelaySpeed = 0;
    this->endPoint = 0.1;
    this->startPoint = 0.1;
    this->endReached = false;
    this->startReached = true;
    this->middleReached = false;
    this->moveEnabled = false;
    this->delay = 0;
    this->checkEnd = true;
    this->checkMiddle = true;
    this->moveEnabled = false;
    this->delayEnabled = false;
    this->middleTimeReached = false;
    this->startTimeReached = true;
    this->endTimeReached = false;
    this->checkDelay = true;
    this->reset = false;
    this->imaginaryTimeRatio = 12;
    this->noDelayPos = 0.1;
    this->totalLinesCount = 0;
    this->actualLinesCount = 0;
    this->setSpeedOne = false;
    this->setSpeedTwo = false;
    this->delayEndPoint = 0.1;
    this->h0 = 0;
    this->m0 = 0;
    this->h1 = 0;
    this->m1 = 0;
    this->h2 = 0;
    this->m2 = 0;
    this->newDay = false;
    newDayScheduleReached = false;

    // inicializacia scen
    this->transportScene = transportScene;
    this->mapScene = mapScene;

    // inicializacia farieb
    this->vehicleColor = "FFFFFF";
    this->pathColor = "FFFFFF";

    // inicializacia ulice na ktorej dany spoj bude premavat
    this->itemStreet = itemStreet;

    // nastavenie pociatocnej pozicie vozidla v scene a jeho ohranicujuceho stvorca
    this->setPos(mapToScene(this->itemStreet->line().pointAt(this->startPoint).x(), this->itemStreet->line().pointAt(this->startPoint).y()));
    this->setRect(-15, -15, 30, 30);

    // pridanie objektu do sceny
    mapScene->addItem(this);

    // nastaveni kurzora pre objekt vozidla
    this->setCursor(Qt::PointingHandCursor);

    // inicializacia cisla linky a jej zastavok
    this->lines = lines;


    //inicializacia ciary, ktora bude zvyraznovat trasu vozidla
    auto tmpX1 = this->itemStreet->line().pointAt(0.1).x();
    auto tmpY1 = this->itemStreet->line().pointAt(0.1).y();
    auto tmpX2 = this->itemStreet->line().pointAt(0.9).x();
    auto tmpY2 = this->itemStreet->line().pointAt(0.9).y();
    this->tmpItemStreet = new QGraphicsLineItem(tmpX1, tmpY1, tmpX2, tmpY2, itemStreet);
    this->tmpItemStreet->setPen(QPen({QBrush(QColor("#737373"))}, 3));

    //premenna na urcovanie ci je cesta vyfarbena
    this->colorDisplayed = false;
}

/**
 * @brief Metoda vytvori ohranicujuci stvorec o danej velkosti
 * @return QRectF Ohranicujuci stvorec
 */
QRectF Vehicle::boundingRect() const
{
    return QRectF(-15,-15,30,30);
}

/**
 * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
 * @param painter Vykresluje zadane parametre do sceny
 * @param option Nepouzita premenna, potrebna pri prepisani funkcie
 * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
 */
void Vehicle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = boundingRect();

    painter->setPen(QPen({QBrush(QColor("#000000"))}, 5));
    painter->setBrush(QBrush(QColor(vehicleColor)));
    painter->drawEllipse(rect);
}

/**
 * @brief Metoda, ktora sa vola pri kazdom kliknuti na objekt elipsy
 * @param event Obsahuje poziciu kliknutia
 */
void Vehicle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->colorRoad(event);
    QGraphicsEllipseItem::mousePressEvent(event);
}

/**
 * @brief Vyfarbi trasu vozdila a zavola metody triedy transportScene pre vypis informacii o vozidle
 * @param event Obsahuje poziciu kliknutia
 */
void Vehicle::colorRoad(QGraphicsSceneMouseEvent *event)
{
    for (auto *item : this->mapScene->items(event->scenePos()))
    {
        if(auto *vehicle = dynamic_cast<QGraphicsEllipseItem *>(item); vehicle != nullptr){
            if(!vehicle) continue;

            if(vehicle == this){
                if(!this->colorDisplayed){

                    this->tmpItemStreet->setPen(QPen({QBrush(QColor(this->pathColor))}, 3));
                    this->colorDisplayed = true;

                    this->transportScene->clearScene();
                    this->transportScene->createGrafficon(this->lines, this->lineSchedules[this->actualLinesCount],this->endPoint, this->endReached,
                                                          this->vehicleColor, this->delay, this->middleReached, this->startReached);
                }

                else {
                    this->tmpItemStreet->setPen(QPen({QBrush(QColor("#737373"))}, 3));
                    this->colorDisplayed = false;
                }
                this->itemStreet->update();
            }
        }
    }
}


/**
 * @brief Metoda je volana pri kazdom vyvolanom casovom signale
 * @param phase Urcuje, ci je faza casoveho signalu
 */
void Vehicle::advance(int phase)
{
    if(!phase) return;  // faza nie je

    if(*this->trafficLevel > 1 && this->checkDelay) this->delayEnabled = true;  // traffic level bol zmeneny na ceste

    if(this->actualLinesCount == this->totalLinesCount) {  // pocitadlo pre reset grafikonu liniek
        this->actualLinesCount = 0;
        this->reset = true;
    }

    // casy odchodov zo zastavok
    this->h0 = this->lineSchedules[this->actualLinesCount].at(0).split(":")[0].toInt();
    this->m0 = this->lineSchedules[this->actualLinesCount].at(0).split(":")[1].toInt();
    this->h1 = this->lineSchedules[this->actualLinesCount].at(1).split(":")[0].toInt();
    this->m1 = this->lineSchedules[this->actualLinesCount].at(1).split(":")[1].toInt();
    this->h2 = this->lineSchedules[this->actualLinesCount].at(2).split(":")[0].toInt();
    this->m2 = this->lineSchedules[this->actualLinesCount].at(2).split(":")[1].toInt();


    // casove rozdiely medzi zastavkami v milisekundach
    if(this->h1 < this->h0) this->startMiddleTimeDifference = (((h1 - h0) + 24) * 3600000) + ((m1 - m0) * 60000);
    else this->startMiddleTimeDifference = ((h1 - h0) * 3600000) + ((m1 - m0) * 60000);

    if(this->h2 < this->h1) this->middleEndTimeDifference = (((h2 - h1) + 24) * 3600000) + ((m2 - m1) * 60000);
    else this->middleEndTimeDifference = ((h2 - h1) * 3600000) + ((m2 - m1) * 60000);


    // vozidlo moze vyrazit z prvej zastavky
    if((this->appTime->hour() == h0 && this->appTime->minute() == m0) && !this->moveEnabled){
        this->realTime = this->startMiddleTimeDifference / this->imaginaryTimeRatio;
        this->speed = ((0.4 / realTime) * 50) / *this->trafficLevel;
        this->noDelaySpeed = (0.4 / realTime) * 50;


        this->moveEnabled = true;
        this->setSpeedTwo = false;
        this->setSpeedOne = true;
        this->moveVehicle();
    }

    // vozidlo moze vyrazit z druhej zastavky
    else if((this->appTime->hour() == h1 && this->appTime->minute() == m1) && !this->moveEnabled) {
        this->realTime = this->middleEndTimeDifference / this->imaginaryTimeRatio;
        this->speed = ((0.4 / realTime) * 50)  / *this->trafficLevel;
        this->noDelaySpeed = (0.4 / realTime) * 50;

        this->moveEnabled = true;
        this->setSpeedTwo = true;
        this->setSpeedOne = false;
        this->moveVehicle();
    }

    // vozidlo sa moze pohnut v case o danu rychlost
    else if(this->moveEnabled){

       if(this->setSpeedOne){
            this->realTime = this->startMiddleTimeDifference / this->imaginaryTimeRatio;
            this->speed = ((0.4 / realTime) * 50)  / *this->trafficLevel;
            this->noDelaySpeed = (0.4 / realTime) * 50;
        }
        else if(this->setSpeedTwo){
            this->realTime = this->middleEndTimeDifference / this->imaginaryTimeRatio;
            this->speed = ((0.4 / realTime) * 50) / *this->trafficLevel;
            this->noDelaySpeed = (0.4 / realTime) * 50;
        }

        this->moveVehicle();
    }
}

/**
 * @brief Simulacia pohybu vozidla v scene
 */
void Vehicle::moveVehicle()
{
    this->countDelay();  // vlastna metoda pre vypocet spozdenia

    if(startReached && !middleReached && !endReached && !moveFromStop && !this->delayEnabled){
       this->moveEnabled = false;
    }

    if(middleReached && !endReached && !moveFromStop && !this->delayEnabled){
       this->moveEnabled = false;
    }

    if(endReached && !moveFromStop && !this->delayEnabled){
        this->moveEnabled = false;
    }

    this->moveFromStop = true;

    // nastavenie vozidla aby sa tocilo medzi zastavkami
    if(this->endReached){
        if(this->endPoint <= 0.5 && this->checkMiddle){
            this->middleReached = true;
            this->moveFromStop = false;
            this->checkMiddle = false;
            this->checkEnd = true;
        }
        if(this->endPoint <= 0.1 && this->checkEnd) {
             this->actualLinesCount++;
             this->startReached = true;
             this->moveFromStop = false;
             this->endReached = false;
             this->checkEnd = false;
             this->middleReached = false;
             this->checkMiddle = true;
             this->delayEndPoint = 0.1;
         }
         else {
            this->endPoint -= this->speed;
            this->delayEndPoint += this->speed;
        }
    }

    if(this->startReached){
        if(this->endPoint >= 0.5 && this->checkMiddle){
            this->middleReached = true;
            this->moveFromStop = false;
            this->checkMiddle = false;
            this->checkEnd = true;
        }
        if(this->endPoint >= 0.9 && this->checkEnd){
            this->actualLinesCount++;
            this->endReached = true;
            this->moveFromStop = false;
            this->startReached = false;
            this->checkEnd = false;
            this->middleReached = false;
            this->checkMiddle = true;
        }
        else {
            this->endPoint += this->speed;
        }
    }

    // nastavenie pozicie na nove suradnice v scene
    this->setPos(this->itemStreet->line().pointAt(this->endPoint).x(),
                 this->itemStreet->line().pointAt(this->endPoint).y());
}

/**
 * @brief Vypocita presnu poziciu v scene pre vozidlo
 * @param currentTime Urcuje, pre ktory cas sa ma vozidlo do sceny nastavit
 */
void Vehicle::countActualPosition(QString currentTime)
{
    auto h = currentTime.split(":")[0].toInt() * 60;
    auto m = currentTime.split(":")[1].toInt();

    auto timeInMin = h + m;

    int newActualLinesCount = 0;

    int storePreviousTime = 0;  // je to nutne pre porovnavanie intervalov mimo trasy

       for(auto const& schedule : this->lineSchedules){
           auto tmpH2 = schedule.at(2).split(":")[0].toInt() * 60;
           auto tmpM2 = schedule.at(2).split(":")[1].toInt();

           storePreviousTime = tmpH2 + tmpM2;
       }

       // iterovanie cez vsetky casy, ktore dana linka obsahuje az dokym nenajde ten spravny
       // v ktorom sa ma vozidlo v scene vykreslit
       for(auto const& schedule : this->lineSchedules){
           auto tmpH0 = schedule.at(0).split(":")[0].toInt() * 60;
           auto tmpM0 = schedule.at(0).split(":")[1].toInt();
           auto tmpH1 = schedule.at(1).split(":")[0].toInt() * 60;
           auto tmpM1 = schedule.at(1).split(":")[1].toInt();
           auto tmpH2 = schedule.at(2).split(":")[0].toInt() * 60;
           auto tmpM2 = schedule.at(2).split(":")[1].toInt();


           auto tmpTimeInMin0 = tmpH0 + tmpM0;
           auto tmpTimeInMin1 = tmpH1 + tmpM1;
           auto tmpTimeInMin2 = tmpH2 + tmpM2;

           // vozidlo sa bude nachadzat v casovom intervale prvej a druhej zastavky
           if(tmpTimeInMin0 < timeInMin && timeInMin <= tmpTimeInMin1){
               this->startMiddleTimeDifference = (((tmpH1 / 60) - (tmpH0 / 60)) * 3600000) + ((tmpM1 - tmpM0) * 60000);
               this->realTime = this->startMiddleTimeDifference / this->imaginaryTimeRatio;
               this->speed = ((0.4 / realTime) * 50) / *this->trafficLevel;
               this->noDelaySpeed = (0.4 / realTime) * 50;

               auto startNewPosTimeDifference = (((h/60) - (tmpH0 / 60)) * 3600000) + ((m - tmpM0) * 60000);
               auto newRealTime = startNewPosTimeDifference / this->imaginaryTimeRatio;
               auto pos = (this->speed * newRealTime) / 50;

               this->setPos(this->itemStreet->line().pointAt(pos + 0.1).x(),
                            this->itemStreet->line().pointAt(pos + 0.1).y());

               this->endPoint = 0.1 + pos;

               this->moveFromStop = true;
               this->delayEnabled = false;
               this->checkDelay = false;
               *this->trafficLevel = 1;

               if(this->endPoint == 0.5){
                   this->moveEnabled = false;
                   this->setSpeedTwo = true;
                   this->setSpeedOne = false;
                   this->moveFromStop = false;
                   this->middleReached = true;
               }
               else{
                   this->moveEnabled = true;
                   this->setSpeedOne = true;
                   this->setSpeedTwo = false;
                   this->moveFromStop = true;
                   this->middleReached = false;
                   this->endReached = false;
                   this->startReached = true;
               }

               break;
           }

           // vozidlo sa bude nachadzat v casovom intervale druhej a tretej zastavky
           else if(tmpTimeInMin1 < timeInMin && timeInMin <= tmpTimeInMin2){
               this->middleEndTimeDifference = (((tmpH2 / 60) - (tmpH1 / 60)) * 3600000) + ((tmpM2 - tmpM1) * 60000);
               this->realTime = this->middleEndTimeDifference / this->imaginaryTimeRatio;
               this->speed = ((0.4 / realTime) * 50) / *this->trafficLevel;
               this->noDelaySpeed = (0.4 / realTime) * 50;

               auto startNewPosTimeDifference = (((h/60) - (tmpH1/60)) * 3600000) + ((m - tmpM1) * 60000);
               auto newRealTime = startNewPosTimeDifference / this->imaginaryTimeRatio;
               auto pos = (this->speed * newRealTime) / 50;

               this->setPos(this->itemStreet->line().pointAt(pos+0.5).x(),
                            this->itemStreet->line().pointAt(pos+0.5).y());

               this->endPoint = 0.5 + pos;
               this->middleReached = true;


               this->checkMiddle = false;
               this->delayEnabled = false;
               this->checkDelay = false;
               *this->trafficLevel = 1;

               if(this->endPoint >= 0.9)  {
                   this->moveEnabled = false;
                   this->moveFromStop = false;
                   this->setSpeedOne = true;
                   this->setSpeedTwo = false;
                   this->middleReached = false;
                   this->endReached = true;
               }
               else{
                   this->setSpeedTwo = true;
                   this->moveEnabled = true;
                   this->setSpeedOne = false;
                   this->moveFromStop = true;
                   this->middleReached = true;
                   this->endReached = false;
               }

               break;
           }

           // vozidlo neobsahuje casovy harmonogram so spravnym intervalom, preto sa umiestni na prvu zastavku
           // s najblizsim casovym intervalom
           else if(tmpTimeInMin0 >= timeInMin && timeInMin > storePreviousTime){
               this->setPos(this->itemStreet->line().pointAt(0.1).x(),
                            this->itemStreet->line().pointAt(0.1).y());

               this->endPoint = 0.1;
               this->moveEnabled = false;
               this->middleReached = false;
               this->startReached = true;
               this->endReached = false;
               this->delayEnabled = false;
               this->checkDelay = false;
               *this->trafficLevel = 1;

               break;
           }

           storePreviousTime = tmpTimeInMin2;

           newActualLinesCount++;
       }

       this->actualLinesCount = newActualLinesCount;
}

/**
 * @brief Spocita meskanie linky
 */
void Vehicle::countDelay()
{

    if(this->appTime->hour() >=0 && this->appTime->hour() <= 11) this->newDay = true;

    if(!this->middleReached){
        if(this->h0 >= 0 && this->h0 <= 11 && !this->newDay) this->newDayScheduleReached = true;
    }
    else{
        if(this->h1 >= 0 && this->h1 <= 11 && !this->newDay) this->newDayScheduleReached = true;
    }

    if(this->newDay && this->newDayScheduleReached){
        this->newDay = false;
        this->newDayScheduleReached = false;
    }

    int delayedTime = 0;
    int noDelayTime = 0;

    // uvazuje spozdenie medzi prvou a druhou zastavkou
    if(!this->middleReached){
        qreal realTime = 0.0;

        if(this->startReached) realTime = (((this->endPoint - 0.1)  * 50) / this->noDelaySpeed) / 60000;
        else if(this->endReached) realTime = (((this->delayEndPoint - 0.1)  * 50) / this->noDelaySpeed) / 60000;

        auto imaginaryTime = realTime * this->imaginaryTimeRatio;


        if(this->newDay && !this->newDayScheduleReached && this->h0 > 20){
                noDelayTime =  (this->appTime->hour()*60  + this->appTime->minute() + 1440);
                delayedTime = imaginaryTime + (this->m0 + 60 * this->h0);
        }

        else if(!this->newDay && this->newDayScheduleReached ){
                noDelayTime =  (this->appTime->hour()*60  + this->appTime->minute());
                delayedTime = imaginaryTime + (this->m0 + 60 * this->h0) + 1440;
        }

        else {
            noDelayTime = this->appTime->hour()*60 + this->appTime->minute();
            delayedTime = imaginaryTime + (this->m0 + 60 * this->h0);
        }
     }

    // uvazuje spozdenie medzi druhou a tretou zastavkou
     else if(this->middleReached){
        qreal realTime = 0.0;

        if(this->startReached) realTime = (((this->endPoint - 0.5)  * 50) / this->noDelaySpeed) / 60000;
        else if(this->endReached) realTime = (((this->delayEndPoint - 0.5)  * 50) / this->noDelaySpeed) / 60000;

        auto imaginaryTime = realTime * this->imaginaryTimeRatio;

        if(this->newDay && !this->newDayScheduleReached  && this->h1 > 20){
                noDelayTime =  (this->appTime->hour()*60  + this->appTime->minute() + 1440);
                delayedTime = imaginaryTime + (this->m1 + 60 * this->h1);
        }
        else if(!this->newDay && this->newDayScheduleReached){
                noDelayTime =  (this->appTime->hour()*60  + this->appTime->minute());
                delayedTime = imaginaryTime + (this->m1 + 60 * this->h1) + 1440;
        }

        else {
            noDelayTime = this->appTime->hour()*60 + this->appTime->minute();
            delayedTime = imaginaryTime + (this->m1 + 60 * this->h1);
        }

    }

    if(noDelayTime - delayedTime <= 1 || this->reset) {
        this->delayEnabled = false;
        this->checkDelay = false;
        this->delay = 0;
        if(this->reset) this->reset = false;
        return;
    }
    this->delay = noDelayTime - delayedTime;

    if(*this->trafficLevel == 1 && this->delay > 1000) {
        this->delay = 0;
    }

    if(this->delay > 1) this->delayEnabled = true;
    else {
        this->delayEnabled = false;
        this->checkDelay = false;
        this->delay = 0;
    }
}

/**
 * @brief Destruktor
 */
Vehicle::~Vehicle()
{

}


