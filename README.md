# Labirnty 3D
## Implementacja:
### Cel:
Poruszamy się jako kula i z dolnego rogu chcemy dojść do najdalej oddalonego punktu.

### Dodana implemntacja:
- Dodane obracanie się przeszkód w losowym kierunku.
- Po skończeniu gry, czyli dotarciu do specjalnego ośmiościana, który jest inny od wszystkich, kolor kaltki się zmienia
- Kolor tła szachownicy zmienia się wraz z odległością gracza do celu.

### Budowanie 
Aby zagrać trzeba zbudować projekt za pomocą cmake. Wystarczy wywołać w głównym folderze w którym jest gra `cmake -S . -B build && cmake --build build`. Po zbudowaniu w odpalamy gre `./build/main [seed] [N]` gdzie seed to ziarno do kąta obrotu przeszkód, a N to ilość przeszkód z graczem na jednym wmiarze czyli razem jest wszystkich obiektó N^3, można też nie podać argumentów i wtedy dostanie sie standartową plansze albo tylko seed.