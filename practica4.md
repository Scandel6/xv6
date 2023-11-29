# Pseudocodigo para recorrer la lista con un planificador de prioridad

```none
capturar lock
while(siguiente != null)
    ejecutar
liberar lock
```


Despues de cada >state = RUNABLE meter a la lista de procesos listos

Despues de >state = ZOMBIE no hace falta hacer nada

Cuando un padre hace fork, el padre mete al hijo a la cola de listos

