# Validación de FET 67 1.2.0

15 de noviembre de 2026. Windows x64, JUCE 9.0.5, C++17. Estado: **candidato de validación**, sin certificación comercial completa.

## Correcciones comprobadas

| Ensayo | 1.1 | 1.2 |
|---|---:|---:|
| Exceso de pico al volver del bypass, primer bloque del ensayo | +9,26 dB | +0,243 dB durante el fundido |
| Residuo frente al control después del fundido | Reiniciaba detector | 0 en el ensayo |
| Error RMS estéreo con máximos alternados | +3,008 dB | Aproximadamente 0,0001 dB |
| Alias 18 kHz: seno 10 kHz, 48 kHz, Input 10, Attack 9 | −13,61 dBc | −106,23 dBc |
| Alias plegado: seno 10 kHz, 44,1 kHz | Sin ensayo equivalente | −111,44 dBc |
| H2: seno 1 kHz, Input 10, Attack 9 | −45,45 dBc | −35,96 dBc |
| H3: misma condición | −13,59 dBc | −24,19 dBc |
| Ruta de plugin inexistente | Éxito falso | Error y regresión automática |

Los bins de aliasing no representan toda la energía de aliasing. Los armónicos son relativos a la fundamental de cada salida y dependen de la condición. No demuestran identidad con Bluey.

FIX iguala el RMS en el ensayo estacionario a unos 0,02 dB tras estabilizar; al desactivarlo, el residuo frente al procesador de control es 0. La prueba también verifica que no modifique el audio seco con POWER OFF.

SC FILTER (80 Hz, solo detector) está incluido en el build independiente final citado más abajo. Su regresión pasa localmente y en CI: GR de 40 Hz OFF 8,18484 / ON 3,14598 dB; a 1 kHz OFF 8,88863 / ON 8,86261 dB. Error de audio ON/OFF con compresión inactiva: 0. Los estados anteriores a SC FILTER lo recuperan apagado. El parámetro LINK permanece para compatibilidad aunque ya no tiene control en el frontal.

## Suite automática

- CTest: DSP, ExtendedValidation y MissingPluginRejected.
- pluginval oficial 1.0.4 nivel 10, dos repeticiones aleatorizadas; matriz 44,1/48/96/192 kHz y bloques 1/16/64/128/512/2048. Incluye editor, automatización, fuzz y estado desde hilo de fondo. Consulta el registro final, no los logs de iteraciones previas.
- Robustez adicional a 44,1/48/88,2/96/176,4/192 kHz; bloques de 4097 pese a anunciar 16 en prepare; buffers vacíos; NaN/Inf y valores extremos.
- Partición de bloques 1/7/31/127/1025/8193 frente a 128: error máximo 0 en el ensayo.
- Impulso seco: exactamente los 65 samples de latencia declarados.
- Cero asignaciones mediante operator new en los callbacks ensayados. El monitor no intercepta directamente malloc; no es una certificación de toda actividad de memoria del sistema.
- Silencio digital recién preparado: pico 0 con Input y Output en 10. Cola: pico 0 en la última ventana. El valor −300 dBFS del informe es un suelo numérico para evitar log(0), no una medición analógica.
- Benchmark indicativo: alrededor de 8–10 % de un núcleo a 48 kHz estéreo en esta máquina. No garantiza carga en otros equipos ni plazos de tiempo real bajo cualquier carga.

La validación extrema encontró que APVTS redondeaba interruptores mientras el host conservaba automatización normalizada fraccionaria. El estado final conserva ambas representaciones. Los logs antiguos con fallos se mantienen como historial; no son el resultado final.

## Prueba vocal privada

Extracto educativo: Al James, Schoolboy Fascination, Cambridge-MT, 33 WAV 24 bits/44,1 kHz. **No procede de Abbey Road.** Voz principal renderizada con CLA-76 Bluey/Analog Off y dos ajustes de FET 67, con latencia retirada y 0,5 s de cola. Se igualó RMS ajustando OUTPUT manualmente. El audio es finito y tiene la misma duración; picos y envolventes difieren. No hay aprobación auditiva ni equivalencia de GR demostrada.

El proveedor restringe esas pistas a uso educativo. Audio y renders permanecen fuera del repositorio y de la distribución comercial. No se utilizan como material publicitario ni como autorización de explotación comercial.

## Pendiente para cerrar la aceptación comercial

1. **FL Studio:** carga, reproducción, compensación de latencia, automatización, guardado/reapertura y exportación. El control de Windows falló antes de iniciarla: `failed to write kernel assets`, error 3, también después de reset. No se marca como realizada.
2. **Escucha:** varias voces y voz en mezcla; sibilancia, consonantes, respiraciones y preferencia musical a niveles igualados. Decisión final del propietario.
3. **Otros hosts y Steinberg validator:** no ejecutados aquí. pluginval no sustituye esas comprobaciones.
4. **Distribución:** binarios sin Authenticode. No se han verificado credenciales de firma, licencia comercial de JUCE, instalador ni soporte. No se han creado certificados ni modificado la confianza de Windows.

## Build independiente y procedencia del binario

**SUCCESS:** [GitHub Actions 43743676421](https://github.com/papielguapo/FET67/actions/runs/43743676421), commit de código `22f96db3eff0d0de5e9f8e2240dc9ef57651f035`. Runner Windows 2022, generador Visual Studio 17 2022, Release x64, JUCE 9.0.5 descargado del archivo oficial.

La compilación, las tres pruebas CTest y pluginval 1.0.4 nivel 10, dos repeticiones, finalizaron correctamente. Se descargó el artefacto de ese run y se utilizaron **sus binarios** para la instalación final y el ZIP. Los commits posteriores de documentación no cambian ese código.

Registros: `Windows2022-CI.log`, `Windows2022-CTest.log` y `Windows2022-pluginval.txt`. Los ensayos numéricos locales usan el mismo código compilado con MSVC 19.51; los ensayos del runner usan su toolchain de Visual Studio 2022.

SHA-256 del VST3 distribuido e instalado:

`4B6F1500D88161FA896051F8B1F7051F2931BD9186551F4A3973B7BF51EDF6B4`

El binario depende de los runtimes MSVCP140/VCRUNTIME140 y de componentes de Windows indicados en `Dependencies.txt`. No se ha certificado ejecución en todas las versiones de Windows. La firma Authenticode continúa pendiente.

La instalación final en Program Files se verificó por SHA-256 y por instanciación del VST3. `Installed-Parameters.txt` confirma FIX y SC Filter 80 Hz, ambos apagados por defecto. La prueba dentro de FL Studio y la aprobación auditiva permanecen pendientes.
