# AMHE

Użyte technologie
* C++
* Python

## Zakres projektu
Wykonana zostanie implementacja algorytmu PrefixSpan, znajdującego się na stronie

## Implementacja
Idea algorytmu PrefixSpan polega na eliminacji tych transakcji, które nie spełniają w danej iteracji kryterium 

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

Ponadto zaimplementowany program zostanie porównany z gotowym rozwiązaniem z biblioteki z Python względem wcześniej wymienionych kwestii oraz:
* wyników dla różnych wartości parametrów wejściowych,
* wyników dla różnych zbiorów danych, w szczególności względem dużych oraz małych zbiorów danych.

Pozwoli to na proste sprawdzenie poprawności rozwiązania, jak i jego efektywności względem róznej wielkości zbiorów danych.
Dodatkowo zaimplementowany algorytm PrefixSpan zostanie porównany z innymi algorytmami z grupy algorytmów do odkrywania częstych wzorców sekwencyjnych, takich jak:
* FreeSpan
* GSP


Zbiory danych, które zostaną użyte podczas badań oraz testów, to:
* 