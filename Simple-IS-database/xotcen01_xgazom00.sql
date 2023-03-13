-- File: database.sql
-- Authors: Mário Gažo(xgazom00), Matej Otčenáš(xotcen01)
-- Zadanie do IDS 2019/20


-- Reset databazy
DROP TABLE Prihlaseny;
DROP TABLE Lekcia;
DROP TABLE Trener;
DROP TABLE Osoba;
DROP TABLE Miestnost;
DROP TABLE Kurz;

DROP MATERIALIZED VIEW mat_pohlad;

-- Vytvorenie blokov schémy databáze
CREATE TABLE Kurz
(
    Nazov       CHAR(50) NOT NULL PRIMARY KEY,
    Typ         CHAR(50),
    Obtiaznost  INT,
    Trvanie     INT,
    Kapacita    INT,
    Popis       CHAR(100)
);

CREATE TABLE Miestnost
(
    Nazov       CHAR(50) NOT NULL PRIMARY KEY,
    Umiestnenie CHAR(50),
    Kapacita    INT
);

CREATE TABLE Osoba
(
    Rodne_cislo     INT      NOT NULL PRIMARY KEY,
    Meno            CHAR(20) NOT NULL,
    Priezvisko      CHAR(20) NOT NULL,
    Tel_cislo       CHAR(15),
    Ulica           CHAR(20) NOT NULL,
    Popisne_cislo   INT      NOT NULL,
    Mesto           CHAR(20) NOT NULL,
    PSC             INT
);

CREATE TABLE Trener
(
    Rodne_cislo     INT PRIMARY KEY NOT NULL,
    FOREIGN KEY     (Rodne_cislo) REFERENCES Osoba(Rodne_cislo),
    Zaciatok        CHAR(5),
    Koniec          CHAR(5)
);

CREATE TABLE Lekcia
(
    Poradie     INT NOT NULL,
    Miesta      INT,
    Den         CHAR(10),
    Zahajenie   CHAR(5),
    Ukoncenie   CHAR(5),
    Cena        FLOAT,
    Miestnost   CHAR(50),
    Kurz        CHAR(50) NOT NULL ,
    Rodne_cislo INT,
    CONSTRAINT Poradie_kurz PRIMARY KEY(Poradie, Kurz),

    CONSTRAINT Miestnost_fk
        FOREIGN KEY (Miestnost) REFERENCES Miestnost(Nazov),

    CONSTRAINT Kurz_fk
        FOREIGN KEY (Kurz) REFERENCES Kurz(Nazov),

    CONSTRAINT  Rodne_cislo_fk
        FOREIGN KEY (Rodne_cislo) REFERENCES Trener(Rodne_cislo)
);

CREATE TABLE Prihlaseny
(
    Poradie_lekcie INT,
    Nazov_kurzu CHAR(50),
    FOREIGN KEY     (Poradie_lekcie, Nazov_kurzu) REFERENCES Lekcia(Poradie, Kurz),
    Rodne_cislo INT,
    FOREIGN KEY     (Rodne_cislo) REFERENCES Osoba(Rodne_cislo)
);

-------------------------------------------------- Časť štvrtá ---------------------------------------------------------
-- TRIGGERY
-- Vždy v pondelok od 8-9 sa koná úvodná lekcia novo pridaného kurzu
CREATE TRIGGER Uvodna_lekcia
    AFTER INSERT ON Kurz
    FOR EACH ROW
BEGIN
    INSERT INTO Lekcia(Poradie, Miesta, Den, Zahajenie, Ukoncenie, Cena, Miestnost, Kurz)
    VALUES (0, 30, 'Pondelok', '8:00', '9:00', 0, 'Box', :NEW.Nazov);
END;

-- Po ukončení kurzu nie je nutné naďalej viesť záznam o jeho lekciách
CREATE TRIGGER Koniec_kurzu
    AFTER DELETE ON Kurz
    FOR EACH ROW
BEGIN
    DELETE FROM Lekcia WHERE Lekcia.Kurz = :OLD.Nazov;
END;

-- Vlozenie dat

-- Miestnost
INSERT INTO Miestnost(Nazov, Umiestnenie, Kapacita)
VALUES ('Box', 'A00', 30);

INSERT INTO Miestnost(Nazov, Umiestnenie, Kapacita)
VALUES ('Spinning', 'A01', 60);

INSERT INTO Miestnost(Nazov, Umiestnenie, Kapacita)
VALUES ('Bazen', 'B04', 40);

INSERT INTO Miestnost(Nazov, Umiestnenie, Kapacita)
VALUES ('Posilňovňa', 'A05', 100);

-- Kurz
INSERT INTO Kurz(Nazov, Typ, Obtiaznost, Trvanie, Kapacita, Popis)
VALUES ('Crossfit', 'Pokročilý/Hybrid', 4, 90, 20, 'Kurz pre pokročilých, zamerané na spodnú časť tela.');

INSERT INTO Kurz(Nazov, Typ, Obtiaznost, Trvanie, Kapacita, Popis)
VALUES ('Body_form', 'Začiatočník/Ženy', 2, 45, 40, 'Základný kurz pre ženy zameraný na precvičenie celého tela.');

INSERT INTO Kurz(Nazov, Typ, Obtiaznost, Trvanie, Kapacita, Popis)
VALUES ('Plávanie', 'Stredne_pokročilý/Muži', 3, 60, 30, 'Kurz pre stredne pokročilých plavcov.');

-- Osoba
INSERT INTO Osoba(Rodne_cislo, Meno, Priezvisko, Tel_cislo, Ulica, Popisne_cislo, Mesto, PSC)
VALUES (8904253333, 'Peter', 'Bezbohý', '+421949503611', 'Nekonečná', 42, 'Trnava', 92154);

INSERT INTO Osoba(Rodne_cislo, Meno, Priezvisko, Tel_cislo, Ulica, Popisne_cislo, Mesto, PSC)
VALUES (9804254444, 'Ignác', 'Bezruký', '+420901504411', 'Konečná', 24, 'Šurany', 99301);

INSERT INTO Osoba(Rodne_cislo, Meno, Priezvisko, Tel_cislo, Ulica, Popisne_cislo, Mesto, PSC)
VALUES (9904295555, 'Violeta', 'Beznohá', '+421967999976', 'Začiatočná', 420, 'Piešťany', 92101);

-- Vlozenie viacej osob koli vacsej variabilite
INSERT INTO Osoba(Rodne_cislo, Meno, Priezvisko, Tel_cislo, Ulica, Popisne_cislo, Mesto, PSC)
VALUES (7303245432, 'Charlie', 'Sheen', '+300458735298', 'Palms Spring', 420, 'Miami', 100000);

INSERT INTO Osoba(Rodne_cislo, Meno, Priezvisko, Tel_cislo, Ulica, Popisne_cislo, Mesto, PSC)
VALUES (5509306665, 'Arnold', 'Švarceneger', '+100432768467', 'Unknown', 300, 'Pezinok', 97893);

-- Trener
INSERT INTO Trener(Rodne_cislo, Zaciatok, Koniec)
VALUES ((SELECT Rodne_cislo FROM Osoba WHERE Osoba.Rodne_cislo=9904295555),'17:25', '19:00');

INSERT INTO Trener(Rodne_cislo, Zaciatok, Koniec)
VALUES ((SELECT Rodne_cislo FROM Osoba WHERE Osoba.Rodne_cislo=9804254444),'08:00', '20:00');

INSERT INTO Trener(Rodne_cislo, Zaciatok, Koniec)
VALUES ((SELECT Rodne_cislo FROM Osoba WHERE Osoba.Rodne_cislo=5509306665),'17:00', '17:30');

-- Lekcia
INSERT INTO Lekcia(Poradie, Miesta, Den, Zahajenie, Ukoncenie, Cena, Miestnost, Kurz, Rodne_cislo)
VALUES (3, 20, 'Pondelok', '12:00', '13:30', 17.50, 'Box','Crossfit', 9904295555);

INSERT INTO Lekcia(Poradie, Miesta, Den, Zahajenie, Ukoncenie, Cena, Miestnost, Kurz, Rodne_cislo)
VALUES  (1, 2, 'Streda', '12:45', '17:50', 23.99,'Bazen', 'Plávanie', 9804254444);

INSERT INTO Lekcia(Poradie, Miesta, Den, Zahajenie, Ukoncenie, Cena, Miestnost, Kurz, Rodne_cislo)
VALUES  (5, 8, 'Streda', '12:45', '17:50', 10, 'Spinning', 'Body_form', 9904295555);

-- Prihlaseny
INSERT INTO  Prihlaseny(Poradie_lekcie, Nazov_kurzu, Rodne_cislo)
VALUES (1, 'Plávanie', 5509306665);

INSERT INTO  Prihlaseny(Poradie_lekcie, Nazov_kurzu, Rodne_cislo)
VALUES (5, 'Body_form', 5509306665);

INSERT INTO  Prihlaseny(Poradie_lekcie, Nazov_kurzu, Rodne_cislo)
VALUES (1, 'Plávanie', 8904253333);

-------------------------------------------------- Časť tretia ---------------------------------------------------------

-- Vytvorenie dotazov SELECT

-- Informácie o lekcii ktorú vedie určitý tréner
SELECT
    Kurz, Miestnost, Poradie, Cena
FROM
    Lekcia
    NATURAL JOIN Trener
WHERE
    Rodne_cislo = 9904295555
OR
    Rodne_cislo = 5509306665
OR
    Rodne_cislo = 9804254444;

-- Informácie o kurze, ktorého piata lekcie sa odohráva v pondelok a stojí viac ako 10e
SELECT
    Nazov, Kapacita
FROM
    Miestnost
    NATURAL JOIN Lekcia
    NATURAL JOIN Prihlaseny
WHERE
    Cena > 10
AND
     Poradie_lekcie = 5
AND
    Den = 'Pondelok';

-- Informácie o trénerovi, ktorý vedie lekcie crossfitu s kapacitou miestnosti maximálne 60
SELECT DISTINCT
    Meno, Priezvisko, Tel_cislo, Mesto
FROM
    Osoba
    NATURAL JOIN Trener
    NATURAL JOIN Miestnost
    NATURAL JOIN Lekcia
WHERE
    Rodne_cislo = 9904295555
AND
    Kurz = 'Crossfit'
AND
    Kapacita < 60;

-- Vyhľadá názvy kurzov, ktorých trvanie je dlhšie ako 45 minút
SELECT
    Nazov, MAX(Trvanie) AS Najdlhsi_kurz
FROM
    Kurz
HAVING MAX(Trvanie) > 45

GROUP BY Nazov;

-- Vyhľadá názov a typ kurzu, ktorého priemerná obtiažnosť je menšia ako 3
SELECT
    Nazov, Typ, AVG(Obtiaznost) AS Priemerná_obtiaznost
FROM
    Kurz
HAVING AVG(Obtiaznost) < 3

GROUP BY Nazov, Typ;

-- Informácie o osobách ktoré sú prihlásené na lekcie
SELECT
    Meno, Priezvisko
FROM
    Osoba
WHERE EXISTS
    (
        SELECT
            Lekcia.Rodne_cislo
        FROM
            Lekcia
        WHERE
            Osoba.Rodne_cislo = Rodne_cislo
    );

-- Informácie o miestnostiach v ktorých sa odohrávajú lekcie
SELECT
    Nazov, Umiestnenie, Kapacita
FROM
    Miestnost
WHERE EXISTS
    (
        SELECT
            Lekcia.Miestnost
        FROM
            Lekcia
        WHERE
            Miestnost.Nazov = Miestnost
    );

-- Popis kurzu, ktoreho lekcia zacina 12:45 a jej trener zacina 17:25
SELECT
    Popis
FROM
    Kurz
WHERE
    Nazov
IN
    (
        SELECT
            Lekcia.Kurz
        FROM
            Lekcia
            INNER JOIN Trener
            ON Lekcia.Rodne_cislo = Trener.Rodne_cislo
        WHERE
            Trener.Zaciatok = '17:25'
            AND Lekcia.Zahajenie = '12:45'
    );

-- TRIGGER DEMO
DELETE FROM Kurz WHERE Nazov = 'Crossfit';



-- Procedury a funkcie

CREATE OR REPLACE PROCEDURE najdi_osobu
AS
    priezvisko Osoba.Priezvisko%type;
    ulica Osoba.Ulica%type;
    cislo_domu Osoba.Popisne_cislo%type;
    mesto Osoba.Mesto%type;
    CURSOR cur is SELECT Priezvisko, Ulica, Popisne_cislo, Mesto FROM Osoba;
BEGIN
    DBMS_OUTPUT.PUT_LINE('Informácie o bývaní osôb, ktorých priezvisko je Beznohá.');
    IF cur %ISOPEN THEN
        CLOSE cur ;
    END IF;
    OPEN cur;

    LOOP
         FETCH cur INTO priezvisko, ulica, cislo_domu, mesto;
         EXIT WHEN cur%notfound;
         IF priezvisko = 'Beznohá' THEN
             DBMS_OUTPUT.put_line('Osoba '|| priezvisko ||'býva v meste '|| mesto ||'na ulici '|| ulica ||' '||cislo_domu);
         END IF;
    END LOOP;
    CLOSE cur;

END;


CREATE OR REPLACE PROCEDURE celkova_kapacita
AS
    CURSOR cur IS SELECT * FROM Miestnost;
    curs_var cur%ROWTYPE;
    celkova_kapacita NUMBER;
    percenta NUMBER;
BEGIN
    IF cur %ISOPEN THEN
        CLOSE cur ;
    END IF;
    OPEN cur;

    celkova_kapacita := ziskaj_celkovy_pocet();

    LOOP
        FETCH cur INTO curs_var;
        EXIT WHEN cur%NOTFOUND;
        IF curs_var.Kapacita < 60 THEN
            percenta := vypocitaj_percent_podiel(celkova_kapacita, curs_var.Kapacita);
            percenta := ROUND(percenta);
            DBMS_OUTPUT.put_line('Miestnosť s kapacitou pod 60 ľudí je ' ||curs_var.Nazov|| 's kapacitou ' ||curs_var.Kapacita);
            DBMS_OUTPUT.put_line('Miestnosť tvorí ' ||percenta|| '% z celkovej kapacity.');
        END IF;
    END LOOP;
    DBMS_OUTPUT.put_line('Celková kapacita v miestnostiach je ' ||celkova_kapacita);

    CLOSE cur;
END;


CREATE OR REPLACE FUNCTION vypocitaj_percent_podiel(celkovy_pocet IN NUMBER, jedna_miestnost IN number)
RETURN NUMBER
AS
    vysledok NUMBER;
BEGIN
    vysledok := (jedna_miestnost * 100) / celkovy_pocet;
    RETURN vysledok;

    EXCEPTION WHEN zero_divide THEN
        RETURN 0;
END;

CREATE OR REPLACE FUNCTION ziskaj_celkovy_pocet
RETURN NUMBER
AS
    CURSOR cur IS SELECT Kapacita FROM Miestnost;
    celkovy_pocet NUMBER;
    iter Miestnost.Kapacita%TYPE;
BEGIN
    IF cur %ISOPEN THEN
        CLOSE cur ;
    END IF;
    OPEN cur;

    celkovy_pocet := 0;

    LOOP
        FETCH cur INTO iter;
        EXIT WHEN cur%NOTFOUND;
        celkovy_pocet := celkovy_pocet + iter;
    END LOOP;

    CLOSE cur;

    RETURN celkovy_pocet;
END;


CALL najdi_osobu();
CALL celkova_kapacita();


-- Explain plan

EXPLAIN PLAN FOR
SELECT
    Nazov, Typ, AVG(Obtiaznost) AS priemerna_obtiaznost
FROM
    Kurz
HAVING AVG(Obtiaznost) < 3

GROUP BY Nazov, Typ;

SELECT * FROM TABLE(dbms_xplan.display);


EXPLAIN PLAN FOR
SELECT
    Rodne_cislo, Poradie, Kurz, Den, AVG(Cena) AS priemerna_cena_lekcii
FROM
    Lekcia
    NATURAL JOIN Prihlaseny
HAVING AVG(Cena) < 10

GROUP BY
    Rodne_cislo, Poradie, Kurz, Den;

SELECT * FROM TABLE(dbms_xplan.display);

-- Explain plan s pouzitim indexu

CREATE INDEX idx ON Prihlaseny(Rodne_cislo);

EXPLAIN PLAN FOR
SELECT
    Rodne_cislo, Poradie, Kurz, Den, AVG(Cena) AS priemerna_cena_lekcii
FROM
    Lekcia
    NATURAL JOIN Prihlaseny
HAVING AVG(Cena) < 10

GROUP BY
    Rodne_cislo, Poradie, Kurz, Den;

SELECT * FROM TABLE(dbms_xplan.display);

DROP INDEX idx;


-- Pridelenie prav

GRANT ALL ON Osoba TO XGAZOM00;
GRANT ALL ON Kurz TO XGAZOM00;
GRANT ALL ON Lekcia TO XGAZOM00;
GRANT ALL ON Miestnost TO XGAZOM00;
GRANT ALL ON Trener TO XGAZOM00;
GRANT ALL ON Prihlaseny TO XGAZOM00;

GRANT EXECUTE ON najdi_osobu TO XGAZOM00;
GRANT EXECUTE ON celkova_kapacita TO XGAZOM00;
GRANT EXECUTE ON ziskaj_celkovy_pocet TO XGAZOM00;
GRANT EXECUTE ON vypocitaj_percent_podiel TO XGAZOM00;


-- Vytvorenie materializovaneho pohladu

CREATE MATERIALIZED VIEW mat_pohlad

NOLOGGING
CACHE
BUILD IMMEDIATE
REFRESH ON COMMIT

AS
    SELECT Meno, Priezvisko FROM Osoba o JOIN Trener t ON o.Rodne_cislo = t.Rodne_cislo;

GRANT ALL ON mat_pohlad TO XGAZOM00;



/*  Koniec súboru xotcen01_xgazom00.sql */