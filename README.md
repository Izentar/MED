# AMHE

Użyte technologie
* C++
* Python

## Zakres projektu
Wykonana zostanie implementacja algorytmu PrefixSpan w języku C++, opisanego w dokumencie :
Mining Sequential Patterns by Pattern-Growth: The PrefixSpan Approach
Jian Pei, Member, IEEE Computer Society, Jiawei Han, Senior Member, IEEE,
Behzad Mortazavi-Asl, Jianyong Wang, Helen Pinto, Qiming Chen,
Umeshwar Dayal, Member, IEEE Computer Society, and Mei-Chun Hsu

Ponadto zostanie do niego dodana modyfikacja umożliwiająca wykonanie go wielowątkowo. Dzięki temu kosztem dodatkowej pamięci, obliczenia powinny zostać w teorii przyśpieszone. 

Algorytm w obydwu wersjach zostanie zbadany pod kątem wydajności czasowej oraz pamięciowej. Zostanie on również porównany z implementacją znajdującą się na stronie 
https://github.com/zhhailon/prefixspan

Język python zostanie użyty do stworzenia prostych skryptów, między innymi do porówania dwóch plików z transakcjami.

## Implementacja
Idea algorytmu PrefixSpan polega na rekurencyjnym dodawaniu prefiksu. Prefiks jest definiowany jako najbardziej lewa część wyrażenia.
W każdej rekursji rozpatrywana jest część bazy, która zawiera rozpatrywany prefix (na samym początku prefix ma zerową długość). Dla każdej transakcji dla każdej rekursji, zostaje zapamiętana pozycja, od której prefix się kończy. Algorytm sprawdza, czy dla danej rozpatrywanej pozycji występuje ona na prawo od prefiksa w danej rozpatrywanej transakcji. Jeżeli tak, to zostaje ona dodana do nowej bazy transakcji wraz z pozycją końca nowego prefiksa. 
W celu optymalizacji pamięci, przechowuje się w projekcji bazy wskazania na dane transakcje.

### Szczegóły implemntacyjne
Na początku ze zbioru transakcji zostaną wyodrębnione tylko te pozycje, które występują conajmniej raz w całym zbiorze. Dzięki temu stwarza się słownik występujących pozycji w bazie. Jest to krok optymalizacyjny, gdyż zmniejsza on liczbę iteracji w dalszej części algorytmu. Ponadto algorytm zostaje uodporniony na dane, w których nie występuje ciągłość w zakresie pozycji.

Następnie przechodząc przez kolejne pozycje, które zostały znalezione, dla każdego z nich w dowolnej kolejności, przechodzi się przez wszystkie transakcje oraz sprawdza się, czy gdzieś w danej, rozpatrywanej transakcji znajduje się dana pozycja. Transakcja jest rozpatrywana względem tej pozycji od lewej strony, dla której to występuje badany prefix. Jeżeli znaleziono w transakcji daną pozycję względem danego prefiksa, to dodaje się ją do nowej bazy transakcji.

Algorytm działa rekurencyjnie do momentu, gdy znaleziono prefix o maksymalnej zdefiniowanej długości lub liczba transakcji w bazie jest zbyt mała. Obydwa parametry są ustawiane przez użytkownika.

Program będzie zapisywał logi odnośnie zużycia pamięci przez cały proces oraz zapamiętywał czasy wejścia oraz wyjścia z rekursji przez dany wątek.
 
Implementacja wielowątkowa bazuje na założeniu rozdawania prac do poszczególnych wątków. Jeżeli wątek zakończył wykonywaną pracę, to pobierze z kolejki następne zadanie. Algorytm został podzielony na 2 możliwe prace. Pierwsza z nich tworzy zbiór wszystkich możliwych pozycji w danej bazie, a następnie iterując po tym zbiorze dodaje prace do kolejki.
Druga część algorytmu iteruje po wszystkich transakcjach z danej bazy, aby dla danego prefiksa znaleźć wszystkie transakcje pasujące do niego. Następnie wywołuje ona pierwszą część algorytmu.
W celu optymalizacji nadano pierwszeństwo tym pracom, które są najgłębiej w rekursji, ponieważ one mają największą szansę na to, że zostaną zakończone, przez co zostanie zwolniona pamięć.

Trzeba zaznaczyć, że dane pomiędzy obydwoma częściami algorytmu są przekazywane za pomocą kopii z wyjątkiem danych dotyczących bazy (database).

### Pseudokod
database = load_data()
emptyPrefix = []
first(database, emptyPrefix)

def first(database, prefix):
    itemSet = getItemSet()

    for item in itemSet:
        if(mthread):
            add_job(second, database, prefix, item)
        else:
            second(database, prefix, item)


def second(database, prefix, item):
    newDatabase
    for trans in database:
        check, newPos = exist(trans, prefix, item)
        if(check):
            newDatabase.add(trans, newPos)
    
    prefix += item
    if(mthread):
        add_job(first, newDatabase, prefix)
    else:
        first(database, prefix)

## Założenia
Dostarczone dane są w formie tabelarycznej, w której to każda linia reprezentuje jedną transakcję. Pozycje w transakcji są oddzielone spacją.


## Testy oraz eksperymenty
Zostaną zbadane takie kwestie programu w wersjach jedno oraz wielowątkowym jak:
* czas wykonania,
* zajętość pamięciowa całego programu,
* wpływ parametrów programu względem różnych zbiorów danych na zużycie pamięci oraz czasu.

Każdy ekperyment będzie powtórzony conajmniej 3 razy w celu uzyskania średniej z wyników. Ilość powtórzeń będzie uzależniona od długości trwania pojedynczego eksperymentu.

Zbiory danych, które zostaną użyte podczas badań oraz testów znajdują się na stronie http://fimi.uantwerpen.be/data/ Planuje się wykorzytanie plików:
* mushroom.dat
* accidents.dat
* pumsb_star.dat
* webdocs.dat - waga 0.5 GB