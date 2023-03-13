/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda obsluhuje simulaciu vozidla na trati
 * @file vehicle.h
 *
 *
 */


#ifndef MYVEHICLE_H
#define MYVEHICLE_H

#include <QObject>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <QTime>

#include "transportscene.h"



class Vehicle : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor inicializuje vsetky potrebne premenne pre vozidlo, vlozi ho do sceny aj s potrebnymi zvysnymi parametrami
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     * @param mapScene Ukazatel na graficku scenu pre mapu
     * @param lines Zoznam, ktory uchovava data o linke v forme stringu
     * @param itemStreet Ukazatel na ulice, na ktorej sa vozdilo nachadza
     * @param transportScene Ukazatel na scenu, urcenu pre grafikon linky
     * @param appTime Vnutorny cas aplikacie
     */
    Vehicle(QGraphicsItem* = nullptr, QGraphicsScene *mapScene = nullptr, QList<QString> lines = {},
              QGraphicsLineItem *itemStreet = nullptr, TransportScene *transportScene = nullptr, QTime* appTime = nullptr);

    /**
     * @brief Destruktor
     */
    ~Vehicle();

    TransportScene *transportScene;
    QGraphicsLineItem *itemStreet;
    QGraphicsLineItem *tmpItemStreet;
    QList<QString> lines;
    QVector<QList<QString>> lineSchedules;

    int totalLinesCount;
    int actualLinesCount;
    int delay;
    uint *trafficLevel;

    QString vehicleColor;
    QString pathColor;

    bool endReached;
    bool startReached;
    bool middleReached;
    bool colorDisplayed;
    bool setSpeedOne;
    bool setSpeedTwo;

    qreal realTime;
    qreal speed;
    qreal noDelaySpeed;
    qreal startMiddleTimeDifference;
    qreal middleEndTimeDifference;
    qreal imaginaryTimeRatio;
    qreal noDelayPos;

    /**
     * @brief Vypocita presnu poziciu v scene pre vozidlo
     * @param currentTime Urcuje, pre ktory cas sa ma vozidlo do sceny nastavit
     */
    void countActualPosition(QString currentTime);

protected:
    /**
     * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
     * @param painter Vykresluje zadane parametre do sceny
     * @param option Nepouzita premenna, potrebna pri prepisani funkcie
     * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    /**
     * @brief Metoda, ktora sa vola pri kazdom kliknuti na objekt elipsy
     * @param event Obsahuje poziciu kliknutia
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief Metoda vytvori ohranicujuci stvorec o danej velkosti
     * @return QRectF Ohranicujuci stvorec
     */
    virtual QRectF boundingRect() const override;

    /**
     * @brief Metoda je volana pri kazdom vyvolanom casovom signale
     * @param phase Urcuje, ci je faza casoveho signalu
     */
    virtual void advance(int phase) override;

private:
    /**
     * @brief Vyfarbi trasu vozdila a zavola metody triedy transportScene pre vypis informacii o vozidle
     * @param event Obsahuje poziciu kliknutia
     */
    void colorRoad(QGraphicsSceneMouseEvent *event);

    /**
     * @brief Simulacia pohybu vozidla v scene
     */
    void moveVehicle();

    /**
     * @brief Spocita meskanie linky
     */
    void countDelay();

    QGraphicsScene *mapScene;
    QTime* appTime;

    qreal startPoint;
    qreal endPoint;
    qreal delayEndPoint;

    bool moveFromStop;
    bool checkEnd;
    bool checkMiddle;
    bool moveEnabled;
    bool delayEnabled;
    bool middleTimeReached;
    bool startTimeReached;
    bool endTimeReached;
    bool checkDelay;
    bool reset;
    bool newDay;
    bool newDayScheduleReached;

    int h0;
    int m0;
    int h1;
    int m1;
    int h2;
    int m2;
};

#endif // MYVEHICLE_H
