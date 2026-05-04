# Technical Report

## 1. Introduccion

Esta practica compara experimentalmente DialSort con una alternativa propuesta llamada Adaptive Range Bucket Sort. Ambos algoritmos ordenan enteros no negativos y se prueban variando el tamano de entrada `n`, el universo de valores `U` y la distribucion de los datos.

## 2. Problema

Se necesita ordenar arreglos cuyos valores estan en el rango `[0, U]`. El objetivo no es solo implementar los algoritmos, sino medir su comportamiento real y relacionarlo con su complejidad teorica.

## 3. DialSort

DialSort usa un arreglo de conteo. Primero cuenta cuantas veces aparece cada valor entre `0` y `U`, y despues reconstruye el arreglo en orden.

Complejidad:

- Mejor caso: `O(n + U)`.
- Caso promedio: `O(n + U)`.
- Peor caso: `O(n + U)`.
- Memoria: `O(U)`.

Su ventaja aparece cuando `U` no es muy grande. Su desventaja es que siempre necesita memoria proporcional a `U`.

## 4. Adaptive Range Bucket Sort

Adaptive Range Bucket Sort es una alternativa hibrida de distribucion por rangos.

El algoritmo:

1. Encuentra el minimo y el maximo.
2. Divide ese rango en `k` buckets.
3. Inserta cada elemento en su bucket.
4. Ordena internamente cada bucket con `std::sort`.
5. Concatena los buckets en orden.

No se presenta como `std::sort`, porque `std::sort` solo se usa dentro de cada bucket. La estrategia principal es distribuir los datos por rangos antes de ordenar.

Complejidad:

- Mejor caso: cercana a `O(n)` si los buckets quedan muy pequenos o equilibrados.
- Caso promedio: `O(n + suma de costos de ordenar cada bucket)`.
- Peor caso: `O(n log n)` si casi todos los elementos caen en un solo bucket.
- Memoria: `O(n + k)`.

## 5. Diseno experimental

El benchmark varia:

- `n`: cantidad de registros.
- `U`: universo de valores.
- Distribucion: `uniform`, `nearly_sorted`, `repeated`.
- Repeticiones por configuracion.

Para cada corrida se genera un dataset con semilla fija. Ambos algoritmos reciben el mismo dataset para que la comparacion sea justa.

La implementacion esta separada por responsabilidades:

- `DialSort.cpp`: contiene solo la logica de DialSort.
- `AdaptiveRangeBucketSort.cpp`: contiene solo la logica de Adaptive Range Bucket Sort.
- `benchmark.cpp`: contiene generacion de datos, medicion, CSV y visualizacion.
- `main.cpp`: contiene la interfaz por consola.

## 6. Metricas

Se miden:

- Tiempo de ejecucion con `chrono`.
- Media.
- Desviacion estandar.
- Throughput en registros por segundo.
- Validacion con `is_sorted`.

Los resultados se guardan en `results/benchmark_results.csv`.

## 7. Visualizacion interna

La visualizacion ayuda a defender el funcionamiento de los algoritmos:

- DialSort muestra los conteos y la reconstruccion ordenada.
- Adaptive Range Bucket Sort muestra el rango observado, los buckets y cuantos elementos cayeron en cada uno.

## 8. Analisis esperado

DialSort deberia ser muy eficiente cuando `U` es pequeno o moderado, porque su costo es lineal respecto a `n + U`. Sin embargo, si `U` es grande, el vector de conteo puede consumir mucha memoria y tiempo.

Adaptive Range Bucket Sort puede funcionar bien cuando los datos se distribuyen entre varios buckets. Si la distribucion es mala y muchos elementos caen en el mismo bucket, su rendimiento se acerca al de ordenar muchos elementos con `std::sort`.

## 9. Conclusiones

DialSort es simple y rapido cuando el universo de valores es controlado. Adaptive Range Bucket Sort es mas flexible frente a universos grandes, pero depende de la distribucion de los datos y del numero de buckets.

La comparacion muestra que el mejor algoritmo depende de las condiciones experimentales: `n`, `U` y distribucion.

## 10. Posibles mejoras

- Medir memoria real del proceso.
- Probar otras cantidades de buckets.
- Agregar distribuciones sesgadas.
- Graficar automaticamente los resultados del CSV.
