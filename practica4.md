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