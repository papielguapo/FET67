# FET 67 — Papielguapo

![Interfaz del plugin FET 67: frontal de aluminio con controles ultravioleta y medidor LED](Preview-v1.1.png)

Compresor FET mono/estéreo, C++17, JUCE 9.0.5, VST3 y Standalone Windows x64. Código completo, gráficos vectoriales sin imágenes externas y ganancia de entrada/salida exclusivamente manual. Identificadores: fabricante `Papi`, plugin `Ft67`.

## Frontal 1.1

El frontal sigue la referencia visual proporcionada: panel con ranuras horizontales, tornillos negros, piloto violeta grande, cinco mandos esculpidos, palancas oscuras y medidor horizontal. Se reconstruye con vectores y conserva los controles y el medidor funcionales. La firma se dibuja como trazos vectoriales y la marca continúa siendo FET 67. La referencia original se conserva en `Reference/AppearanceTarget.png` para comparar; no se utiliza como imagen de fondo del plugin.

La proporción del lienzo es ahora 1634 × 252, con límites 817 × 126 y 4085 × 630 (50–250 %). La escala del sistema y las posiciones actuales de los controles afectan a la captura; no se afirma identidad píxel a píxel.

## Probar los binarios

En GitHub, descarga `FET67-Complete.zip` de la release para obtener la carpeta `Binaries`; los ejecutables no se guardan en el historial de código.

`Binaries/FET 67.exe` es la aplicación Standalone. Selecciona el dispositivo de audio y sus entradas en sus ajustes. Evita realimentar físicamente la salida hacia la entrada.

Copia la carpeta completa `Binaries/FET 67.vst3` a la carpeta VST3 que escanee tu DAW. La ubicación habitual de Windows es `C:\Program Files\Common Files\VST3`, que puede requerir permisos de escritura. No copies solamente el archivo interno. Se entrega sin instalador ni firma digital. El proyecto no instala ni sustituye automáticamente ningún plugin durante estas pruebas.

## Compilar con Visual Studio 2022

Requisitos: CMake 3.22+, Git no es necesario para descargar el archivo de JUCE, Visual Studio con Desarrollo para el escritorio con C++ y Windows SDK. Desde esta carpeta:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DFET67_COPY_PLUGIN_AFTER_BUILD=OFF
cmake --build build --config Release --parallel 4
```

También funciona con el generador `Visual Studio 18 2026`, usado para los binarios entregados. CMake utiliza `C:/JUCE` si contiene JUCE 7, 8 o 9; en caso contrario descarga JUCE 9.0.5 desde el repositorio oficial. Para otra copia local, añade `-DFET67_JUCE_PATH="D:/SDK/JUCE"`. La selección por defecto de JUCE 9 responde a la petición posterior de utilizar la versión actual.

Los resultados se generan en `build/FET67_artefacts/Release/VST3` y `build/FET67_artefacts/Release/Standalone`. `FET67_COPY_PLUGIN_AFTER_BUILD` está activado por defecto como se pidió; el ejemplo lo desactiva para permitir compilar sin escribir en Program Files.

## Controles y DSP

- Power apagado y Bypass dejan pasar la señal seca. Los cambios internos tienen una transición de 20 ms para evitar clics; una vez terminada, la salida seca es exacta. El bypass del host también está soportado.
- Input 0 silencia la ruta procesada. De 0 a 0,1 se interpola hasta −24 dB; de 0,1 a 10 se llega a +30 dB. Output 0–10 representa −24 a +24 dB, con 5 = 0 dB. No hay compensación automática de ganancia.
- Attack 1–9 corresponde a 20–800 µs; Release 1–9 a 50–1100 ms. **1 es rápido y 9 lento**, según la especificación original. La numeración y sentido no deben trasladarse directamente desde Waves.
- Ratios 4, 8, 12, 20 y ALL tienen los umbrales y knees solicitados. ALL usa ataque de 450 µs, retardo del detector y una rama de recuperación más larga.
- El detector utiliza la señal post-FET del sample anterior. La ley de control usa R−1 para obtener la pendiente R:1 en una realimentación ideal; el comportamiento dinámico y las no linealidades alteran la pendiente medida.
- Transformador de entrada con curva tanh, HP de 18 Hz; asimetría FET dependiente de GR, compensación polinómica LN y bloqueo DC de 20 Hz. La salida tiene ganancia manual y limitación suave por encima de la rodilla de calibración +18 dBu.
- Link comparte el máximo de los dos detectores. Apagado permite detección dual mono. Los estados se mantienen separados y el cambio de enlace se interpola.
- Meter GR marca 0 hacia la izquierda; OUT utiliza RMS suavizado, con 0 = −18 dBFS RMS. El LED CLIP ocupa el siguiente escalón del rango de visualización (+6 respecto a esa referencia), no es un medidor de true peak. No existe una conversión única de RMS a pico para todas las señales: la referencia −12 dBFS pico supone 6 dB de crest factor.
- El editor usa un lienzo lógico 1634 × 252 y transformación afín, con escala del 50 al 250 %. El tamaño se guarda en el estado del plugin. Doble clic en un knob restaura su valor; el popup muestra el valor, tiempos o ratio.

## Pruebas y mediciones de referencia

```powershell
cmake -S . -B build -DFET67_BUILD_TESTS=ON -DFET67_COPY_PLUGIN_AFTER_BUILD=OFF
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

`Tests/Tests.cpp` contiene pruebas offline reales y un host VST3. `Reference/RESULTADOS.md` describe las mediciones realizadas sobre el CLA-76 Stereo V14 instalado, seleccionado en Bluey, Analog Off. Los archivos `.txt` conservan los resultados originales, parámetros públicos y las trazas de transitorios de 1 ms. No se distribuye software, presets ni recursos gráficos de Waves.

## Estado de validación

Se han construido VST3 y Standalone con MSVC 19.51 / Visual Studio 2026 y JUCE 9.0.5, y se ha renderizado la GUI. También se construyeron ambos formatos con JUCE 8.0.6. Las comprobaciones offline cubren bypass, silencio de entrada, ratios, enlace/dual mono, persistencia y mono a varias frecuencias de muestreo. No se ha ejecutado la matriz de compatibilidad de DAWs, pluginval ni una sesión de escucha de material musical. Visual Studio 2022 y JUCE 7 conservan el código compatible pero no se han probado en esta máquina.

Este es un modelo original funcional, con mediciones de referencia, no una reproducción certificada 1:1 del circuito ni un clon de Waves. No utiliza sobremuestreo: la distorsión puede producir aliasing con señales intensas de alta frecuencia. Para una publicación comercial quedan validación en hosts, evaluación auditiva, pruebas de aliasing/rendimiento y cumplimiento de la licencia de JUCE que corresponda al distribuidor. Ninguna afirmación de equivalencia sonora o certificación comercial está implícita.

Fuentes técnicas: [JUCE CMake oficial](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md), [manual oficial Waves CLA-76](https://assets.wavescdn.com/pdf/plugins/cla-76-compressor-limiter.pdf).
