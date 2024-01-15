# Interpret pseudocod
Acest program vă permite să rulați pseudocod direct pe calculatorul dvs.

Puteți descărca codul sursă al interpretorului folosind următoarea comandă:
```
git clone https://github.com/vitelariu/pseudocod
```

Aveți nevoie doar de [CMake](https://cmake.org/) și de Make, precum și de un
compilator de C++. Verificați instrucțiunile specifice platformei dvs. pentru a
instala aceste dependințe.

### Unix
Pentru a compila interpretorul, navigați în directorul proiectului și rulați:

```
./build.sh
```
Acestea sunt opțiunile disponibile în script (`./build.sh -h`):
```
Usage: ./build.sh [options]
Options:
  -c, --clean     Clean the build directory before building
  -v, --verbose   Show additional output
  -q, --quiet     Suppress all output
  -h, --help      Show this help message and exit
Example:
  ./build.sh --clean --verbose
```
Acest script generează un fișier în directorul `build/` numit `build.log`.

Pentru a interpreta un fișier:
```sh
./bin/interpret [fisier]
```

Se pot interpreta de asemenea și comenzi direct din terminal:
```sh
./bin/interpret
```

### Windows
Pentru Windows, există o procedură similară, folosind scriptul de Powershell:
```
.\build.ps1
```
Acestea sunt opțiunile disponibile în script (`.\build.ps1 -Help`):
```
 Usage: build.ps1 [options]
 Options:
   -Clean         Clean the build directory before building
   -Verbose       Show additional output
   -Quiet         Suppress all output
   -Help          Show this help message and exit
 Example:
   ./build.ps1 -Clean -Verbose
```
Acest script generează un fișier în directorul `build/` numit `build.log`.

Pentru a interpreta un fișier:
```sh
.\bin\interpret.exe [fisier]
```

Se pot interpreta de asemenea și comenzi direct din terminal:
```sh
.\bin\interpret.exe
```

### Sintaxa

Pentru o explicatie la sintaxa, aruncati o privire la fisierul [Documentatie](Documentatie.pdf)
