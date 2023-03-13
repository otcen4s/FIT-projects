/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Main bol vygenerovany pomocou QT spusta vysledny program pomocou obejektu 'w' a 'a' tried MainWindow a QApplication
 * @file main.cpp
 *
 *
 */


#include "mainwindow.h"

#include <QApplication>

/**
 * @brief Hlavna funkcia vygenerovana QT
 * @param argc Parameter vygenerovany QT
 * @param argv Parameter vygenerovany QT
 * @return Vrati vyslednu spustitelnu aplikaciu
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
