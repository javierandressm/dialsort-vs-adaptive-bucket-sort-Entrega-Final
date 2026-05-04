# Practice II - DialSort vs Adaptive Range Bucket Sort

**ST0245 - SI001 - Estructuras de Datos y Algoritmos**  
Escuela de Ciencias Aplicadas e Ingenieria - Universidad EAFIT  
Docente: Alexander Narvaez Berrio. Abril 2026

Programa en C++17 que compara experimentalmente **DialSort** con una alternativa propuesta: **Adaptive Range Bucket Sort**.  
Genera arreglos de enteros no negativos, los ordena con ambos algoritmos, mide tiempos y guarda los resultados en CSV.

## 👤 Autores

**Javier Sierra** | **Francis Leon**  
Desarrollado para la materia ST0245 - Estructuras de Datos y Algoritmos

## 📁 Estructura del Proyecto

```text
src/
  main.cpp                     # Interfaz de consola (modos: demo, benchmark, visualize)
  DialSort.h / DialSort.cpp    # Implementacion de DialSort y visualizacion de conteos
  AdaptiveRangeBucketSort.h
  AdaptiveRangeBucketSort.cpp  # Implementacion de Adaptive Range Bucket Sort y visualizacion de buckets
  benchmark.h / benchmark.cpp  # Generacion de datos, medicion, CSV y visualizaciones

docs/
  technical_report.md          # Reporte tecnico

results/
  benchmark_results.csv        # Resultados del benchmark

README.md
Makefile
CMakeLists.txt
```

## 🔧 Algoritmos Comparados

### DialSort

DialSort ordena enteros en el rango `[0, U]` usando un arreglo de conteo.

Pasos:

1. Crear un arreglo `counts` de tamano `U + 1`.
2. Contar cuantas veces aparece cada valor.
3. Reconstruir el arreglo de menor a mayor usando esos conteos.

No usa `std::sort`; el ordenamiento completo se resuelve con el arreglo de conteos.

### Adaptive Range Bucket Sort

Propuesta hibrida de distribucion por rangos.

Pasos:

1. Encontrar minimo y maximo del arreglo.
2. Dividir el rango observado en `k` buckets.
3. Distribuir cada elemento en su bucket correspondiente.
4. Ordenar cada bucket internamente con `std::sort`.
5. Concatenar los buckets en orden.

La estrategia principal es la distribucion por rangos; `std::sort` solo actua dentro de cada bucket.

## ⏱️ Complejidad Temporal

| Algoritmo                  | Mejor Caso   | Caso Promedio | Peor Caso    |
|----------------------------|-------------|---------------|-------------|
| DialSort                   | O(n + U)    | O(n + U)      | O(n + U)    |
| Adaptive Range Bucket Sort | O(n)        | O(n + k·log(n/k)) | O(n log n) |

> DialSort es lineal respecto a `n + U`. Adaptive Range Bucket Sort se acerca a `O(n log n)` cuando los datos se acumulan en pocos buckets.

## 💾 Complejidad Espacial

| Algoritmo                  | Espacio       |
|----------------------------|---------------|
| DialSort                   | O(U)          |
| Adaptive Range Bucket Sort | O(n + k)      |

> `k` = numero de buckets (por defecto `sqrt(n)`).

## 📊 Distribuciones de Datos

El benchmark incluye tres distribuciones:

| Nombre          | Descripcion                                    |
|-----------------|------------------------------------------------|
| `uniform`       | Valores aleatorios uniformes entre `0` y `U`  |
| `nearly_sorted` | Arreglo casi ordenado con pocos intercambios  |
| `repeated`      | Muchos valores repetidos (paleta de 32 valores)|

Se usan semillas fijas para reproducibilidad.

## 🚀 Compilacion y Ejecucion

### Compilar con Makefile

```bash
make
```

En Windows con MinGW:

```powershell
mingw32-make
```

### Compilar con CMake

```bash
cmake -S . -B build
cmake --build build
```

### Modos de ejecucion

**Demo con arreglo pequeno:**

```bash
./sort_benchmark demo
```

**Benchmark completo** (n de 100,000 a 10,000,000):

```bash
./sort_benchmark benchmark
```

**Benchmark con parametros personalizados:**

```bash
./sort_benchmark benchmark --n 100000 --u 100000 --dist uniform --runs 5
```

Opciones disponibles:

```
--n <lista>         Ejemplo: 1000000 o 100000,500000
--u <lista>         Ejemplo: 1000000 o 10000,100000
--dist <nombre>     uniform, nearly_sorted o repeated
--runs <numero>     Repeticiones por configuracion
--buckets <numero>  Buckets para Adaptive Range Bucket Sort; 0 usa sqrt(n)
--out <archivo.csv> Ruta del CSV de salida
```

**Visualizacion interna de ambos algoritmos:**

```bash
./sort_benchmark visualize --n 100 --u 1000 --dist repeated
```

En Windows (`.exe`):

```powershell
.\sort_benchmark.exe demo
.\sort_benchmark.exe benchmark --n 100000 --u 100000 --dist uniform --runs 5
.\sort_benchmark.exe visualize --n 100 --u 1000 --dist repeated
```

## 📋 Salida Esperada

El benchmark imprime por consola el avance de cada configuracion y guarda en CSV:

```text
Configuracion: n=1000000, U=100000, dist=uniform, runs=5
  Run 1: DialSort=45.123 ms, AdaptiveRangeBucketSort=98.456 ms
  ...
Resultados guardados en results/benchmark_results.csv
```

## 📄 CSV de Resultados

Archivo generado: `results/benchmark_results.csv`

Columnas:

| Columna                   | Descripcion                                      |
|---------------------------|--------------------------------------------------|
| `algorithm`               | Nombre del algoritmo                             |
| `n`                       | Cantidad de registros                            |
| `U`                       | Universo de valores                              |
| `distribution`            | Distribucion usada                               |
| `run`                     | Numero de corrida                                |
| `time_ms`                 | Tiempo de esa corrida en milisegundos            |
| `mean_ms`                 | Media de todas las corridas                      |
| `stddev_ms`               | Desviacion estandar de las corridas              |
| `throughput_records_sec`  | Registros ordenados por segundo                  |
| `is_sorted`               | Confirma si el resultado quedo correctamente ordenado |

## 🖥️ Visualizacion Interna

El modo `visualize` muestra el comportamiento interno de ambos algoritmos:

- **DialSort**: conteos agrupados por rango y vista previa de la reconstruccion ordenada.
- **Adaptive Range Bucket Sort**: rango observado, numero de buckets, y cantidad de elementos por bucket con barras de progreso `#`.

## ✅ Verificacion

El programa verifica automaticamente que cada algoritmo produzca un ordenamiento valido usando `std::is_sorted`. El resultado aparece en la columna `is_sorted` del CSV y en la salida del modo `demo`.

## 📌 Notas Importantes

- El dataset obligatorio cubre entre 100,000 y 10,000,000 registros.
- Las semillas son fijas para que ambos algoritmos reciban exactamente el mismo dataset en cada corrida.
- Los tiempos se miden con `std::chrono::high_resolution_clock`.

## 🔮 Posibles Mejoras

Sin agregar complejidad al proyecto, las mejoras mas directas serian:

- **Medicion de memoria real**: registrar el uso de memoria del proceso con `/proc/self/status` en Linux o `GetProcessMemoryInfo` en Windows, en lugar de estimarlo.
- **Grafico automatico del CSV**: agregar un script Python de una sola funcion que lea el CSV y genere graficas de tiempo vs `n` por distribucion.
- **Distribucion sesgada**: agregar una distribucion donde la mayoria de valores se concentra en un extremo, para estresar el caso peor de Adaptive Range Bucket Sort.
- **Numero variable de buckets**: probar automaticamente varios valores de `k` (por ejemplo `sqrt(n)`, `n/10`, `log(n)`) y registrar cual da mejor tiempo.

## 📚 Referencias

- Introduction to Algorithms (CLRS) - Cormen, Leiserson, Rivest, Stein
- C++ STL Documentation
- Dial's Algorithm - Original paper
