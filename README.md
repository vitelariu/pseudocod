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

# Sintaxa

### Expresii
| Nume | Pseudocod | C++ |
| :--: | :-------: | :-: |
| Adunare | ```+``` | ```+``` |
| Scadere | ```-``` | ```-``` |
| Inmultire | ```*``` | ```*``` |
| Impartire | ```/``` | ```/``` |
| Modulo | ```%``` | ```%``` |
| Putere | ```^``` | ```pow()``` |
| Asignare | ```<-``` | ```=``` |
| Egal | ```=``` | ```==``` |
| Mai mare | ```>``` | ```>``` |
| Mai mic | ```<``` | ```<``` |
| Mai mare sau egal | ```>=``` | ```>=``` |
| Mai mic sau egal | ```<=``` | ```<=``` |
| Diferit | ```!=``` | ```!=``` |
| Si | ```si``` | ```and``` |
| Sau | ```sau``` | ```or``` |


### Intrare / Iesire
| Pseudocod | C++ |
| :-------: | :-: |
| ```scrie 1``` | ```cout << 1``` |
| ```citeste x``` | ```cin >> x``` |

### Tipuri de date
| Interval | Pseudocod | C++ | Exemplu |
| :------: | :-------: | :-: | :-----: |
| $0, 1, 2 ... 2^{64} - 1$ | ```(numere naturale)``` | ```unsigned long long  int``` | ```69```, ```0```, ```999``` |
| $-2^{63}, -2^{63} + 1 ... 2^{63} - 1$ | ```(numere intregi)``` | ```long long int``` | ```-1```, ```120```, ```0``` |
| $[1.7 * 10^{-308}, 1.7 * 10^{308}]$ | ```(numere reale)``` | ```double``` | ```3.141592```, ```-12.0```, ```32``` | 
| - | ```(text)``` | ```string``` | ```"Hello"```, ```"a"``` |
| - | ```(logica)``` | ```bool``` | ```Adevarat```, ```Fals```|

> [!IMPORTANT]
> In pseudocod se precizeaza tipul de data doar cand se citeste o variabila!
> Exemple:
>
> #### Pentru tipul natural:
> 
> ```citeste x (numar natural)```
> 
> ```citeste y, z (numere naturale)```
>
> #### Pentru tipul intreg:
> 
> ```citeste x (numar intreg)```
> 
> ```citeste y, z (numere intregi)```
>
> #### Pentru tipul real
>
> ```citeste x (numar real)```
>
> ```citeste y, z (numere reale)```
>
> #### Pentru tipul text
>
> ```citeste var (text)```
>
> ```citeste var1, var2, var3, var4, var5 (text)```
>
> #### Pentru tipul boolean
>
> ```citeste x (bool)```
>
> ```citeste x (logica)```
>
> ```citeste y, z (bool)```
>
> ```citeste y, z (logica)```
>
> ----------------------------------------------------------------
> 
> Daca nu se precizeaza tipul de data, se ia ca default tipul real.

### Declaratii de control
| Nume | Pseudocod | C++ |
| :--: | :-------: | :-: |
| conditional | ```daca``` ```altfel daca``` ```altfel``` | ```if()``` ```else if()``` ```else``` |
| bucla | ```cat timp``` ```executa ... pana cand``` | ```while()``` ```do {...} while()```
| bucla cu numar cunoscut de pasi | ```pentru i <- 0, 10 executa``` | ```for(int i{}; i <= 10; i++)``` |

### Conversii
In pseudocod exista un singur tip de conversie: ```(TODO: mai multe tipuri de coversii in viitor poate?)```


Din tipul real in tipul intreg ```scrie [5 / 2]```

output: ```2```

### Identare

Identarea se face cu tab-uri sau spatii. Un tab este egal cu o identare. Un spatiu, pana la patru spatii este egal cu o identare.

Spre exemplu: Daca avem Un tab + 3 spatii se ia ca 2 identari



# Exemple pseudocod

### Aduna 2 numere citite de la tastatura
```
citeste x, y
scrie x + y, "\n"
```

```
#include <iostream>
using namespace std;

int main() {
  cin >> x >> y;
  cout << x + y << '\n';

  return 0;
}
```

### Verifica daca n este palindrom
```
citeste n
n_copy <- n

inv <- 0

cat timp n_copy != 0 executa
    c <- n_copy % 10
    n_copy <- [n_copy / 10]
    inv <- inv * 10 + c

daca n = inv atunci
    scrie "Numarul n: ", n, " este palindrom\n"
altfel
    scrie  "Numarul n: ", n, " nu este palindrom\n"
```

```
#include <iostream>
using namespace std;

int main() {
    int n;
    cin >> n;
    int n_copy = n;
    int inv = 0;

    while(n_copy != 0) {
        int c = n_copy % 10;
        n_copy = n_copy / 10;
        inv = inv * 10 + c;
    }

    if(n == inv) {
        cout << "Numarul n: " << n << " este palindrom\n";
    }
    else {
        cout << "Numarul n: " << n << " nu este palindrom\n";
    }
    

    return 0;
}
```

### Al n-lea numar fibonacci
```
citeste n
a <- 0
b <- 1
c <- 1

n <- n - 1

pentru i <- 0, n - 1 executa
    c <- a + b
    a <- b
    b <- c
scrie c, "\n"
```

```
#include <iostream>
using namespace std;

int main()
{
    int n;
    cin >> n;
    int a = 0, b = 1, c = 1;
    
    n--;

    for(int i = 0; i < n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    cout << c << '\n';
    
    
    return 0;
}
```

### TODO: mai multe exemple


