# FET 67 — papielguapo

**Fabricante: papielguapo · 1.2.0 · Windows x64**

![FET 67 con POWER a la izquierda y FIX bajo el medidor](Preview-v1.2.png)

Compresor FET mono/estéreo, C++17/JUCE 9.0.5, VST3 y Standalone. Detector de realimentación, recuperación dependiente del programa y coloración asimétrica. Es un modelo original comparado con CLA-76 Bluey; no se ha demostrado equivalencia 1:1 ni preferencia en escucha ciega.

## Novedades de 1.2

- Sobremuestreo FIR **8×**, con filtros y tiempos recalculados a la frecuencia interna.
- **65 samples de latencia** declarados al host y compensados también en la ruta seca: unos 1,35 ms a 48 kHz.
- Más margen en la saturación de entrada y mayor armónico par. En el ensayo de 1 kHz con Input 10 y Attack 9, H2 pasa de −45,45 a −35,96 dBc y H3 de −13,59 a −24,19 dBc frente a 1.1. Son resultados de esa condición concreta.
- Detector y filtros continuos durante bypass; desaparece el reinicio que provocaba el pico al volver a activar.
- RMS independiente por canal, seguido de selección del mayor.
- Estado compatible con automatización fraccionaria de interruptores y con proyectos anteriores.
- Procesamiento por fragmentos de capacidad fija, preparado fuera del callback.

El DSP cambia el sonido de sesiones anteriores. Se mantienen fabricante `Papi`, código `Ft67` y los IDs antiguos. El parámetro `bypass` continúa para el DAW y para proyectos antiguos; su botón se ha sustituido por **FIX**. POWER permanece junto al piloto violeta.

## FIX: comparar niveles

**FIX está apagado por defecto.** Actívalo para aproximar el RMS de salida al RMS de la señal original de entrada, antes del mando INPUT. Usa un ajuste común para ambos canales, después del compresor, sin modificar su detector.

La estimación RMS usa 750 ms y la ganancia se mueve suavemente durante aproximadamente 1 s; deja varios segundos para estabilizar. El margen está limitado a ±24 dB y el aprendizaje se mantiene durante el silencio. No puede recuperar una entrada silenciada ni garantiza igualdad de sonoridad LUFS o percepción. Tampoco limita picos: deja margen en tu DAW. Cuando POWER está en OFF o el DAW hace bypass, FIX no aplica ganancia a la ruta seca.

Con FIX apagado, INPUT y OUTPUT son completamente manuales y no hay autogain. Los cambios de activación tienen fundidos de 20 ms.

## Controles

| Control | Rango y uso |
|---|---|
| INPUT | 0 = OFF; 0,1–10 equivale a −24…+30 dB |
| OUTPUT | −24…+24 dB; posición 5 = 0 dB |
| ATTACK | 1 = 20 µs, 9 = 800 µs: **1 rápido, 9 lento** |
| RELEASE | 1 = 50 ms, 9 = 1100 ms; recuperación dependiente del programa |
| RATIO | 4:1, 8:1, 12:1, 20:1 y ALL nominal 24:1 |
| ALL | Ataque de 450 µs, lag del detector y recuperación modificada |
| LINK | ON comparte detección; OFF permite dual mono |
| METER | GR o RMS OUT; 0 OUT = −18 dBFS RMS |

Los tiempos son constantes del modelo, no tiempos efectivos certificados frente al hardware. CLIP es un escalón de la escala y no un medidor de true peak. La saturación suave no es un limitador brickwall. El bypass asentado conserva los samples secos exactamente, desplazados los 65 samples declarados.

La interfaz es vectorial: 1634×252, escala 50–250 %, tamaño guardado. Doble clic en un knob restaura el valor inicial.

## Punto de partida para voz

Ratio 4:1, Attack 7–9, Release 4, Output 5. Sube Input hasta obtener aproximadamente 3–6 dB de GR en las partes fuertes. Compara con FIX o iguala OUTPUT manualmente. Baja Attack hacia 1 para sujetar más los picos; súbelo hacia 9 para preservar más el inicio de palabras. La numeración no se traslada directamente desde Waves.

Se preparó un A/B educativo privado con la voz de Al James, Schoolboy Fascination, de Cambridge-MT. Los renders y las pistas no se distribuyen en este repositorio. Su uso no constituye autorización para una demo comercial ni una aprobación auditiva.

## Instalar

Descarga el ZIP de la release y copia la carpeta completa `Binaries/FET 67.vst3` a `C:\Program Files\Common Files\VST3`. Puede requerir permisos de administrador. Cierra instancias cargadas antes de actualizar, reescanea plugins y busca **FET 67 / Papielguapo**. También se entrega Standalone. No se incluye instalador ni firma Authenticode.

## Compilar y validar

Visual Studio 2022 con C++ de escritorio, Windows SDK y CMake 3.22+:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DFET67_COPY_PLUGIN_AFTER_BUILD=OFF -DFET67_BUILD_TESTS=ON
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

CMake usa `C:/JUCE` si contiene JUCE 7/8/9; en su ausencia descarga JUCE 9.0.5 oficial. Otra copia local: `-DFET67_JUCE_PATH="D:/SDK/JUCE"`. Los binarios están en `build/FET67_artefacts/Release`; las pruebas en `build/FET67Tests_artefacts/Release/FET67Validation.exe`. La copia automática está activada por defecto; el ejemplo la desactiva para no escribir en Program Files durante el build.

El workflow Windows validation compila con Visual Studio 2022, ejecuta CTest y pluginval oficial 1.0.4 nivel 10, y conserva artefactos. Consulta [Validation/STATUS.md](Validation/STATUS.md) y sus logs para distinguir lo comprobado de lo pendiente. Los datos de `Reference/` son históricos de 1.1, no una calibración de 1.2.

La aprobación auditiva, la prueba en FL Studio, otros hosts, el validador independiente de Steinberg y los requisitos de distribución deben registrarse por separado. No se declara validación comercial completa.
