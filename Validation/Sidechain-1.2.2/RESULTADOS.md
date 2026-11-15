# FET 67 1.2.2 — sidechain externo sin cambios de skin

Entrada auxiliar Sidechain, mono o estéreo, desactivada por defecto. El host activa el bus y enruta su fuente; no hay nuevo parámetro ni control gráfico. Un key externo activo y silencioso produce cero reducción después de la recuperación, sin conmutación por detección de silencio. Sin bus auxiliar activo se conserva el detector interno.

La señal externa se sanea y sobremuestrea a 8× antes del detector. Se aplica INPUT y una atenuación virtual de realimentación para mantener la ley del controlador; SC FILTER actúa también sobre esa señal. El audio principal sigue la ruta limpia 1.2.1. La señal externa no se suma a la salida. FIX permanece después de la compresión: apagarlo si se busca ducking, ya que su compensación lenta puede contrarrestar la reducción media.

La GUI no cambia. Se conservan IDs y valores iniciales de los parámetros, el bus auxiliar está desactivado por defecto y la latencia sigue siendo de 65 muestras. El estado de rutas/buses corresponde al proyecto del host.

Las nuevas regresiones cubren todas las combinaciones de principal mono/estéreo y key mono/estéreo; key estéreo solo derecho; key silencioso; filtrado a 40 Hz; ausencia de fuga a salida; buffer 4097 tras prepare de 16; NaN en el key y asignaciones en callback. La validación de routing/reapertura en FL Studio y la escucha siguen pendientes.

## Resultados locales

- Compilación MSVC 19.51, JUCE 9.0.5. CTest 3/3, cero fallos. CTest.log conserva el detalle.
- En las cuatro combinaciones de buses: key silencioso GR 0; key 40 Hz activo GR 8.85254 dB; con SC FILTER GR 3.72518 dB. Key estéreo solo derecho activa también el detector con salida principal mono. Principal en silencio, salida exactamente 0 con key activo.
- Key con NaN, bloques 4097 pese a prepare de 16: salida finita; cero operator new en el callback ensayado, sin interceptar malloc nativo.
- Detector interno: render de seno de referencia idéntico muestra a muestra a 1.2.1 Clean, error máximo 0. No es una prueba exhaustiva para cualquier señal; la ruta interna no se ha rediseñado.
- pluginval 1.0.4 nivel 10, semilla 1221511, dos repeticiones aleatorias, 44.1/48/96/192 kHz y bloques 1/16/64/128/512/2048: exit 0, SUCCESS. Local-pluginval.txt. El validador separado de Steinberg no está configurado.
- VST3 cargado realmente en el host de pruebas JUCE, dos buses de entrada, aux estéreo solo derecho: RMS principal con key silencioso −49.0437 dBFS, con key activo −58.0110 dBFS; atenuación 8.96727 dB. Key con principal en silencio no se escucha: pico 0. Local-hosted-key.txt. Es una prueba del VST3, no una sesión de FL Studio.

La orden `FET67Validation --reference <descripcion.xml> --external-key-test` reproduce el ensayo de VST3 con valores iniciales. `--scan <bundle.vst3> <directorio-existente>` genera la descripción. Las nuevas funciones de test posteriores al commit de DSP no modifican el binario del plugin.

## Artefacto final e instalación

SUCCESS: [GitHub Actions 43755872368](https://github.com/papielguapo/FET67/actions/runs/43755872368), código de plugin 5c2859b6dc9c8b5e232c9b623e42040801d6b905. Runner Windows 2022, Visual Studio 2022, JUCE 9.0.5, Release x64. Compilación, tres CTest y pluginval 1.0.4 nivel 10 con dos repeticiones correctos. Registros CI.log, CI-CTest.log y CI-pluginval.txt.

El ZIP y la instalación usan los binarios descargados de ese run. Instalado en `C:\Program Files\Common Files\VST3\FET 67.vst3`, con la copia de usuario sincronizada. La comprobación SHA-256 confirma identidad con el artefacto. Se volvió a cargar el VST3 desde Program Files en el host de pruebas: expone dos buses de entrada, produce 8.96727 dB de atenuación por el key externo derecho y salida 0 al silenciar el principal. Installed-hosted-key.txt conserva el resultado.

SHA-256 DLL instalada/distribuida: `BEAAC5704D9EEA3577F7CFB2C125FF580CBB8164A83EBF8DD7C91C7036F24122`.

La skin y sus fuentes permanecen idénticas a 1.2.1. Se conserva el screenshot del README. Los commits posteriores añaden documentación y un ensayo de host, sin cambiar el DSP del artefacto.

La aceptación dentro de FL Studio, automatización/ruteo/reapertura/exportación en ese DAW, la escucha y los requisitos comerciales siguen pendientes. Binarios sin firma Authenticode. Cerrar y volver a abrir instancias previamente cargadas para usar 1.2.2.
