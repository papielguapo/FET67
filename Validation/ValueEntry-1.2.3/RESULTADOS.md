# FET 67 1.2.3 — doble clic para escribir valores

Campo transitorio sobre el mando, sin alterar la skin en reposo ni el DSP. INPUT/OUTPUT 0–10; ATTACK/RELEASE 1–9; RATIO 4/8/12/20/ALL o formato N:1. INPUT también admite OFF. Coma y punto decimal. Enter aplica con gesto begin/end hacia el host; Escape o pérdida de foco cancela. Los valores numéricos fuera de rango se limitan; texto inválido no altera el parámetro y permanece marcado en rojo.

El doble clic ya no restaura el valor predeterminado. La edición usa un único campo compartido, sigue la escala de la interfaz y no añade parámetros. Los tiempos físicos siguen apareciendo en los popups informativos de los knobs; el campo de edición usa la escala 1–9.

Se añadió una regresión de componentes JUCE: apertura, formato de ratio, coma decimal, límites, ataque/release, ratios y ALL, texto NaN rechazado, Escape, cancelación al perder foco e INPUT OFF. No sustituye una prueba interactiva del foco/teclado dentro de FL Studio. La aceptación dentro de ese DAW sigue pendiente por el fallo del control de Windows.
