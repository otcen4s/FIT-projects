/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda, vygenerovana pomocou QT a nasladne dotvorena, zaobstarava komunikaciu medzi vsetkymi triedami
 * @file mainwindow.h
 *
 *
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsLineItem>
#include <QTimer>
#include <QDateTime>
#include <QLineF>
#include <QMenu>
#include <QTimeEdit>
#include <QSpinBox>
#include <QDesktopWidget>

#include "files.h"
#include "mapscene.h"
#include "street.h"
#include "transportscene.h"
#include "vehicle.h"
#include "stop.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor hlavneho okna
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor
     */
    ~MainWindow();

private:
    /**
     * @brief Inicializacia grafickej sceny pre mapu
     * @return Ukazatel na graficku scenu
     */
    QGraphicsScene* initMapScene();

    /**
     * @brief Inicializacia grafickej sceny pre grafikon
     * @return Ukazatel na graficku scenu
     */
    QGraphicsScene* initTransportScene();

    QTimer *vehicleTimer;
    QTime *appTime;
    QTimer* appTimer;

    MapScene *mapScene;
    TransportScene *transportScene;
    QGraphicsScene *initialMapScene;

    int h, m, s;
    int interval;


private slots:
    /**
     * @brief Slot pre tlacitko stop
     */
    void vehicleTimerStop();

    /**
     * @brief Slot pre tlacitko start
     */
    void vehicleTimerStart();

    /**
     * @brief Slot pre zoom
     * @param val Hodnota zmenena na slideri
     */
    void zoom(int val);

    /**
     * @brief Vypis casu do labelu
     */
    void time();

    /**
     * @brief Pri zmene casu sa resetuju sceny a nastavia sa do pozicie podla daneho casu
     * @param time Objekt casu, ktory obsahuje dany zmeneny cas
     */
    void timeChanged(const QTime &time);

    /**
     * @brief Reset oboch grafickych scen
     */
    void resetScene();

    /**
     * @brief Zmena rychlosti casu na pomalu
     */
    void slowSpeed();

    /**
     * @brief Zmena rychlosti casu na strednu
     */
    void mediumSpeed();

    /**
     * @brief Zmena rychlosti casu na rychlu
     */
    void fastSpeed();

    /**
     * @brief Slot nastavi casovace na nove hodnoty
     */
    void slowButtonReleased();

    /**
     * @brief Slot nastavi casovace na nove hodnoty
     */
    void mediumButtonReleased();

    /**
     * @brief Slot nastavi casovace na nove hodnoty
     */
    void fastButtonReleased();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
