/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda vytvara ulice na grafickej scene
 * @file street.h
 *
 *
 */

#ifndef STREETS_H
#define STREETS_H

#include <QGraphicsLineItem>
#include <QPen>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

#include "transportscene.h"
#include "files.h"


class Street : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor objektu ulice
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     * @param x1 Suradnica x prveho bodu ulice
     * @param y1 Suradnica y prveho bodu ulice
     * @param x2 Suradnica x druheho bodu ulice
     * @param y2 Suradnica y druheho bodu ulice
     * @param mapScene Ukazatel na graficku scenu mapy
     * @param streetName Nazov ulice
     * @param transportScene Ukazatel na graficku scenu transportu
     */
    Street(QGraphicsItem *parent = Q_NULLPTR, int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0,
           QGraphicsScene *mapScene = Q_NULLPTR, QString streetName = "",  TransportScene *transportScene = Q_NULLPTR);

    /**
    * @brief Destruktor
    */
    ~Street();

    QString streetName;
    QList<QString> lines;
    uint trafficLevel;

protected:
    /**
     * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
     * @param painter Vykresluje zadane parametre do sceny
     * @param option Nepouzita premenna, potrebna pri prepisani funkcie
     * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    /**
     * @brief Metoda, ktora sa vola pri kazdom kliknuti na objekt ulice
     * @param event Obsahuje poziciu kliknutia
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsScene *mapScene{nullptr};
    TransportScene *transportScene{nullptr};
    Files File;

    bool windowShowed;
    QWidget *window;
    QPushButton *trafficButton1;
    QPushButton *trafficButton2;
    QPushButton *trafficButton3;
    QPushButton *trafficButton4;
    QPushButton *trafficButton5;

private slots:
    /**
     * @brief Slot pri stlaceni tlacitka pre normal traffic
     */
    void normalTraffic();

    /**
     * @brief Slot pri stlaceni tlacitka pre light traffic
     */
    void lightTraffic();

    /**
     * @brief Slot pri stlaceni tlacitka pre medium traffic
     */
    void mediumTraffic();

    /**
     * @brief Slot pri stlaceni tlacitka pre big traffic
     */
    void bigTraffic();

    /**
     * @brief Slot pri stlaceni tlacitka pre terrible traffic
     */
    void terribleTraffic();

    /**
     * @brief Slot pri pusteni tlacitka pre normal traffic
     */
    void normalTrafficReleased();

    /**
     * @brief  Slot pri pusteni tlacitka pre light traffic
     */
    void lightTrafficReleased();

    /**
     * @brief  Slot pri pusteni tlacitka pre medium traffic
     */
    void mediumTrafficReleased();

    /**
     * @brief  Slot pri pusteni tlacitka pre big traffic
     */
    void bigTrafficReleased();

    /**
     * @brief Slot pri pusteni tlacitka pre terrible traffic
     */
    void terribleTrafficReleased();

};

#endif // STREETS_H
