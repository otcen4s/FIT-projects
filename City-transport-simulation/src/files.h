/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda pre spracovanie vstupnych dat zo suborov
 * @file files.h
 *
 *
 */

#ifndef FILES_H
#define FILES_H


#include <QObject>
#include <QIODevice>
#include <QDebug>
#include <QFile>

class Files : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor
     * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
     */
    explicit Files(QObject *parent = nullptr);

    /**
     * @brief Destruktor
     */
    ~Files();

public:
    /**
     * @brief Citanie suboru, ktory je vo formate CSV a obsahuje suradnice ciar pre mapovy podklad
     * @return Mapu, ktora obsahuje suradnice ciar
     */
    QMap<int, QList<int>> readCSVmap();

    /**
     * @brief Citanie suboru, ktory je vo formate TXT a obsahuje nazvy ulic
     * @return Zoznam, ktory obsahuje nazvy ulic
     */
    QList<QString> readTXTstreetNames();

    /**
     * @brief Citanie suboru, ktory je vo formate CSV a obsahuje nazvy zastavok
     * @return Mapu, ktora obsahuje informacie nazvy zastavok
     */
    QMap<int, QList<QString>> readCSVstops();

    /**
     * @brief Citanie suboru, ktory je vo formate CSV a obsahuje informacie o linkach
     * @return  Mapu, ktora obsahuje informacie o linkach
     */
    QMap<int, QList<QString>> readCSVlines();

    /**
     * @brief Citanie suboru, ktory je vo formate CSV a obsahuje casovy harmonogram
     * @return Mapu, ktora obsahuje casovy harmonogram
     */
    QMap<QString, QVector<QList<QString>>> readCSVlinesSchedule();

    /**
     * @brief Citanie suboru so stylovym predpisom
     * @param filename Nazov suboru
     * @return Precitany subor vo forme stringu
     */
    QString readStyleSheet(QString filename);
};

#endif // FILES_H
