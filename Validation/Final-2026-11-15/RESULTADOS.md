# Pruebas finales — 15 de noviembre de 2026

Resultado: suite automática superada. Sin cambios de DSP ni binario.

## Binario instalado

Ruta: C:\Program Files\Common Files\VST3\FET 67.vst3
SHA-256 DLL: 9FFEC92D49C13253B2C60A2146F436E0B4BCF91CF34B1A14F82C33CC4EAD198F
Coincide con Binaries y el artefacto publicado del código c0aa3d3.

## Resultados

- CTest: 3/3, 0 fallos (CTest.log). Ejecutable de pruebas local MSVC 19.51; no es la DLL instalada.
- DLL instalada: pluginval oficial 1.0.4 nivel 10, semilla 151126, dos repeticiones aleatorias, 44.1/48/96/192 kHz, bloques 1/16/64/128/512/2048. Código de salida 0 y SUCCESS. Incluye editor, automatización, estado y buses.
- Suite extendida: PASS, también repetida por separado (Extended-isolated.txt).
- Latencia seca: 65 muestras. Error por partición de bloques: 0. NaN/Inf, bloques vacíos y bloques 4097 con prepare de 16: PASS.
- Silencio nuevo y cola: pico 0. Asignaciones operator new en callbacks ensayados: 0; no intercepta malloc nativo.
- Alias del ensayo 10 kHz: -106.23 dBc a 48 kHz, -111.437 dBc a 44.1 kHz; no es una medida de toda la energía de aliasing.
- FIX: diferencia RMS estacionaria ~0.02 dB; apagado, residuo 0.
- SC: a 40 Hz GR OFF 8.18484 / ON 3.14598 dB. A 1 kHz OFF 8.88863 / ON 8.86261 dB. Audio sin compresión: diferencia 0.

## Tiempo de procesamiento

Primera ejecución concurrente con pluginval: bloque 32, peor callback 0.9897 ms frente a presupuesto de 0.6667 ms. No se oculta este dato; un benchmark offline de Windows incluye planificación y carga ajena.

Repetición aislada: media 9.2–9.9 % de un núcleo. Peores callbacks: 32 muestras 0.3619 ms; 64 muestras 0.3005 ms; 128 muestras 0.4975 ms; 512 muestras 1.7633 ms. Todos por debajo de sus presupuestos en esa ejecución. No garantiza ausencia de cortes en sesiones reales.

## Pendientes

FL Studio está abierto, pero el control de Windows no pudo inicializar: failed to write kernel assets, os error 3. No se ha modificado su sesión. Carga/reproducción, guardado/reapertura y exportación dentro de FL permanecen pendientes; consultar ../FL-STUDIO-ACCEPTANCE.md. La escucha final, otros hosts y los requisitos de distribución comercial indicados en ../STATUS.md siguen pendientes. Estos resultados no acreditan equivalencia con CLA-76 Bluey ni superioridad auditiva.
