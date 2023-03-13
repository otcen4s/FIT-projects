/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda vykresluje grafikon linky do grafickej sceny TransportScene
 * @file graphicon.cpp
 *
 *
 */

#include "graphicon.h"

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

Graphicon::Graphicon(QGraphicsItem *parent, QGraphicsScene *transportScene, qreal position, QList<QString> lines,
                 QList<QString> lineSchedules, QString vehicleColor, bool endReached, uint delay, bool middleReached, bool startReached) : QGraphicsLineItem(parent)
{
    this->transportScene = transportScene;
    this->position = position;
    this->lines = lines;
    this->lineSchedules = lineSchedules;
    this->vehicleColor = vehicleColor;
    this->endReached = endReached;
    this->middleReached = middleReached;
    this->startReached = startReached;
    this->delay = delay;

    setLine(100, 400, 700, 400);
    transportScene->addItem(this);
}

/**
 * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
 * @param painter Vykresluje zadane parametre do sceny
 * @param option Nepouzita premenna, potrebna pri prepisani funkcie
 * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
 */
void Graphicon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Ciara
    painter->setPen(QPen({QBrush(QColor("#666666"))}, 3));
    painter->drawLine(this->line());


    painter->drawLine(100, 410, 100, 390);

    painter->drawLine(700, 410, 700, 390);

    // Vozidlo
    painter->setBrush(QBrush(QColor(this->vehicleColor)));
    painter->setPen(QPen({QBrush(QColor("#000000"))}, 2));

    painter->drawEllipse(this->line().pointAt(this->position), 7, 7);

    if(this->endReached){
        this->endDirection(painter);
    }
    else{
        this->startDirection(painter);
    }

    // Nazvy zastavok
    QFont font;
    font.setFamily("Open Sans");
    font.setPixelSize(13);
    painter->setFont(font);


    painter->setPen(QPen({QBrush(QColor("#0099e6"))}, 4));
    QPointF p = this->line().pointAt(0.1);
    p.setX(p.rx() - 15);
    p.setY(p.ry() - 20);
    painter->drawText(p, this->lines.at(1));
    p = this->line().pointAt(0.5);
    p.setX(p.rx() - 15);
    p.setY(p.ry() - 20);
    painter->drawText(p, this->lines.at(2));
    p = this->line().pointAt(0.9);
    p.setX(p.rx() - 15);
    p.setY(p.ry() - 20);
    painter->drawText(p, this->lines.at(3));


    // Linka
    painter->setPen(QPen({QBrush(QColor("#0099e6"))}, 4));
    font.setFamily("Open Sans");
    font.setPixelSize(20);
    painter->setFont(font);

    p = this->line().pointAt(0.5);
    p.setY(p.ry() - 50);
    p.setX(p.rx() - 40);

    QString str = "Line " + this->lines.at(0);
    painter->drawText(p, str);

}

/**
 * @brief V pripade, ze dana linka ide z pociatocneho smeru, tak sa vola funkcia na spravne vykreslenie grafikonu
 * @param painter Vykresluje zadane parametre do sceny
 */
void Graphicon::startDirection(QPainter *painter)
{

    painter->setBrush(QBrush(Qt::NoBrush));
    painter->setPen(QPen({QBrush(QColor("#cccccc"))}, 2));

    // Zastavky
    if(this->line().pointAt(this->position).x() >= this->line().pointAt(0.0).x()) {
        painter->setBrush(QBrush(QColor("#cccccc")));
        painter->drawEllipse(this->line().pointAt(0.1), 7, 7);
        painter->setBrush(QBrush(Qt::NoBrush));

     }
    else painter->drawEllipse(this->line().pointAt(0.1), 7, 7);


    if(this->line().pointAt(this->position).x() >= this->line().pointAt(0.5).x()) {
        painter->setBrush(QBrush(QColor("#cccccc")));
        painter->drawEllipse(this->line().pointAt(0.5), 7, 7);
        painter->setBrush(QBrush(Qt::NoBrush));
    }
    else painter->drawEllipse(this->line().pointAt(0.5), 7, 7);

    if(this->line().pointAt(this->position).x() >= this->line().pointAt(1.0).x()) {
        painter->setBrush(QBrush(QColor("#cccccc")));
        painter->drawEllipse(this->line().pointAt(0.9), 7, 7);
        painter->setBrush(QBrush(Qt::NoBrush));
    }
    else painter->drawEllipse(this->line().pointAt(0.9), 7, 7);

    QFont font;
    QPointF p;

    // Casy
    painter->setPen(QPen({QBrush(QColor("#d9d9d9"))}, 5));
    font.setFamily("Open Sans");
    font.setPixelSize(10);
    painter->setFont(font);

    p = this->line().pointAt(0.1);
    p.setX(p.rx() - 15);
    p.setY(p.ry() + 25);
    if(this->delay > 1){
        painter->drawText(p, this->lineSchedules.at(0));

        if(!this->startReached){
            p.setX(p.rx() + 40);

            if(this->delay <= 5) painter->setPen(QPen({QBrush(QColor("#66ff33"))}, 5));
            else if(this->delay > 5 && this->delay < 10) painter->setPen(QPen({QBrush(QColor("#ff9900"))}, 5));
            else painter->setPen(QPen({QBrush(QColor("#ff3300"))}, 5));

            painter->drawText(p, "(+" + QString::number(this->delay) + ")");
        }
    }
    else painter->drawText(p, this->lineSchedules.at(0));

    painter->setPen(QPen({QBrush(QColor("#d9d9d9"))}, 5));
    p = this->line().pointAt(0.5);
    p.setX(p.rx() - 15);
    p.setY(p.ry() + 25);
    if(this->delay > 1){
        painter->drawText(p, this->lineSchedules.at(1));

        if(!this->middleReached){
            p.setX(p.rx() + 40);

            if(this->delay <= 5) painter->setPen(QPen({QBrush(QColor("#66ff33"))}, 5));
            else if(this->delay > 5 && this->delay < 10) painter->setPen(QPen({QBrush(QColor("#ff9900"))}, 5));
            else painter->setPen(QPen({QBrush(QColor("#ff3300"))}, 5));

            painter->drawText(p, "(+" + QString::number(this->delay) + ")");
        }
    }
    else painter->drawText(p, this->lineSchedules.at(1));

    painter->setPen(QPen({QBrush(QColor("#d9d9d9"))}, 5));
    p = this->line().pointAt(0.9);
    p.setX(p.rx() - 15);
    p.setY(p.ry() + 25);
    if(this->delay > 1){
        painter->drawText(p, this->lineSchedules.at(2));

        if(!this->endReached){
            p.setX(p.rx() + 40);

            if(this->delay <= 5) painter->setPen(QPen({QBrush(QColor("#66ff33"))}, 5));
            else if(this->delay > 5 && this->delay < 10) painter->setPen(QPen({QBrush(QColor("#ff9900"))}, 5));
            else painter->setPen(QPen({QBrush(QColor("#ff3300"))}, 5));

            painter->drawText(p, "(+" + QString::number(this->delay) + ")");
        }
    }
    else painter->drawText(p, this->lineSchedules.at(2));
}

/**
 * @brief V pripade, ze sa dana linka vracia z druheho smeru, tak sa vola funkcia na spravne vykreslenie grafikonu
 * @param painter Vykresluje zadane parametre do sceny
 */
void Graphicon::endDirection(QPainter *painter)
{

    painter->setBrush(QBrush(Qt::NoBrush));
    painter->setPen(QPen({QBrush(QColor("#cccccc"))}, 2));

    // Zastavky
    if(this->line().pointAt(this->position).x() <= this->line().pointAt(0.0).x()) {
        painter->setBrush(QBrush(QColor("#cccccc")));
        painter->drawEllipse(this->line().pointAt(0.1), 7, 7);
        painter->setBrush(QBrush(Qt::NoBrush));

     }
    else painter->drawEllipse(this->line().pointAt(0.1), 7, 7);


    if(this->line().pointAt(this->position).x() <= this->line().pointAt(0.5).x()) {
        painter->setBrush(QBrush(QColor("#cccccc")));
        painter->drawEllipse(this->line().pointAt(0.5), 7, 7);
        painter->setBrush(QBrush(Qt::NoBrush));
    }
    else painter->drawEllipse(this->line().pointAt(0.5), 7, 7);

    if(this->line().pointAt(this->position).x() <= this->line().pointAt(1.0).x()) {
        painter->setBrush(QBrush(QColor("#cccccc")));
        painter->drawEllipse(this->line().pointAt(0.9), 7, 7);
        painter->setBrush(QBrush(Qt::NoBrush));
    }
    else painter->drawEllipse(this->line().pointAt(0.9), 7, 7);

    QFont font;
    QPointF p;

    // Casy
    painter->setPen(QPen({QBrush(QColor("#d9d9d9"))}, 5));
    font.setFamily("Open Sans");
    font.setPixelSize(10);
    painter->setFont(font);


    p = this->line().pointAt(0.1);
    p.setX(p.rx() - 15);
    p.setY(p.ry() + 25);
    if(this->delay > 1){
        painter->drawText(p, this->lineSchedules.at(2));

        if(!this->startReached){
            p.setX(p.rx() + 40);

            if(this->delay <= 5) painter->setPen(QPen({QBrush(QColor("#66ff33"))}, 5));
            else if(this->delay > 5 && this->delay < 10) painter->setPen(QPen({QBrush(QColor("#ff9900"))}, 5));
            else painter->setPen(QPen({QBrush(QColor("#ff3300"))}, 5));

            painter->drawText(p, "(+" + QString::number(this->delay) + ")");
        }
    }
    else painter->drawText(p, this->lineSchedules.at(2));

    painter->setPen(QPen({QBrush(QColor("#d9d9d9"))}, 5));
    p = this->line().pointAt(0.5);
    p.setX(p.rx() - 15);
    p.setY(p.ry() + 25);
    if(this->delay > 1){
        painter->drawText(p, this->lineSchedules.at(1));

        if(!this->middleReached){
            p.setX(p.rx() + 40);

            if(this->delay <= 5) painter->setPen(QPen({QBrush(QColor("#66ff33"))}, 5));
            else if(this->delay > 5 && this->delay < 10) painter->setPen(QPen({QBrush(QColor("#ff9900"))}, 5));
            else painter->setPen(QPen({QBrush(QColor("#ff3300"))}, 5));

            painter->drawText(p, "(+" + QString::number(this->delay) + ")");
        }
    }
    else painter->drawText(p, this->lineSchedules.at(1));

    painter->setPen(QPen({QBrush(QColor("#d9d9d9"))}, 5));
    p = this->line().pointAt(0.9);
    p.setX(p.rx() - 15);
    p.setY(p.ry() + 25);
    if(this->delay > 1){
        painter->drawText(p, this->lineSchedules.at(0));

        if(!this->endReached){
            p.setX(p.rx() + 40);

            if(this->delay <= 5) painter->setPen(QPen({QBrush(QColor("#66ff33"))}, 5));
            else if(this->delay > 5 && this->delay < 10) painter->setPen(QPen({QBrush(QColor("#ff9900"))}, 5));
            else painter->setPen(QPen({QBrush(QColor("#ff3300"))}, 5));

            painter->drawText(p, "(+" + QString::number(this->delay) + ")");
        }
    }
    else painter->drawText(p, this->lineSchedules.at(0));
}

/**
 * @brief Destruktor
 */
Graphicon::~Graphicon()
{

}

