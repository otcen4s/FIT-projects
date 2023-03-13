/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda vytvara zastavky na uliciach, ktore zakresli do grafickej sceny
 * @file stop.h
 *
 *
 */


#ifndef STOPS_H
#define STOPS_H

#include <QGraphicsItem>
#include <QObject>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>


class Stop : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor zastavky
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     * @param mapLine Objekt ciary, na ktoru sa dana zastavka vykresli
     * @param mapScene Ukazatel na graficku scenu mapy
     * @param position Pozicia zastavky na ulici
     */
    Stop(QGraphicsItem *parent = Q_NULLPTR, QLineF mapLine = {}, QGraphicsScene *mapScene = nullptr, qreal position = 0.0);

    /**
     * @brief Destruktor
     */
    ~Stop();

    QLineF mapLine;

    qreal stopPos;  // pozicia na ceste(ciare)

    /**
     * @brief Metoda vytvori ohranicujuci stvorec o danej velkosti
     * @return QRectF Ohranicujuci stvorec
     */
    QRectF boundingRect() const override;

protected:
    /**
     * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
     * @param painter Vykresluje zadane parametre do sceny
     * @param option Nepouzita premenna, potrebna pri prepisani funkcie
     * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;



private:
     QGraphicsScene *mapScene{nullptr};
};

#endif // STOPS_H
