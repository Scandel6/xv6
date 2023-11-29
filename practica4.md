# Pseudocodigo para recorrer la lista con un planificador de prioridad

```none
capturar lock
while(siguiente != null)
    ejecutar
liberar lock
```


Despues de cada >state = RUNABLE meter a la lista de procesos listos

Despues de >state = ZOMBIE, UNUSED no hace falta hacer nada

Cuando un padre hace fork, el padre mete al hijo a la cola de listos

Cuando está en RUNNING hay que sacarlo de la cola de listos

Un proceso se bloquea si se está ejecutando porque tiene que hacer una llamada al sistema

Todos los procesos tienen una prioridad inicial 5

# Tests

Adaptar los tests de prueba

## dup2

Tienen que pasar todos los tests


## exitwait

Mientras en la linea 39 sea menor de 40, tiene que saltar trap y el resto normal

## tprio


## tsbrk 

Tiene que ser correcto

## tsbrk2

Tiene que fallar

## tsbrk3 

Tiene que ser correcto

## tsbrk4

Tiene que ser correcto

## tsbrk5

Los 3 tests deben de fallar
