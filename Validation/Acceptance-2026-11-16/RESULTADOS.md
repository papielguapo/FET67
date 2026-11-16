# Validación técnica — 16 de noviembre de 2026

Plugin: PAPIELGUAPO - 1167 Compressor 1.2.4. Alcance solicitado: DSP, robustez, pruebas de host y comparación vocal. Quedan fuera distribución comercial, firma, instalador y revisión de licencias. No se ha modificado el DSP ni instalado otra revisión durante esta sesión.

## Respuesta en frecuencia

Se añadió un medidor offline de respuesta estacionaria: seno independiente por frecuencia, tres segundos, análisis de la fundamental mediante correlación compleja sobre el último segundo. Tasas 44.1/48/96 kHz. Nivel bajo 0.00001 pico (−100 dBFS), sin compresión apreciable. Se mide el VST3 realmente instalado y también el procesador compilado en la herramienta; la diferencia máxima es 0.0000003 dB entre los datos redondeados.

Se repitió con seno de amplitud 0.5 (−6.02 dBFS pico), Input 5.8, Output 5, Attack 3, Release 4, ratio 4:1, FIX/SC FILTER/External Sidechain apagados. Esto evalúa un ajuste comprimido concreto, no todos los posibles.

Ganancia de 20 kHz relativa a 1 kHz:

| Frecuencia de muestreo | Nivel bajo | Ajuste comprimido |
|---|---:|---:|
| 44.1 kHz | −0.07784 dB | −0.08051 dB |
| 48 kHz | +0.00388 dB | +0.00045 dB |
| 96 kHz | +0.00866 dB | +0.00482 dB |

No aparece el realce de varios dB de la captura en este ensayo estacionario. Se verifica que los puntos de 5/10/15/20 kHz quedan dentro de ±0.2 dB respecto a 1 kHz en los seis casos. No se ha confirmado la configuración exacta de Plugin Doctor del usuario. Un barrido sobre un procesador con ganancia variable, y especialmente con FIX, no demuestra por sí solo una EQ fija. Antes de compararlo, desactivar FIX, igualar ganancia y usar nivel sin compresión. Este ensayo no mide directamente aliasing.

A 48 kHz, nivel bajo, el modelo mide −5.585 dB a 20 Hz, −1.172 a 50 Hz y −0.307 a 100 Hz, relativos a 1 kHz. Es la caída de sus filtros de audio, no el SC FILTER. No se añadió EQ para forzar una coincidencia visual con UAD. Se midieron también UADx Rev A y CLA-76 Bluey a nivel bajo; son modelos distintos, normalizables en 1 kHz, sin demostrar identidad. Un dato inicial de UAD a 1 Hz no es consistente con el resto de tasas y no se usa para conclusiones; la tabla de aceptación usa banda audible y registros completos preservados.

Archivos: Installed-response.txt, Installed-compressed-response.txt, FET-response.txt, UAD-response.txt, Bluey-response.txt, Response-summary.json. El harness permite --response-fet y --reference descripcion.xml --response, con level=amplitud y argumentos de parámetros documentados en Tests.cpp.

## Regresiones del binario actual

CTest 3/3, cero fallos. Incluye latencia de 65 muestras, buffers extremos, NaN/Inf, silencio, C++ operator new, estado, FIX, filtros, selector externo y entrada numérica. CTest.txt conserva el resultado.

El VST3 instalado supera pluginval oficial 1.0.4, nivel 10, semilla 161126, dos repeticiones aleatorias, 44.1/48/96/192 kHz y bloques 1/16/64/128/512/2048, exit 0 y SUCCESS. Installed-pluginval.txt. No se declara ejecutado el validador separado de Steinberg.

## Comprobaciones reales en FL Studio 2026

Se abrió una sesión de prueba independiente. Se cargó el plugin en el Master. El host mostró 65 samples / 1.35 ms de latencia. El editor y sus parámetros se reconocen mediante accesibilidad.

Se abrió INPUT con doble clic y se introdujo 6,25 con teclas individuales; Enter aplicó 6.25 al parámetro. El envío masivo de texto/pegado del controlador no modificaba el campo, por lo que se cambió el método de entrada; no se atribuye ese fallo de automatización al plugin.

Se guardó acceptance67.flp y se reabrió desde proyectos recientes. Se creó otra instancia del wrapper y se verificó INPUT 6.25, Output 5, Attack 3, Release 4, Ratio 4:1, SC/FIX OFF y Power ON. La copia del proyecto de prueba se mantiene fuera del repo en ../67-Compressor-Final-AB/FL-Project (ruta relativa al directorio outputs del usuario). Era una sesión sin audio: no se confunde con validación de reproducción o exportación.

Limitación concreta: coexistían varias instancias de FL y otro trabajo con EQP-1A. Después de solicitar pausar el otro control, se completó escritura y reapertura. Más tarde la captura del área interna de FL apareció negra aunque AX seguía mostrando los controles, y el helper volvió a responder «already has an active request». Se detuvo la interacción; no se modificaron otras sesiones.

Se reinició el controlador y se volvió a consultar la ventana: la misma instancia pasó a `vocal24-acceptance.flp` y mostraba PAPIELGUAPO - VOCAL Compressor además de 1167. No se continuó modificando esa sesión ni se atribuyeron sus resultados a esta validación. El proyecto acceptance67 guardado antes de ese cambio se conserva como evidencia de la prueba aislada realizada.

**No completados en FL:** reproducción con audio real, automatización grabada, routing/recuperación de sidechain, exportación y comparación con reproducción, prueba de compensación en rutas reales. Los tests offline y pluginval no sustituyen esos pasos.

## Comparación auditiva preparada

Dos fragmentos de voz de 20 segundos: Al James y Clara Berry & Wooldog. El segundo procede de un stem MP3 público del mixer educativo de NYU, no de un WAV seco certificado. Fuentes e instrucciones quedan con el material privado, fuera del repositorio y del paquete del plugin.

Se calibró INPUT de los tres plugins a 6 dB de atenuación efectiva RMS sobre seno de 1 kHz a −6.02 dBFS pico respecto a su ganancia a señal pequeña. Tras 12 segundos de asentamiento en bajo y 6 en alto por iteración: FET 6.00765 dB (Input normalizado 0.377495), UAD 6.00704 (0.107809) y Bluey 6.01561 (0.191217). Logs *-calibration.txt. Esto no equivale a igualar GR instantánea en las voces, ni a calibrar las constantes temporales.

Los seis renders usan VST3 cargados realmente. Salida normalizada por ganancia uniforme a −24 dBFS RMS, sin igualación LUFS. Duración común 904050 muestras a 44.1 kHz, latencia declarada retirada y 0.5 s de cola. Audio finito y picos por debajo de 0 dBFS. AB-metrics.json contiene cifras y correspondencias; para escucha ciega usar las instrucciones privadas sin abrir la clave antes.

Configuración de referencia: FET Input normalizado 0.377495, Output 5, Attack 3, Release 4, ratio 4:1, FIX/SC/externo OFF. UAD Input normalizado 0.107809, Output normalizado 0.625, Attack 4, Release 5, ratio 4:1, HR 16 dB, SC OFF, Mix 100 %, Power ON. Bluey Input normalizado 0.191217, Output normalizado 0.625, Attack 3, Release 4, ratio 4:1, Analog OFF, Mix 100 %, Trim 0, Comp ON. Los valores normalizados son los enviados al host; no se presupone que sus escalas numéricas equivalgan entre modelos.

SHA-256 de la DLL instalada durante esta sesión: `389E8CCD29E1B5CDDBD852804C6754490F517EA00D0A45CD07B8AF4664A1209F`.

No se declara una preferencia auditiva ni que sustituya a UAD o Bluey. La elección final requiere escucha del propietario, también dentro de una mezcla. Sigue pendiente la aprobación auditiva.

## Estado

La investigación estacionaria no justifica corregir con EQ el pico de la captura. No se ha observado ese pico en los casos medidos, y las pruebas técnicas realizadas pasan. La aceptación completa solicitada permanece abierta por los pasos de FL y la evaluación auditiva indicados arriba. Firma, instalador y revisión de licencias quedan excluidos por instrucción del usuario.
