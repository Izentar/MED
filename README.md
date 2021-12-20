# AMHE

Użyte technologie
* C++
* Python

## Zakres projektu
Wykonana zostanie implementacja algorytmu PrefixSpan, opisanego w dokumencie :
Mining Sequential Patterns by Pattern-Growth: The PrefixSpan Approach
Jian Pei, Member, IEEE Computer Society, Jiawei Han, Senior Member, IEEE,
Behzad Mortazavi-Asl, Jianyong Wang, Helen Pinto, Qiming Chen,
Umeshwar Dayal, Member, IEEE Computer Society, and Mei-Chun Hsu

Ponadto zostanie do niego dodana modyfikacja umożliwiająca wykonanie go wielowątkowo. Dzięki temu kosztem dodatkowej pamięci, obliczenia powinny zostać w teorii przyśpieszone. 

Algorytm w obydwu wersjach zostanie zbadany oraz zostanie porównany z innym algorytmem odkrywania częstych wzorców sekwencyjnych.

## Implementacja
Idea algorytmu PrefixSpan polega na rekurencyjnym dodawaniu prefiksu. Prefiks jest definiowany jako najbardziej lewa część wyrażenia.
W każdej rekursji rozpatrywana jest część bazy, która to zawiera rozpatrywany prefix (na samym początku prefix ma zerową długość). Dla każdej transakcji dla każdej rekursji, zostaje zapamiętana pozycja, od której prefix się kończy. Algorytm sprawdza, czy dla danej rozpatrywanej pozycji występuje ona na prawo od prefiksa w danej rozpatrywanej transakcji. Jeżeli tak, to zostaje ona dodana do nowej bazy transakcji wraz z zakutalizowaną pozycją.

### Szczegóły implemntacyjne
Na początku ze zbioru transakcji zostaną wyodrębnione tylko te pozycje, które występują conajmniej raz w całym zbiorze. Dzięki temu stwarza się słownik występujących pozycji w bazie. Jest to krok optymalizacyjny, gdyż zmniejsza on liczbę iteracji w dalszej części algorytmu. Ponadto algorytm zostaje uodporniony na dane, w których nie występuje ciągłość w zakresie pozycji.

Następnie przechodząc przez kolejne pozycje, które zostały znalezione, dla każdego z nich w dowolnej kolejności, przechodzi się przez wszystkie transakcje oraz sprawdza się, czy gdzieś w danej, rozpatrywanej transakcji znajduje się dana pozycja. Transakcja jest rozpatrywana względem tej pozycji od lewej strony, dla której to występuje badany prefix. Jeżeli znaleziono w transakcji daną pozycję względem danego prefiksa, to dodaje się ją do nowej bazy transakcji.

Algorytm działa rekurencyjnie do momentu, gdy znaleziono prefix o maksymalnej zdefiniowanej długości lub liczba transakcji w bazie jest zbyt mała. Obydwa parametry są ustawiane przez użytkownika.

Program będzie zapisywał logi odnośnie zużycia pamięci przez cały proces oraz zapamiętywał czasy wejścia oraz wyjścia z rekursji przez dany wątek.

Podczas działania algorytmu zliczana będzie ilość alokacji oraz dealokacji, co pozwoli na zachowanie statystyk dotyczących alokacji pamięci. Zliczane będą ilość alokacji, dealokacji oraz największego zużycia pamięci sterty programu.  
Pseudokod algorytmu PrefixSpan wygląda następująco, z założeniem, że dane zostały już wczytane całkowicie do pamięci.

maxPatternSize = <ustala użytkownik> // maksymalna długość szukanego wzorca w danych

for()
if()




Algorytm PrefixSpan zostanie zaimplementowany w formie iteracyjnej, w której to w każdej kolejnej iteracji zostaną powtórzone 
Na początku występuje faza wczytania danych. Następnie w formie rekurencyjnej zostają wywołane:
* zliczanie poszczególnych wartości,
* 

Z uwagi na dużą możliwą zajętość pamięciową oraz wywołania rekurencyjne zmienne, które już nie są potrzebne w programie, będą dealokowane ze sterty programu.

## Testy oraz eksperymenty
Zostaną zbadane takie kwestie programu jak:
* czas wykonania,
* zajętość pamięciowa sterty programu.

Każdy ekperyment będzie powtórzony conajmniej 3 razy w celu uzyskania średniej z wyników.

Ponadto zaimplementowany program zostanie porównany z gotowym rozwiązaniem z biblioteki z Python względem wcześniej wymienionych kwestii oraz:
* wyników dla różnych wartości parametrów wejściowych,
* wyników dla różnych zbiorów danych, w szczególności względem dużych oraz małych zbiorów danych.

Pozwoli to na proste sprawdzenie poprawności rozwiązania, jak i jego efektywności względem róznej wielkości zbiorów danych.
Dodatkowo zaimplementowany algorytm PrefixSpan zostanie porównany z innymi algorytmami z grupy algorytmów do odkrywania częstych wzorców sekwencyjnych, takich jak:
* GSP


Zbiory danych, które zostaną użyte podczas badań oraz testów, to:
* 