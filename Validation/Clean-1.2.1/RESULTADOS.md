# FET 67 1.2.1 Clean

Revisión solicitada para reducir la distorsión, sin añadir controles.

- Drive de entrada: 0.06 → 0.015.
- Coeficiente par FET: 0.10 → 0.015.
- Coeficiente impar FET: 0.008 → 0.001.
- Saturación de salida: umbral −4 → 0 dBFS, así se evita colorear salidas que todavía tienen margen digital. Sigue sin ser un limitador true peak y puede entregar picos por encima de 0 dBFS.
- Se conservan detector, ratios, constantes temporales, oversampling 8×, latencia, SC FILTER y FIX. Al cambiar la no linealidad dentro de la realimentación, la envolvente puede variar, sobre todo con INPUT extremo.

Las sesiones antiguas conservan los parámetros, pero esta revisión cambia su sonido. Las cifras y aprobaciones de versiones anteriores son históricas. La prueba dentro de FL Studio y la evaluación auditiva siguen pendientes.

## Antes y después medidos

Seno de 1 kHz, amplitud aproximada −6.02 dBFS, 48 kHz, cuatro segundos, último segundo analizado. Input 5.8, Output 5.5304, Attack 3, Release 4, ratio 4:1, FIX y SC apagados. Herramienta local MSVC 19.51; las magnitudes se expresan respecto a la fundamental de cada salida.

| Armónico | 1.2.0 | 1.2.1 Clean |
|---|---:|---:|
| H2 | −44.16 dBc | −60.63 dBc |
| H3 | −59.49 dBc | −60.55 dBc |
| H4 | −82.16 dBc | −99.07 dBc |

H2 baja 16.47 dB en esa condición. A drive extremo (Input 10, Attack 9, seno 1 kHz de amplitud 0.5), el ensayo de regresión pasa de H2/H3 históricos −35.96/−24.19 dBc a −51.40/−46.10 dBc. No es distorsión cero: la modulación de ganancia también puede generar armónicos y los resultados dependen del programa y los ajustes.

Curva sostenida a 1 kHz, niveles de entrada pico −48/−36/−30/−24/−18/−12/−6/0 dBFS: máxima diferencia frente a 1.2.0 de 0.0021 dB con los ajustes anteriores. No garantiza identidad de envolventes en transitorios o al llevar INPUT al máximo.

## Regresiones locales

CTest: 3/3, cero fallos. La suite incluye una nueva comprobación de H2 < −56 dBc y H3 < −60 dBc a Input 5.8, Attack 3. Pasan también mono/estéreo, parámetros y estado, FIX, SC, latencia de 65 samples, silencio, buffers extremos y valores no finitos. Los registros están en CTest.log, Transfer.txt y Harmonics.json.

VST3 local: pluginval oficial 1.0.4, nivel 10, semilla 1211511, dos repeticiones aleatorias, frecuencias 44.1/48/96/192 kHz y bloques 1/16/64/128/512/2048. Exit 0, SUCCESS; registro Local-pluginval.txt. El validador separado de Steinberg no está configurado y no se declara ejecutado.

El monitor de memoria informa cero operator new en los callbacks ensayados, sin interceptar malloc nativo. Los benchmarks son offline y no certifican ausencia de cortes en una sesión real.

Se conserva el silencio digital exacto; no se ha añadido un denoiser ni una puerta. Esta revisión reduce la saturación propia, no elimina el ruido de una grabación. FIX no evita saturar otra etapa ni garantiza margen de pico.

## Frente a CLA-76 Bluey

Referencia instalada: CLA-76 Stereo, Bluey, Analog Off, Input −30, Output −18, Attack 3, Release 4, ratio 4:1, Mix 100 %, Trim 0. No se igualó GR ni se calibraron tiempos entre modelos. Con el seno anterior, Bluey mide H2 −52.85 dBc y H3 −61.48 dBc. Clean tiene H2 7.78 dB inferior y H3 0.93 dB superior en esa condición. No se concluye menor distorsión total para cualquier entrada ni superioridad auditiva.

Se renderizó también la misma voz educativa privada que en el A/B previo, compensando latencia y añadiendo 0.5 segundos de cola. A RMS común −21.1146 dBFS, Bluey tiene pico −4.6648 dBFS y Clean −8.4014 dBFS (cresta 16.4498 frente a 12.7133 dB). Clean conserva la menor cresta que mostraba el ajuste Control de 1.2.0, con menos coloración en el ensayo sintético. Un pico máximo no representa la GR de toda la voz. El nuevo render se igualó con ganancia uniforme posterior +0.00223 dB, no con FIX ni con normalización LUFS.

El audio sigue fuera del repositorio/distribución: Cambridge-MT, uso educativo privado, no Abbey Road. La elección musical requiere escucha al mismo nivel dentro de la mezcla.

## Build e instalación final

SUCCESS: [GitHub Actions 43753009889](https://github.com/papielguapo/FET67/actions/runs/43753009889), código 326213dfbcaba87e3244cf41c66534256a2abda6. Windows 2022, Visual Studio 2022, JUCE 9.0.5, Release x64. Compilación, tres CTest y pluginval 1.0.4 nivel 10 con dos repeticiones correctos. Registros CI.log, CI-CTest.log y CI-pluginval.txt.

Se utilizan los binarios descargados de ese run para el ZIP y la instalación en `C:\Program Files\Common Files\VST3\FET 67.vst3`. La copia de usuario también se ha sincronizado para evitar una versión antigua en el escaneo. El instalador verificó que el SHA-256 instalado coincidiera con el artefacto; escaneo e instanciación offline confirmaron versión 1.2.1 y parámetros. Esto no equivale a probar una instancia ya abierta dentro de FL.

SHA-256 DLL: `1BFD2AEEE7720896F0B308EA9DA0335B9F17BED6231628A91FB9E940BC2CF90F`.

Los commits posteriores de documentación no cambian el código del binario. Distribución candidata, sin firma Authenticode; aceptación en FL Studio, escucha y demás requisitos comerciales siguen pendientes según STATUS.md. Cerrar y volver a abrir instancias previamente cargadas para usar la revisión nueva.
