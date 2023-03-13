/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda vytvara ulice na grafickej scene
 * @file street.cpp
 *
 *
 */

#include "street.h"

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
Street::Street(QGraphicsItem *parent, int x1, int y1, int x2, int y2, QGraphicsScene *mapScene, QString streetName,  TransportScene *transportScene) :  QObject(), QGraphicsLineItem(parent)
{
    this->streetName = streetName;
    this->trafficLevel = 1;
    this->transportScene = transportScene;

    QPointF p1;
    QPointF p2;

    p1.setX(x1);
    p1.setY(y1);
    p2.setX(x2);
    p2.setY(y2);

    QLineF l;
    l.setP1(p1);
    l.setP2(p2);


    this->setLine(l);
    mapScene->addItem(this);
    this->setCursor(Qt::PointingHandCursor);
    this->mapScene = mapScene;


    this->windowShowed = false;
    this->window = new QWidget;
    this->trafficButton1 = new QPushButton("Normal Traffic");
    this->trafficButton2 = new QPushButton("Light Traffic");
    this->trafficButton3 = new QPushButton("Medium Traffic");
    this->trafficButton4 = new QPushButton("Big Traffic");
    this->trafficButton5 = new QPushButton("Terrible Traffic");

    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1"));
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2"));
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3"));
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4"));
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5"));

    QLabel *label = new QLabel();
    label->setText("TRAFFIC LEVEL");
    label->setStyleSheet("color: #a6a6a6;\nfont: bold");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(trafficButton1);
    layout->addWidget(trafficButton2);
    layout->addWidget(trafficButton3);
    layout->addWidget(trafficButton4);
    layout->addWidget(trafficButton5);

    this->window->setLayout(layout);
    this->window->setGeometry(x1 + 10, y1 + 10, 100, 100);
    this->window->setStyleSheet("background-color: #404040");
    this->mapScene->addWidget(this->window);
    this->window->hide();

    connect(this->trafficButton1, &QPushButton::pressed, this, &Street::normalTraffic);
    connect(this->trafficButton1, &QPushButton::released, this, &Street::normalTrafficReleased);
    connect(this->trafficButton2, &QPushButton::pressed, this, &Street::lightTraffic);
    connect(this->trafficButton2, &QPushButton::released, this, &Street::lightTrafficReleased);
    connect(this->trafficButton3, &QPushButton::pressed, this, &Street::mediumTraffic);
    connect(this->trafficButton3, &QPushButton::released, this, &Street::mediumTrafficReleased);
    connect(this->trafficButton4, &QPushButton::pressed, this, &Street::bigTraffic);
    connect(this->trafficButton4, &QPushButton::released, this, &Street::bigTrafficReleased);
    connect(this->trafficButton5, &QPushButton::pressed, this, &Street::terribleTraffic);
    connect(this->trafficButton5, &QPushButton::released, this, &Street::terribleTrafficReleased);
}

/**
 * @brief Prepisana funkcia zdedeneho objektu, pomocou ktorej sa vykresluje do sceny
 * @param painter Vykresluje zadane parametre do sceny
 * @param option Nepouzita premenna, potrebna pri prepisani funkcie
 * @param widget Nepouzita premenna, potrebna pri prepisani funkcie
 */
void Street::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen({QBrush(QColor("#737373"))}, 3));
    painter->drawLine(this->line());


    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(30);
    painter->setFont(font);


    QPointF p = this->line().pointAt(0.3);

    p.setX(p.rx() - 35);

    painter->setPen(QPen({QBrush(QColor("#0099e6"))}, 4));
    painter->drawText(p, this->streetName);
}

/**
 * @brief Metoda, ktora sa vola pri kazdom kliknuti na objekt ulice
 * @param event Obsahuje poziciu kliknutia
 */
void Street::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!this->windowShowed){
        this->window->setGeometry(event->pos().x() , event->pos().y(), 100, 100);
        this->window->show();
        this->windowShowed = true;
    }
    else{
        this->window->hide();
        this->windowShowed = false;
    }

    QGraphicsLineItem::mousePressEvent(event);
}

/**
 * @brief Slot pri stlaceni tlacitka pre normal traffic
 */
void Street::normalTraffic()
{
    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1")+ "background-color: #bfbfbf;");
}

/**
 * @brief Slot pri stlaceni tlacitka pre light traffic
 */
void Street::lightTraffic()
{
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2")+ "background-color: #bfbfbf");
}

/**
 * @brief Slot pri stlaceni tlacitka pre medium traffic
 */
void Street::mediumTraffic()
{
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3")+ "background-color: #bfbfbf");
}

/**
 * @brief Slot pri stlaceni tlacitka pre big traffic
 */
void Street::bigTraffic()
{
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4")+ "background-color: #bfbfbf");
}

/**
 * @brief Slot pri stlaceni tlacitka pre terrible traffic
 */
void Street::terribleTraffic()
{
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5")+ "background-color: #bfbfbf");

}

/**
 * @brief Slot pri pusteni tlacitka pre normal traffic
 */
void Street::normalTrafficReleased()
{
    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1") + "border-width: 4px;" + "border-color: #0066ff");
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2"));
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3"));
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4"));
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5"));

    this->window->hide();
    this->windowShowed = false;
    this->trafficLevel = 1;

}

/**
 * @brief  Slot pri pusteni tlacitka pre light traffic
 */
void Street::lightTrafficReleased()
{
    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1"));
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2") + "border-width: 4px;" + "border-color: #0066ff");
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3"));
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4"));
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5"));

    this->window->hide();
    this->windowShowed = false;
    this->trafficLevel = 2;
}

/**
 * @brief  Slot pri pusteni tlacitka pre medium traffic
 */
void Street::mediumTrafficReleased()
{
    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1"));
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2"));
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3") + "border-width: 4px;" + "border-color: #0066ff");
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4"));
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5"));
    this->window->hide();
    this->windowShowed = false;
    this->trafficLevel = 3;
}

/**
 * @brief  Slot pri pusteni tlacitka pre big traffic
 */
void Street::bigTrafficReleased()
{
    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1"));
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2"));
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3"));
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4") + "border-width: 4px;" + "border-color: #0066ff");
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5"));
    this->window->hide();
    this->windowShowed = false;
    this->trafficLevel = 4;
}

/**
 * @brief Slot pri pusteni tlacitka pre terrible traffic
 */
void Street::terribleTrafficReleased()
{
    this->trafficButton1->setStyleSheet(this->File.readStyleSheet("trafficLevelButton1"));
    this->trafficButton2->setStyleSheet(this->File.readStyleSheet("trafficLevelButton2"));
    this->trafficButton3->setStyleSheet(this->File.readStyleSheet("trafficLevelButton3"));
    this->trafficButton4->setStyleSheet(this->File.readStyleSheet("trafficLevelButton4"));
    this->trafficButton5->setStyleSheet(this->File.readStyleSheet("trafficLevelButton5") + "border-width: 4px;" + "border-color: #0066ff");
    this->window->hide();
    this->windowShowed = false;
    this->trafficLevel = 5;
}

/**
 * @brief Destruktor
 */
Street::~Street()
{

}

