/**
 * Predmet ICP - FIT VUT Brno, 2020
 * ICP 2020 - Projekt simulacie mestskej hromadnej dopravy
 *
 * @author Matej Otcenas, xotcen01
 *
 * @brief Trieda pre spracovanie vstupnych dat zo suborov
 * @file files.cpp
 *
 *
 */


#include "files.h"

/**
 * @brief Konstruktor
 * @param parent Ukazatel na objekt, z ktoreho dana trieda dedi
 */
Files::Files(QObject *parent) : QObject(parent)
{

}


/**
 * @brief Citanie suboru, ktory je vo formate CSV a obsahuje suradnice ciar pre mapovy podklad
 * @return Mapu, ktora obsahuje suradnice ciar
 */
QMap <int, QList<int>> Files::readCSVmap()
{
    QMap <int, QList<int>> lines{};

    QFile file("../files/map_layer.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return lines;  // nacitanie suboru zlyhalo
    }
    QTextStream input(&file);  // ulozenie suboru


    QList<QString> axisValuesInString;  // len pre nacitanie zo suboru je to vo forme stringu
    QList<int> axisValues;  // vysledny zoznam v int hodnotach
    int lineCounter = 0;    // prvy riadok sa bude preskakovat v CSV obsahuje iba informacie o stlpcoch

    // citanie zo suboru
    while (!input.atEnd()) {
        QString line = input.readLine();

        if(lineCounter){
            axisValuesInString.append(line.split(','));

            for(auto const& item : axisValuesInString){     // iterovanie cez list
                axisValues.append(item.toInt());
            }

            lines[lineCounter] = axisValues;
            axisValuesInString.clear();     // docasne zoznamy sa kazdu iteraciu premazu
            axisValues.clear();
        }
        lineCounter++;
    }

    file.close();
    return lines;
}

/**
 * @brief Citanie suboru, ktory je vo formate TXT a obsahuje nazvy ulic
 * @return Zoznam, ktory obsahuje nazvy ulic
 */
QList<QString> Files::readTXTstreetNames()
{
    QList <QString> streets{};

    QFile file("../files/street_names.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return streets;  // nacitanie suboru zlyhalo
    }
    QTextStream input(&file);  // ulozenie suboru

    int pos = 0;
    // citanie zo suboru
    while (!input.atEnd()) {
        QString street = input.readLine();

        if(street.at(0) != "\0" && street.at(0) != "\n"){
            streets.insert(pos, street);
        }
        pos++;
    }

    file.close();
    return streets;
}

/**
 * @brief Citanie suboru, ktory je vo formate CSV a obsahuje nazvy zastavok
 * @return Mapu, ktora obsahuje informacie nazvy zastavok
 */
QMap<int, QList<QString>> Files::readCSVstops()
{
    QMap <int, QList<QString>> stops{};  // mapa, ktora bude obsahovat kluc(nazov zastavky) a zoznam zastavok(ST0,M0,SP0)

    QFile file("../files/stops.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return stops;  // nacitanie suboru zlyhalo
    }
    QTextStream input(&file);  // ulozenie suboru

    QList<QString> lStops;  // vysledny zoznam zastavok
    int stopsCounter = 0;    // prvy riadok sa bude preskakovat v CSV obsahuje iba informacie o stlpcoch

    // citanie zo suboru
    while (!input.atEnd()) {
        QString line = input.readLine();

        if(stopsCounter){
            lStops.append(line.split(','));

            stops[stopsCounter] = lStops;
            lStops.clear();     // docasny zoznam sa kazdu iteraciu premaze
        }
        stopsCounter++;
    }

    file.close();
    return stops;
}

/**
 * @brief Citanie suboru, ktory je vo formate CSV a obsahuje informacie o linkach
 * @return  Mapu, ktora obsahuje informacie o linkach
 */
QMap<int, QList<QString> > Files::readCSVlines()
{
    QMap <int, QList<QString>> lines{};

    QFile file("../files/lines.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return lines;  // nacitanie suboru zlyhalo
    }
    QTextStream input(&file);  // ulozenie suboru

    QList<QString> lLines;  // vysledny zoznam liniek
    int stopsCounter = 0;    // prvy riadok sa bude preskakovat v CSV obsahuje iba informacie o stlpcoch

    // citanie zo suboru
    while (!input.atEnd()) {
        QString line = input.readLine();

        if(stopsCounter){
            lLines.append(line.split(','));

            lines[stopsCounter] = lLines;
            lLines.clear();     // docasny zoznam sa kazdu iteraciu premaze
        }
        stopsCounter++;
    }

    file.close();
    return lines;
}

/**
 * @brief Citanie suboru, ktory je vo formate CSV a obsahuje casovy harmonogram
 * @return Mapu, ktora obsahuje casovy harmonogram
 */
QMap<QString, QVector<QList<QString>>> Files::readCSVlinesSchedule()
{
    QMap <QString, QVector<QList<QString>>> linesSchedules{};

    QFile file("../files/lines_schedule.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return linesSchedules;  // nacitanie suboru zlyhalo
    }
    QTextStream input(&file);  // ulozenie suboru

    QList<QString> lLines;
    int linesCounter = 0;    // prvy riadok sa bude preskakovat v CSV obsahuje iba informacie o stlpcoch

    // citanie zo suboru
    while (!input.atEnd()) {
        QString line = input.readLine();
        QString tmp;

        if(linesCounter){
            lLines.append(line.split(','));
            tmp = lLines.at(0);

            lLines.removeAt(0);

            linesSchedules[tmp].push_back(lLines);

            lLines.clear();     // docasny zoznam sa kazdu iteraciu premaze
        }
        linesCounter++;
    }

    file.close();
    return linesSchedules;
}

/**
 * @brief Citanie suboru so stylovym predpisom
 * @param filename Nazov suboru
 * @return Precitany subor vo forme stringu
 */
QString Files::readStyleSheet(QString filename)
{
    QString styleSheet = "";
    QFile file("../files/"+filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return "";  // nacitanie suboru zlyhalo
    }
    QTextStream input(&file);  // ulozenie suboru


    // citanie zo suboru
    while (!input.atEnd()) {
        QString line = input.readLine();
        styleSheet += line;
    }

    file.close();
    return styleSheet;
}

/**
 * @brief Files::~Files
 */
Files::~Files()
{

}
