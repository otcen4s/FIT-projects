
# Štruktúra adresáru a implementácia
Projekt obsahuje nasledujúce súbory na spustenie programu v prológu:  **main**, ktorý zaistuje spustenie a beh programu pomocou vedlajších imporotvaných modulov. Program načíta štandardný vstup a zparsuje tento vstup pomocou modulov **utils** a **read** . Následne je pomocou modulov **rotations** a **search** zaistená rotácia kocky do jednotlivých polôh a hľadanie cesty v prehľadávacom priestore na nájdenie riešenia.  Na záver je pomocou modulu **print** zaistený výpis pôvodnej konfigurácie kocky a jednotlivých medzikrokov (rotovaných konfigurácií kocky) až po finálne riešenie.  Spôsob prehľadávania priestoru je realizovaný pomocou algoritmu IDS (Iterative Depth Search), ktorý bol zvolený po pôvodnom otestovaní alogritmu DFS (Depth First Search), ktorý pri tažšej vstupnej konfigurácií nebol schopný nájsť riešenie a samotný program skončil neúspechom. Toto je obecný problém hlbokého zanorenia v prehľadávacom priestore pomocou DFS. Preto bolo zvolené prehľadávanie pomocou iteratívneho prehľadávania do hĺbky, kde sa postupne zvyšuje limit zanorenia po neúspešnej fáze prehľadávanie v určitej hĺbke. Tento limit sa iteratívne zvyšuje o hodnotu 1 (hĺbka prehľadávania). V každom takomto kroku sa prehľadávajú jednotlivé uzly, ktoré môžeme interpretovať ako konfigurácie kocky a testuje sa, či daná konfigurácia je hľadanou (vyriešená rubíková kocka).


## Testy
Testy prebehli na rôznych valídnych vstupných konfiguráciach kocky a tabuľka uvádza čas potrebný na vyriešenie, počet ťahov, ktorými bola kocka vyriešená a názov súboru testu, uložený v adresáry **test**.  Korešpondujúce výstupy sú uložené v rovnakom adresáry s koncovkou ***.out***, kde prvý výstup je počiatočná konfigurácia.


```
| Čas [s] | #Ťahov |    Názov   | 
| ------- | ------ |------------|
|  0,01   |    1   | example.in |
|  0,03   |    2   |  test3.in  |
|  0,06   |    3   |  test5.in  |
|  0,5    |    5   |  test8.in  |
|  14,9   |    6   |  test6.in  |
|  30,39  |    6   |  test9.in  |
|  55,8   |    7   |  test10.in |
|  179,98 |    7   |  test7.in  |
|  6311,68|    8   |  test11.in |
```


## Záver
Z výslednej tabuľky testov je vidieť, že s náročnosťou počiatočnej konfigurácie rapídne stúpa časová náročnosť algoritmu IDS. Napriek tomu je tento algoritmus schopný riešiť rôzne konfigurácie kocky, ktoré môžu byť obtiažne. Náročnejšie experimenty, ktoré by požadovali viac ťahov, realizované neboli z časového hľadiska. Napriek tomu sa dá predpokladať, že algoritmus je schopný vyriešiť rôzne zadané konfigurácie rubíkovej kocky.



