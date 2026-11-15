# FET 67 1.2.4 — selección segura del sidechain

## Problema reproducido

La versión anterior seleccionaba el detector externo en cuanto el host activaba el bus auxiliar. Algunos hosts pueden habilitar buses sin enviar señal. Con key silencioso, el detector no comprime; INPUT elevado puede entonces llevar la salida a su saturación de seguridad. No es una propiedad que se deba justificar como carácter 1176.

Seno 80 Hz, amplitud aproximada −6.02 dBFS, 48 kHz, Input 10, Output 5, Attack 1, Release 1, ratio 4:1, FIX/SC FILTER apagados. Último segundo de cuatro, análisis de armónicos 2–299 (hasta 23.92 kHz), magnitudes relativas a la fundamental. Sin ruido añadido; no es THD+N de banda ancha.

| Caso | THD dB | THD % |
|---|---:|---:|
| 1.2.3, detector interno | −25.41 | 5.37 |
| 1.2.3, bus auxiliar activo y silencioso | −7.18 | 43.76 |
| 1.2.4, bus activo sin seleccionar External Sidechain | −25.41 | 5.37 |

La captura del usuario muestra −7.3 dB. El caso reproducido es compatible con ese resultado, pero no demuestra identidad con su configuración: faltan sus valores exactos. No se elimina todo armónico ni se declara equivalencia con otros 1176.

Como referencia exploratoria, Bluey instalado con Input −30, Output −18, Attack/Release 7 (rápidos), ratio 4:1, Analog OFF, Mix 100 %, Trim 0, sobre el mismo seno: THD −36.27 dB / 1.54 %. No se igualó GR ni calibraron tiempos; no constituye comparación de calidad a condiciones equivalentes. FET Input 5.8 con tiempos rápidos mide 3.41 %. El problema del bus explica una distorsión extrema, no toda diferencia de color o envolvente entre modelos.

Se cargó también el UADx 1176 Rev A instalado: Input −28 dB (valor público normalizado 0.4166667), Output −18 dB, Attack/Release 7, ratio 4:1, SC OFF, Mix 100 %, Headroom 16 dB, Power ON, bypass OFF. Mismo seno: THD −27.77 dB / 4.09 %, H2 −31.03 dBc, H3 −32.21 dBc. Su latencia declarada de 86 muestras se retiró al renderizar. Es otra referencia exploratoria sin igualar GR, no evidencia de superioridad o equivalencia. Las versiones instaladas son referencias medidas directamente, no un modelo basado solo en descripciones comerciales.

## Corrección

Nuevo parámetro del host `externalSC`, nombre **External Sidechain**, añadido al final, OFF por defecto. Requiere activación explícita y bus auxiliar habilitado. La skin permanece igual. Cuando se selecciona externo, su silencio libera la compresión; cuando no se selecciona, el bus auxiliar no afecta al detector interno, aunque contenga señal. Sin bus disponible se usa el detector interno.

Los estados anteriores recuperan la selección externa apagada, incluso sobre una instancia reutilizada donde estaba encendida. Los proyectos que usaban sidechain externo deben activarlo desde los parámetros del host y volver a guardar. Para ducking conviene apagar FIX.

También se corrigió el renderizador de pruebas para leer el archivo exclusivamente en el bus principal y dejar los auxiliares en silencio; un lector JUCE puede duplicar un mono en todos los canales del buffer si se le pasa el buffer multibus completo. Esa corrección pertenece al harness, no al audio del VST3.

## Regresiones locales

CTest 3/3. Nuevo ensayo: bus auxiliar habilitado con selector OFF, primero silencioso y luego con señal, salida idéntica al detector interno (error máximo 0), GR 8.74687 dB. Pasan selección externa mono/estéreo, key derecho, ausencia de fuga, SC FILTER, robustez, asignaciones y estado normalizado/legacy. Local-CTest.log y LowFrequency-metrics.json contienen la evidencia. La compilación local verificó el DSP antes del incremento de versión a 1.2.4.

La selección/foco/ruteo/reapertura/exportación dentro de FL Studio y la escucha comparativa siguen pendientes. Esta revisión corrige un fallo reproducido; no certifica que sustituya universalmente a UAD o Waves.

## Build final e instalación

SUCCESS: [GitHub Actions 43766184761](https://github.com/papielguapo/FET67/actions/runs/43766184761), código `cf65118f4220500499510fc2f46ba9532e23ffce`. Windows 2022, Visual Studio 2022, JUCE 9.0.5, versión 1.2.4 Release x64. Compilación, CTest 3/3 y pluginval 1.0.4 nivel 10 con dos repeticiones superados. Registros CI-CTest.log, CI-pluginval.txt y CI.log.

ZIP e instalación usan ese artefacto CI. Instalado en `C:\Program Files\Common Files\VST3\FET 67.vst3`, copia de usuario sincronizada. Se verificó identidad de SHA-256 y se cargó el VST3 instalado en el host de pruebas: External Sidechain aparece al final (índice 11), OFF por defecto. Seleccionándolo y activando el bus externo, el key derecho produce 8.96727 dB de atenuación y no se fuga al audio. Installed-hosted-key.txt conserva los resultados.

SHA-256 DLL: `AB083E79D2D4ECA01A5903DB4C65A1F75F095EFC5455535A8F352AE15D2BB6BF`.

Los commits posteriores de documentación no cambian el binario. La skin permanece igual. Reabrir las instancias para cargar 1.2.4. Firma y aceptación comercial pendientes; estos registros no sustituyen la prueba interactiva del host ni la decisión auditiva.
