# Interpret pseudocod
Acest program va permite sa rulati pseudocod direct pe calculator.
Pentru a copia folder-ul:
```
git clone https://github.com/vitelariu/pseudocod
```
Compilarea interpretului:
```
TODO
```
> [!NOTE]
> Pentru a interpreta un fisier:
> ```
> /interpret {fisier sursa}
> ```
> Pentru a interpreta comenzi direct din terminal:
> ```
> ./interpret
> ```

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
| Mai mic | ```>``` | ```>``` |
| Mai mare sau egal | ```>=``` | ```>=``` |
| Mai mic sau egal | ```<=``` | ```<=``` |
| Diferit | ```!=``` | ```!=``` |


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
> ```citeste y, z (bool)```
>
> ----------------------------------------------------------------
> 
> Daca nu se precizeaza tipul de data, se ia ca default tipul real.

### Declaratii de control
| Nume | Pseudocod | C++ |
| :--: | :-------: | :-: |
| conditional | ```daca``` ```altfel daca``` ```altfel``` | ```if()``` ```else if()``` ```else``` |
| bula | ```cat timp``` ```executa ... pana cand``` | ```while()``` ```do {...} while()```
| bula cu numar cunoscut de pasi | ```pentru i <- 0, 10 executa``` | ```for(int i{}; i <= 10; i++)``` |

### Conversii
In pseudocod exista un singur tip de conversie: ```(TODO: mai multe tipuri de coversii in viitor poate?)```


Din tipul real in tipul intreg ```scrie [5 / 2]```

output: ```2```

### Alte lucruri importante
Identarea este foarte importanta! Se face cu un **tab**, sau **4 spatii**.

(Doua tab-uri = 8 spatii, etc)



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


