/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda vytvara zastavky na uliciach, ktore zakresli do grafickej sceny
 * @file stop.cpp
 *
 *
 */

#include "stop.h"

/**
 * @brief Konstruktor zastavky
 * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
 * @param mapLine Objekt ciary, na ktoru sa dana zastavka vykresli
 * @param mapScene Ukazatel na graficku scenu mapy
 * @param position Pozicia zastavky na ulici
 */
Stop::Stop(QGraphicsItem *parent, QLineF mapLine, QGraphicsScene *mapScene, qreal position) : QObject(), QGraphicsEllipseItem(parent)
{
    this->mapLine = mapLine;
    this->stopPos = position;
    this->setRect(this->mapLine.pointAt(this->stopPos).x(), this->mapLine.pointAt(this->stopPos).y(), 8, 8);

    mapScene->addItem(this);
}

/**
 * @brief Metoda vytvori ohranicujuci stvorec o danej velkosti
 * @return QRectF Ohranicujuci stvorec
 */
QRectF Stop::boundingRect() const
{
    QPainterPath path;
    path.addRect(QGraphicsEllipseItem::boundingRect());
    path.addEllipse(this->mapLine.pointAt(this->stopPos), 12, 12);

    return path.boundingRect();
}

/**
 * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
 * @param painter Vykresluje zadane parametre do sceny
 * @param option Nepouzita premenna, potrebna pri prepisani funkcie
 * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
 */
void Stop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(QBrush(QColor("#cccccc")));
    painter->setPen(QPen({QBrush(QColor("#000000"))}, 2));

    painter->drawEllipse(mapLine.pointAt(this->stopPos), 8 , 8);
}


/**
 * @brief Destruktor
 */
Stop::~Stop()
{

}
