#**Implementačná dokumentácia IPP 2019/2020 časť II (interpret)**
###### Meno a priezvisko: Matej Otćenáš
###### Login: xotcen01

## Moduly

Program sa skladá z troch modulov: <br />
`interpret.py` => obsahuje funkciu `main()` a prebieha v ňom spracovanie <br />
argumentov a volanie vedľajších modulov <br />
`lex_synt_analysis.py` => je v ňom spracovaná lexikálna a syntaktická <br />
analýza kódu IPPCode20 <br />
`semantic_analysis.py` => pri korektnom spracovaní predošlou analýzou <br />
je tento modul použitý pre sémantickú analýzu a výslednú interpretáciu <br />
kódu IPPCode20

### Popis implementácie modulu *lex_synt_analysis.py*
Modul lexikálnej a syntaktickej analýzu simuluje prácu *parseru*. <br />
Využíva najmä *naimportovaný* modul Pythonu3 `xml.etree.ElementTree` <br />
pre jednoduchšie spracovanie vstupu vo formáte XML notácie <br />
(viac link: [ElementTree](https://docs.python.org/3/library/xml.etree.elementtree.html)). <br />
Postup pri spracovaní vstupu sa dá neformálne prirovnať k tzv. <br />
~~lúpaniu cibule~~, kde budeme lexikálne a syntakticky skúmať najskôr <br />
*koreň(angl. root)* a postupne jeho *potomkov(angl. child)*, a potom <br />
potomkov ich potomkov. Tento postup je zaobstaraný pomocou metód <br />
`parse_root()`, `parse_root_subelements()`, `parse_subelement_subelements()`. <br />
Každá z týchto metód kontroluje syntaktickú a lexikálnu korektnosť <br />
jednotlivých atribútov a elementov. <br />
Modul disponuje metódou `xml_parser()`, ktorú po vytvorení objektu <br />
stačí zavolať a vykoná sa potrebná analýza. <br />

### Popis implementácie modulu *semantic_analysis.py*

Tento modul similuje sémantickú analýzu a výslednú interpretáciu XML <br />
notácie IPPCode20. <br />
Metódy, ktoré su naimplementované v triede `SemanticAnalysis` operujú <br />
predovšetkým s Python3 slovníkmi (angl. dictionaries), ktoré pracujú <br />
na princípe mapy(kľúč => hodnota) alebo sú využívané iné dátove <br />
štruktúry, ako napríklad zoznam. Trieda je tvorená viacerými metódami. <br />
Hlavnou z nich je metóda `semantic_checker()`, v ktorej sa analyzujú <br />
všetky inštrukcie IPPCode20. Táto metóda volá svoje vlastné triedne, <br />
častokrát statické, metódy pre jednoduchšiu a prehľadnejšiu štruktúru <br />
kódu. Telo metódy sémantickej kontroly je zaobalené do dvoch cyklov. <br />
Vonkajší cyklus iteruje cez slovník naśledovne: <br />
`for instruction in self.data`. <br />
Tento slovník je zložený z ďalších dvoch vnorených slovníkov, a je to <br />
štruktúra, ktorá obsahuje dáta o celom "*zparsovanom*" XML formáte <br />
IPPCode20 kóde. Premenná `data` je globálna premenná triedy <br />
`SemanticAnalysis` a je zadefinovaná pomocou metódy `collect_data()`. <br />
Druhým cyklom je cyklus `while`, ktorý je použitý pre všetky skokové <br />
inštrukcie aby sa pri ich výskyte dalo vyhnúť rekurzií a iterovať <br />
cez dané inštrukcie odznovu. <br />
Modul obsahuje aj metódu `run_analysis()`, ktorá pri zavolaní objektom <br />
zaobstará spustenie celej analýzy bez nutnosti volania iných metód.