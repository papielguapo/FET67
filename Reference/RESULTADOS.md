# Referencia medida: Waves CLA-76 Bluey

Fecha: 15 de noviembre de 2026. Referencia elegida por el usuario: **CLA-76 Bluey**, sustituyendo UADx Rev A. Se cargó el plugin instalado a través de su interfaz pública VST3; no se inspeccionó su código DSP ni se copiaron sus recursos.

## Método

Windows x64, JUCE 9.0.5, host offline propio. WaveShell1-VST3 14.12_x64; CLA-76 instalado 14.12.90.381. 48 kHz, bloques de 128 samples, senoidal de 1 kHz durante 3 segundos por nivel. El resultado es RMS de la última parte del tercer segundo. Los niveles de entrada son dBFS pico; los de salida, dBFS RMS. `reset()` se invoca entre condiciones, pero un plugin externo puede conservar memoria analógica interna: son medidas de este protocolo y orden de barrido, no valores universales.

Parámetros Waves: Input −30, Output −18, Attack 3, Release 4, Revision Bluey, Analog Off, Mix 100 %, Trim 0 dB, bypass desactivado. El ratio se cambia entre 4, 20 y ALL. Una cuarta pasada activa Comp Off para distinguir la ruta analógica del detector. Cada archivo incluye un volcado inicial (Blacky por defecto) y otro **APPLIED PARAMETERS**, que confirma Bluey durante las medidas.

Para el escalón: seno continuo de 1 kHz, −36 dBFS pico durante 100 ms, −6 dBFS pico durante 500 ms y retorno a −36 durante 1400 ms. Se registra RMS por ventana de 1 ms. Son trazas de amplitud de salida, no lecturas directas del medidor GR ni una medida submilisegundo del ataque.

## Curvas de transferencia medidas

| Entrada pico | Bluey 4:1 | Bluey 20:1 | Bluey ALL | Bluey Comp Off | FET 67 4:1 ajustado |
|---:|---:|---:|---:|---:|---:|
| −48 | −41,831 | −41,831 | −41,181 | −41,831 | −41,880 |
| −36 | −29,827 | −29,827 | −29,177 | −29,827 | −29,880 |
| −30 | −23,821 | −23,821 | −23,171 | −23,821 | −23,880 |
| −24 | −18,646 | −17,809 | −17,670 | −17,809 | −19,208 |
| −18 | −17,090 | −12,831 | −17,530 | −11,786 | −17,206 |
| −12 | −16,059 | −12,631 | −17,235 | −5,741 | −15,684 |
| −6 | −14,956 | −12,481 | −16,768 | −0,158 | −14,183 |
| 0 | −13,466 | −12,253 | −15,954 | +4,678 | −12,683 |

Todos los resultados de salida están en dBFS RMS. Los valores positivos de Comp Off se conservan tal como salen del plugin flotante; no se recortaron a 0 dBFS.

El VST3 compilado de FET 67 también se cargó como plugin externo en el mismo host. Su barrido coincide con el del procesador directo a la precisión mostrada (archivo `FET67-VST3-Host.txt`), comprobando la ruta de audio del binario distribuido.

El ajuste de comparación de FET 67 utiliza **Input 5,80, Output 5,43, Attack 3, Release 4, Ratio 4:1**, sin cambiar los defaults solicitados. Igualar nivel de pequeña señal permite comparar mejor la curva; no iguala tiempos ni distorsión. La diferencia de pequeña señal es de unos −0,05 dB; en los puntos comprimidos va aproximadamente de −0,56 a +0,78 dB. Se entregan también los resultados con los defaults originales de FET 67, cuya ganancia de pequeña señal es prácticamente unidad.

## Qué se aprende y qué se conserva

- Bluey muestra cambio de ganancia y de umbral al entrar en ALL. El modelo FET 67 conserva su modo ALL dedicado y los umbrales especificados, sin introducir autogain para perseguir esas curvas.
- El cambio de ratio en Waves no equivale a cambiar únicamente una pendiente: la familia de curvas tiene distintos umbrales. FET 67 ya implementa esa dependencia explícitamente.
- Con el protocolo de 3 segundos, la respuesta a 18 kHz de Bluey Comp Off cae aproximadamente 4,87 dB respecto a 1 kHz. FET 67 no incorpora esa caída de agudos: su ruta solicitada contiene dos HP de baja frecuencia y conserva los agudos. Es una diferencia identificada, no un parámetro ajustado a ciegas.
- El HP del detector y el estado dinámico pueden influir en las pasadas de baja frecuencia. La medición de frecuencia en modo compresor no debe interpretarse como respuesta lineal del transformador.
- Los tiempos y la numeración de Waves no se trasladan directamente a los rangos 1–9 solicitados. Las trazas permiten comparar recuperación de nivel; no demuestran una equivalencia exacta del ataque de 20 µs.

## Reproducir

Con `FET67_BUILD_TESTS=ON`, se construye `FET67ReferenceCatalog.exe` en Windows. Usa solamente el catálogo de clases público del SDK VST3 y genera descripciones XML de CLA-76, evitando el escaneo completo de los 612 componentes del shell instalado:

```powershell
.\build\Release\FET67ReferenceCatalog.exe "C:/Program Files/Common Files/VST3/WaveShell1-VST3 14.12_x64.vst3" "C:/ruta/absoluta/para/descripciones"
.\build\FET67Tests_artefacts\Release\FET67Tests.exe --reference "C:/ruta/absoluta/para/descripciones/CLA-76 Stereo.xml" --measure 8=0 7=1 6=0.75
.\build\FET67Tests_artefacts\Release\FET67Tests.exe --fetmeasure input=5.8 output=5.43 ratio=0
```

La carpeta de descripciones debe existir. Los índices corresponden a los parámetros públicos de la versión V14 medida. Se deben revisar con `--reference archivo.xml` antes de reutilizarlos con otra versión. No se cambia el plugin instalado ni sus presets. La advertencia de WMIC que aparece en los logs procede de Waves en este Windows y no impidió cargar ni procesar audio.
