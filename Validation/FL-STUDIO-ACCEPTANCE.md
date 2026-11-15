# Aceptación pendiente en FL Studio

Ruta del VST3: `C:\Program Files\Common Files\VST3\FET 67.vst3`. Esta prueba aún no se ha ejecutado: el control de Windows falló al inicializarse. El procedimiento permite cerrarla de forma explícita, sin confundir pluginval con una prueba en el DAW.

Trabajar en un proyecto de prueba nuevo o en una copia, con monitores a nivel moderado.

1. Cerrar instancias anteriores y reescanear los plugins. Confirmar **FET 67 / Papielguapo** y la versión de la revisión instalada.
2. Cargar una voz seca y abrir el plugin. Confirmar POWER junto al piloto y FIX en el hueco derecho bajo el medidor.
3. Reproducir y alternar POWER. Verificar transición sin reinicio brusco de compresión, silencio inesperado o clic de activación.
4. Con FIX apagado, ajustar Input y Output manualmente. Activar FIX, esperar varios segundos y comparar el RMS con la entrada original, dejando margen de pico. FIX no iguala LUFS y puede alcanzar su límite de ±24 dB en ajustes extremos.
5. Cambiar Ratio, SC FILTER y Meter; automatizar Input, POWER y FIX. Verificar que el audio siga siendo finito y que la interfaz siga la automatización. SC FILTER debe reducir la reacción a graves sin recortarlos directamente del audio.
6. Guardar el proyecto de prueba, cerrar/reabrirlo y comprobar todos los valores y el tamaño del editor. Un proyecto anterior a FIX debe recuperarlo apagado.
7. Exportar una copia a WAV, compararla con reproducción y comprobar la compensación de los 65 samples de latencia en el contexto de ruteo utilizado.
8. Escuchar la voz en mezcla y el A/B privado, alternando salidas igualadas. Registrar sibilancia, consonantes, respiraciones, control de picos y preferencia de timbre. La escucha no se sustituye por una cifra de aliasing.

## Sidechain externo (1.2.2)

Con FIX apagado, enviar una pista disparadora mediante «Sidechain to this track» al insert con FET 67. En el wrapper, Processing → Connections, habilitar/asignar la entrada auxiliar Sidechain al envío. Comprobar que el key dispara GR y no se escucha en la salida; silenciarlo y observar la recuperación. Con bus externo activo y silencioso no debe conmutar al detector interno. Desactivar el bus para volver al detector interno. Probar SC FILTER, automatización, exportación offline y guardado/reapertura con la ruta externa. El estado de los buses/envíos se guarda en el proyecto del host.

Referencia: [manual oficial de Connections](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/plugins/wrapper.htm). No registrar estos pasos como superados hasta realizarlos en FL Studio.

Registrar la versión exacta de FL Studio, interfaz/controlador, frecuencia, buffer, resultado y cualquier incidencia. Hasta que se complete, la aceptación comercial dentro de FL Studio queda pendiente.
