# Practice II - DialSort vs Adaptive Range Bucket Sort

Proyecto en C++17 para comparar experimentalmente **DialSort** con una alternativa propuesta: **Adaptive Range Bucket Sort**.

La idea del proyecto es sencilla: generar arreglos de enteros no negativos, ordenarlos con ambos algoritmos, medir tiempos y guardar los resultados en CSV.

## Estructura

```text
src/
  main.cpp
  DialSort.h
  DialSort.cpp
  AdaptiveRangeBucketSort.h
  AdaptiveRangeBucketSort.cpp
  benchmark.h
  benchmark.cpp

docs/
  technical_report.md

results/
  benchmark_results.csv

README.md
Makefile
CMakeLists.txt
```

## Que hace cada archivo

- `src/main.cpp`: lee los argumentos de consola y llama el modo pedido: `demo`, `benchmark` o `visualize`.
- `src/DialSort.h` y `src/DialSort.cpp`: implementan DialSort y su visualizacion interna de conteos.
- `src/AdaptiveRangeBucketSort.h` y `src/AdaptiveRangeBucketSort.cpp`: implementan Adaptive Range Bucket Sort y su visualizacion interna de buckets.
- `src/benchmark.h`: declara la configuracion del benchmark y las funciones principales de ejecucion.
- `src/benchmark.cpp`: genera datasets, mide tiempos, calcula media, desviacion estandar, throughput, valida ordenamiento, escribe CSV y muestra visualizaciones.
- `docs/technical_report.md`: reporte tecnico para explicar la practica.
- `results/benchmark_results.csv`: archivo donde se guardan los resultados.

## Algoritmos comparados

### DialSort

DialSort ordena enteros en el rango `[0, U]` usando un vector de conteo.

Pasos:

1. Crear un arreglo `counts` de tamano `U + 1`.
2. Contar cuantas veces aparece cada valor.
3. Reconstruir el arreglo de menor a mayor usando esos conteos.

Complejidad:

- Tiempo: `O(n + U)`.
- Memoria: `O(U)`.

### Adaptive Range Bucket Sort

Adaptive Range Bucket Sort es una propuesta hibrida basada en buckets por rango.

Pasos:

1. Encontrar minimo y maximo.
2. Dividir el rango observado en varios buckets.
3. Enviar cada numero a su bucket correspondiente.
4. Ordenar cada bucket internamente con `std::sort`.
5. Concatenar los buckets.

Importante: la alternativa no es simplemente `std::sort`; es un algoritmo de distribucion por rangos que usa `std::sort` solo dentro de cada bucket.

Complejidad:

- Promedio esperado: depende de que tan bien se repartan los datos entre buckets.
- Peor caso: `O(n log n)` si casi todo cae en un solo bucket.
- Memoria: `O(n + k)`, donde `k` es el numero de buckets.

## Distribuciones

El benchmark incluye tres distribuciones:

- `uniform`: valores aleatorios uniformes entre `0` y `U`.
- `nearly_sorted`: arreglo casi ordenado.
- `repeated`: muchos valores repetidos.

Se usan semillas fijas para que los resultados sean reproducibles.

## Compilar

Con Makefile:

```bash
make
```

En Windows con MinGW:

```powershell
mingw32-make
```

Tambien se incluye `CMakeLists.txt`:

```bash
cmake -S . -B build
cmake --build build
```

## Ejecutar

Demo pequena:

```bash
./sort_benchmark demo
```

Benchmark completo:

```bash
./sort_benchmark benchmark
```

Benchmark reducido:

```bash
./sort_benchmark benchmark --n 100000 --u 100000 --dist uniform --runs 5
```

Visualizacion interna:

```bash
./sort_benchmark visualize --n 100 --u 1000 --dist repeated
```

En Windows, si el ejecutable queda como `.exe`:

```powershell
.\sort_benchmark.exe demo
.\sort_benchmark.exe benchmark --n 100000 --u 100000 --dist uniform --runs 5
.\sort_benchmark.exe visualize --n 100 --u 1000 --dist repeated
```

## CSV

El benchmark genera:

```text
results/benchmark_results.csv
```

Columnas:

```text
algorithm,n,U,distribution,run,time_ms,mean_ms,stddev_ms,throughput_records_sec,is_sorted
```

- `time_ms`: tiempo de una corrida.
- `mean_ms`: promedio de las corridas.
- `stddev_ms`: desviacion estandar.
- `throughput_records_sec`: registros ordenados por segundo.
- `is_sorted`: confirma si el resultado quedo ordenado.

## Visualizacion

El modo `visualize` muestra el comportamiento interno de ambos algoritmos:

- DialSort: conteos por valor o rango y reconstruccion ordenada.
- Adaptive Range Bucket Sort: cantidad de buckets, rango observado y elementos por bucket con barras `#`.
