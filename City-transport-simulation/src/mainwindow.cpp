/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda, vygenerovana pomocou QT a nasladne dotvorena, zaobstarava komunikaciu medzi vsetkymi triedami
 * @file mainwindow.cpp
 *
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"


/**
 * @brief Konstruktor hlavneho okna
 * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->h = 17, this->m = 0, this->s = 0;
    this->interval = 10000;

    resize(QDesktopWidget().availableGeometry(this).size() * 1);
    this->setStyleSheet("background-color: #1a1a1a;");

    this->appTimer = new QTimer(this);
    connect(this->appTimer, &QTimer::timeout, this, &MainWindow::time);
    this->appTimer->start(5000);

    this->appTime = new QTime();
    this->appTime->setHMS(17,00,00);
    auto tmpH = this->appTime->toString().split(":")[0];
    auto tmpM = this->appTime->toString().split(":")[1];
    auto str = tmpH + ":" + tmpM;
    ui->timeLabel->setText(str);

    // kurzory
    this->ui->ZoomSlider->setCursor(Qt::PointingHandCursor);
    this->ui->stopButton->setCursor(Qt::PointingHandCursor);
    this->ui->resetButton->setCursor(Qt::PointingHandCursor);
    this->ui->startButton->setCursor(Qt::PointingHandCursor);
    this->ui->fastSpeedButton->setCursor(Qt::PointingHandCursor);
    this->ui->mediumSpeedButton->setCursor(Qt::PointingHandCursor);
    this->ui->slowSpeedButton->setCursor(Qt::PointingHandCursor);
    this->ui->ZoomSlider->setCursor(Qt::PointingHandCursor);
    this->ui->timeEdit->setCursor(Qt::PointingHandCursor);

    // inicializacia grafickych scen
    QGraphicsScene* mapScene = initMapScene();  // inicializacia sceny pre mapu
    QGraphicsScene *transportScene = initTransportScene();  // inicializacia sceny pre zobrazovanie informacii o public transport

    this->transportScene = new TransportScene(transportScene);  // vytvorenie skrytej sceny
    this->mapScene = new MapScene(mapScene, this->transportScene, this->appTime);  // vytvorenie mapoveho podkladu

    // nastavenie casoveho updatu scen
    vehicleTimer = new QTimer(this);
    connect(vehicleTimer, SIGNAL(timeout()), mapScene, SLOT(advance()));  // signal pre scenu na update
    connect(vehicleTimer, SIGNAL(timeout()), transportScene, SLOT(advance()));
    vehicleTimer->start(50);


    // prepojenie signalov a slotov
    connect(ui->timeEdit, &QDateTimeEdit::timeChanged, this, &MainWindow::timeChanged);
    connect(ui->ZoomSlider, &QSlider::valueChanged, this, &MainWindow::zoom);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::vehicleTimerStop);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::vehicleTimerStart);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::resetScene);
    connect(ui->slowSpeedButton, &QPushButton::pressed, this, &MainWindow::slowSpeed);
    connect(ui->mediumSpeedButton, &QPushButton::pressed, this, &MainWindow::mediumSpeed);
    connect(ui->fastSpeedButton, &QPushButton::pressed, this, &MainWindow::fastSpeed);
    connect(ui->fastSpeedButton, &QPushButton::released, this, &MainWindow::fastButtonReleased);
    connect(ui->mediumSpeedButton, &QPushButton::released, this, &MainWindow::mediumButtonReleased);
    connect(ui->slowSpeedButton, &QPushButton::released, this, &MainWindow::slowButtonReleased);
}

/**
 * @brief Inicializacia grafickej sceny pre mapu
 * @return Ukazatel na graficku scenu
 */
QGraphicsScene *MainWindow::initMapScene()
{
    auto mapScene = new MapScene(ui->mapSceneView);  // vytvorenie sceny
    ui->mapSceneView->setScene(mapScene);   // nacitanie sceny
    ui->mapSceneView->setRenderHint(QPainter::Antialiasing); // nastavenie antialiasingu pre primitiva v grafickej scene
    ui->mapSceneView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->mapSceneView->setCacheMode(QGraphicsView::CacheBackground);
    ui->mapSceneView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->mapSceneView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mapScene->setBackgroundBrush(QBrush(QColor("#1a1a1a"), Qt::SolidPattern));
    //mapScene->setSceneRect(-300, -300, 2900, 1800);

    return mapScene;
}

/**
 * @brief Inicializacia grafickej sceny pre grafikon
 * @return Ukazatel na graficku scenu
 */
QGraphicsScene *MainWindow::initTransportScene()
{
    auto transportScene = new TransportScene(ui->transportInfoView);
    ui->transportInfoView->setScene(transportScene);
    ui->transportInfoView->setRenderHints(QPainter::Antialiasing);
    ui->transportInfoView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->transportInfoView->setCacheMode(QGraphicsView::CacheBackground);
    ui->transportInfoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->transportInfoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    transportScene->setBackgroundBrush(QBrush(QColor("#1a1a1a")));

    return transportScene;
}

/**
 * @brief Slot pre tlacitko stop
 */
void MainWindow::vehicleTimerStop()
{
    vehicleTimer->stop();
    this->interval = this->appTimer->interval();
    this->appTimer->stop();
}

/**
 * @brief Slot pre tlacitko start
 */
void MainWindow::vehicleTimerStart()
{
    vehicleTimer->start(50);
    this->appTimer->start(this->interval);
}

/**
 * @brief Slot pre zoom
 * @param val Hodnota zmenena na slideri
 */
void MainWindow::zoom(int val)
{
    ui->mapSceneView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    auto m = ui->mapSceneView->transform();
    qreal scale = val / 20.0;
    ui->mapSceneView->setTransform(QTransform(scale, m.m12(), m.m21(), scale, m.dx(), m.dy()));
}

/**
 * @brief Vypis casu do labelu
 */
void MainWindow::time()
{
    QTime time;

    time = this->appTime->addMSecs(60000);
    this->appTime->setHMS(time.hour(), time.minute(),time.second());
    auto tmpH = this->appTime->toString().split(":")[0];
    auto tmpM = this->appTime->toString().split(":")[1];
    auto str = tmpH + ":" + tmpM;
    ui->timeLabel->setText(str);
}

/**
 * @brief Pri zmene casu sa resetuju sceny a nastavia sa do pozicie podla daneho casu
 * @param time Objekt casu, ktory obsahuje dany zmeneny cas
 */
void MainWindow::timeChanged(const QTime &time)
{
    this->appTime->setHMS(time.hour(), time.minute(),time.second());
    ui->timeLabel->setText(time.toString());

    this->h = time.hour(), this->m = time.minute(), this->s = time.second();
    this->resetScene();
    this->appTimer->stop();
    this->vehicleTimer->stop();

    for(auto const& vehicle : this->mapScene->vehicles){
        vehicle->countActualPosition(time.toString());
    }
    this->appTimer->start(5000);
    vehicleTimer->start(50);
}

/**
 * @brief Reset oboch grafickych scen
 */
void MainWindow::resetScene()
{
    delete this->transportScene;
    delete this->mapScene;
    delete this->appTime;
    delete this->vehicleTimer;

    this->appTime = new QTime();
    this->appTime->setHMS(this->h,this->m,this->s);
    auto tmpH = this->appTime->toString().split(":")[0];
    auto tmpM = this->appTime->toString().split(":")[1];
    auto str = tmpH + ":" + tmpM;
    ui->timeLabel->setText(str);


    QGraphicsScene* mapScene = initMapScene();  // inicializacia sceny pre mapu
    QGraphicsScene *transportScene = initTransportScene();  // inicializacia sceny pre zobrazovanie informacii o public transport

    this->transportScene = new TransportScene(transportScene);  // vytvorenie skrytej sceny

    this->mapScene = new MapScene(mapScene, this->transportScene, this->appTime);  // vytvorenie mapoveho podkladu

    this->appTimer->start(5000);
    this->interval = 5000;

    vehicleTimer = new QTimer(this);
    connect(vehicleTimer, SIGNAL(timeout()), mapScene, SLOT(advance()));  // signal pre scenu na update
    connect(vehicleTimer, SIGNAL(timeout()), transportScene, SLOT(advance()));
    vehicleTimer->start(50);

    this->h = 17, this->m = 0, this->s = 0;
}

/**
 * @brief Zmena rychlosti casu na pomalu
 */
void MainWindow::slowSpeed()
{
    this->vehicleTimer->stop();
    this->appTimer->stop();

    for(auto const& vehicle : this->mapScene->vehicles){
        vehicle->imaginaryTimeRatio = 6;
        vehicle->realTime = 0.0;

        if(vehicle->startReached && !vehicle->middleReached){
             vehicle->setSpeedTwo = false;
             vehicle->setSpeedOne = true;
             vehicle->realTime = vehicle->startMiddleTimeDifference / vehicle->imaginaryTimeRatio;
             vehicle->speed = ((0.4 / vehicle->realTime) * 50)  / *vehicle->trafficLevel;
             vehicle->noDelaySpeed = (0.4 / vehicle->realTime) * 50;
        }
        if(vehicle->middleReached){
            vehicle->setSpeedTwo = true;
            vehicle->setSpeedOne = false;
            vehicle->realTime = vehicle->middleEndTimeDifference / vehicle->imaginaryTimeRatio ;
            vehicle->speed = ((0.4 / vehicle->realTime) * 50)  / *vehicle->trafficLevel;
            vehicle->noDelaySpeed = (0.4 / vehicle->realTime) * 50;
        }

        this->interval = 10000;
    }
}

/**
 * @brief Zmena rychlosti casu na strednu
 */
void MainWindow::mediumSpeed()
{
    this->vehicleTimer->stop();
    this->appTimer->stop();

    for(auto const& vehicle : this->mapScene->vehicles){

        vehicle->imaginaryTimeRatio = 12;  // 60 / 5
        vehicle->realTime = 0.0;

        if(vehicle->startReached && !vehicle->middleReached){
             vehicle->setSpeedTwo = false;
             vehicle->setSpeedOne = true;
             vehicle->realTime = vehicle->startMiddleTimeDifference / vehicle->imaginaryTimeRatio;
             vehicle->speed = ((0.4 / vehicle->realTime) * 50)  / *vehicle->trafficLevel;
             vehicle->noDelaySpeed = (0.4 / vehicle->realTime) * 50;
        }
        if(vehicle->middleReached){
            vehicle->realTime = vehicle->middleEndTimeDifference / vehicle->imaginaryTimeRatio;
            vehicle->setSpeedTwo = true;
            vehicle->setSpeedOne = false;
            vehicle->speed = ((0.4 / vehicle->realTime) * 50)  / *vehicle->trafficLevel;
            vehicle->noDelaySpeed = (0.4 / vehicle->realTime) * 50;
        }
        this->interval = 5000;

    }

}

/**
 * @brief Zmena rychlosti casu na rychlu
 */
void MainWindow::fastSpeed()
{
    this->vehicleTimer->stop();
    this->appTimer->stop();

    for(auto const& vehicle : this->mapScene->vehicles){

        vehicle->imaginaryTimeRatio = 120;  // 60 / 0.5

        if(vehicle->startReached && !vehicle->middleReached){
             vehicle->setSpeedTwo = false;
             vehicle->setSpeedOne = true;
             vehicle->realTime = vehicle->startMiddleTimeDifference / vehicle->imaginaryTimeRatio;
             vehicle->speed = ((0.4 / vehicle->realTime) * 50)  / (*vehicle->trafficLevel);
             vehicle->noDelaySpeed = (0.4 / vehicle->realTime) * 50;
        }
        if(vehicle->middleReached){
            vehicle->setSpeedTwo = true;
            vehicle->setSpeedOne = false;
            vehicle->realTime = vehicle->middleEndTimeDifference / vehicle->imaginaryTimeRatio ;
            vehicle->speed = ((0.4 / vehicle->realTime) * 50)  / (*vehicle->trafficLevel);
            vehicle->noDelaySpeed = (0.4 / vehicle->realTime) * 50;
        }
        this->interval = 500;
    }
}

/**
 * @brief Slot nastavi casovace na nove hodnoty
 */
void MainWindow::slowButtonReleased()
{
    this->vehicleTimer->start(50);
    this->appTimer->start(this->interval);
}

/**
 * @brief Slot nastavi casovace na nove hodnoty
 */
void MainWindow::mediumButtonReleased()
{
    this->vehicleTimer->start(50);
    this->appTimer->start(this->interval);
}

/**
 * @brief Slot nastavi casovace na nove hodnoty
 */
void MainWindow::fastButtonReleased()
{
    this->vehicleTimer->start(50);
    this->appTimer->start(this->interval);
}


/**
 * @brief Destruktor
 */
MainWindow::~MainWindow()
{
    delete appTime;
    delete appTimer;
    delete ui;
    delete vehicleTimer;
}
