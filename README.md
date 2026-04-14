# Strain Gauge Sensor

Pomiar ciśnienia w butlach kompozytowych na podstawie analizy odkształceń zbiornika.  

Projekt magisterski **Konrada Kobielus**.

Celem projektu było stworzenie i zbadanie czujnika ciśnienia w butlach kompozytowych metodą nieinwazyjną. Rozwiązaniem problemu było wykorzystanie czujników odkształceń – tensometrów. Zbadane odkształcenia zestawia się z ciśnieniem referencyjnym zmierzonym manometrem. Na tej podstawie wyznacza się charakterystykę, która pozwala jednoznacznie określić ciśnienie panujące w butli.

---

## 1. Sprzęt i PCB

W ramach projektu została zaprojektowana i wykonana płytka PCB z mostkiem Wheatstone’a, do którego przez multiplekser podłączane są tensometry. Wyniki z układu odczytywane są za pomocą jednego z trzech przygotowanych interfejsów (Tab. 1).

![alt text](attachements/pcb.png)

Komunikacja z płytką odbywa się poprzez interfejs **USART** z prędkością 115200 baud. Użytkownik przesyła komendy w formacie:

```c
cmd uint8_t command_id uint8_t param_1 ... uint8_t param_6
```

Kolejne parametry są interpretowane indywidualnie przez każdą komendę. Dla komend pomiarowych parametry oznaczają wartości opisane w Tab. 2. 

Uwaga! Wyjścia multipleksera indeksowane są od 1 do 8. Dla gałezi bridge out 1 dostępne są dwa multipleksery a zatem 16 połączeń.

Rezultaty wypisywane są  przez interfejs USART.

---

## 2. Analiza danych

Zebrane dane należy umieścić w folderze `logs` jako plik `dowolna_nazwa.txt`. Następnie w folderze `python` uruchomić skrypt:

```bash
python parse_to_csv.py
```

Efektem działania skryptu będzie:  

- Plik `.csv` w folderze `csv`, przedstawiające dane w pliku typu excel

![alt text](attachements/csv.png)
- Plik `.html` w folderze `charts`, przedstawiający dane na wykresie

![alt text](attachements/chart.png)
---

## 3. Propozycje dla nowego PCB

- Połączyć dwie linie sygnałowe (**DOUT**, **SCK**) dla układów HX711.
- Rezystory i zworki mogą na siebie nachodzić, aby ograniczyć zajmowaną przestrzeń PCB.
- Część złączek powinna być podpięta do kilku multiplekserów, aby ułatwić przełączanie.
- Dodać diodę zabezpieczającą przed odwrotną polaryzacją napięcia wejściowego.
- Poprawić footprint złącza DC – obecnie **plus zwarty z minusem (prawy dolny pin)**.
- Można zwiększyć odstępy między konektorami tensometrów.
- Można podłączyć złącza **B** układów HX711 do wolnych pinów multipleksera interfejsów.
- Obecna konfiguracja ADC nie pozwala na pomiary różnicowe – błędna konfiguracja w przełącznicy ADC. Można wykorzystać oba kanały, ponieważ jest dużo wolnego miejsca w multiplekserze interfejsów.
- Prawdopodobnie potrzebny będzie mikrokontroler z większą liczbą pinów.
- Dodać układ **soft start** dla **superkondensatora (SUPER CAP)**.
- Dodać **testpointy** na liniach podłączonych do interfejsów.
- Opcjonalnie dodać czwarte mocowanie na śrubę.
- Dodać interfejs do odczytywania referencyjnego ciśnienia z manometru.
- Wstępna przetwornica napięcia wejściowego.
- Zewnętrzne układy **op-amp**.
- Zmienić złacze zasilające na standardowe wymiary


## 4. Typy interfejsów

| ID  | Interfejs |
| --- | --------- |
| 0   | HX711     |
| 1   | ADC_OPAMP |
| 2   | ADC       |

tab 1.

---

## 5. Parametry komend

| Parametr      |
|---------------|
| INTERFACE     |
| BRIDGE_1      |
| BRIDGE_2      |
| BRIDGE_3      |
| BRIDGE_4      |
| EXTRA_PARAM   |

tab 2.

---

## 6. Lista komend

| ID  | Komenda                           |
| --- | --------------------------------- |
| 0   | SCHEDULE_MEASUREMENT              |
| 1   | EXECUTE_MEASUREMENTS              |
| 2   | SINGLE_MEASUREMENT                |
| 3   | REMOVE_MEASUREMENT                |
| 4   | REMOVE_SCHEDULED_MEASUREMENTS     |
| 5   | SERIALIZE_COMMANDS                |
| 6   | SAVE_COMMAND_TO_FLASH *(private)* |
| 7   | RESTORE_SERIALIZED_COMMANDS       |
| 8   | ERASE_FLASH                       |
| 9   | PRINT_SCHEDULED_COMMANDS          |

tab 3. 

## 7. Rezystory dla poszczególnych układów IC

### IC 1

| Wyjście | Parametr | Rezystory | Wartość | pass/fail |
| ------- | -------- | --------- | ------- | --------- |
| Y0      | 1        | R16       | 402R    | T         |
| Y1      | 2        | R15       | 1R      | T         |
| Y2      | 3        | R13 + R14 | 8K      | T         |
| Y3      | 4        | R17 + R18 | 804R    | T         |
| Y4      | 5        | R10 + R12 | 20R     | T         |
| Y5      | 6        | R8  + R9  | 2K      | T         |
| Y6      | 7        | R11       | 10R     | T         |
| Y7      | 8        | R7        | 5R      | T         |

### IC 3

| Wyjście | Parametr | Rezystory | Wartość    | pass/fail |
| ------- | -------- | --------- | ---------- | --------- |
| Y0      | 1        | R21       | 120R       | T         |
| Y1      |          |           | wolne pady |           |
| Y2      |          |           | wolne pady |           |
| Y3      | 4        | R22       | 348R       | T         |
| Y4      | 5        |           |            | T         |
| Y5      | 6        |           |            | T         |
| Y6      | 7        |           |            | T         |
| Y7      | 8        |           |            | T         |

### IC 4

| Wyjście | Parametr | Rezystory | Wartość    | pass/fail |
| ------- | -------- | --------- | ---------- | --------- |
| Y0      | 1        | R29       | 120R       | T         |
| Y1      | 2        |           | wolne pady |           |
| Y2      | 3        |           | wolne pady |           |
| Y3      | 4        | R30       | 348R       | T         |
| Y4      | 5        |           |            | T         |
| Y5      | 6        |           |            | T         |
| Y6      | 7        |           |            | T         |
| Y7      | 8        |           |            | T         |

### IC 7

| Wyjście | Parametr | Rezystory                      | Wartość | pass/fail |
| ------- | -------- | ------------------------------ | ------- | --------- |
| Y0      | 9        | R31 + S9                       | 120R    | T         |
| Y1      | 10       | T1 - naklejony na pustej butli | 120R    | T         |
| Y2      | 11       | T2                             | 120R    | T         |
| Y3      | 12       | R32 + S10                      | 348R    | T         |
| Y4      | 13       | T4                             | 120R    | T         |
| Y5      | 14       | T5                             | 120R    | T         |
| Y6      | 15       | T8                             | 350R    | T         |
| Y6      | 16       |                                | wolne   | T         |


### IC 8

| Wyjście | Parametr | Rezystory | Wartość | pass/fail |
| ------- | -------- | --------- | ------- | --------- |
| Y0      | 1        |           |         | T         |
| Y1      | 2        |           |         | T         |
| Y2      | 3        |           |         | T         |
| Y3      | 4        |           |         | T         |
| Y4      | 5        |           |         | T         |
| Y5      | 6        |           |         | T         |
| Y6      | 7        |           |         | T         |
| Y7      | 8        |           |         | T         |


### IC 9

| Wyjście | Parametr | Rezystory | Wartość | pass/fail |
| ------- | -------- | --------- | ------- | --------- |
| Y0      | 1        | R35 + S11 | 120R    | T         |
| Y1      |          | R39 + S13 | 10K R   |           |
| Y2      | 3        | T3        | 120R    | T         |
| Y3      | 4        | R36 + S12 | 348R    | T         |
| Y4      | 5        | T6        | 120R    | T         |
| Y5      | 6        |           |         | T         |
| Y6      | 7        | T7        | 120R    | T         |
| Y7      | 8        |           |         | T         |

