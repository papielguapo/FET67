# FET 67 1.2.3 — doble clic para escribir valores

Campo transitorio sobre el mando, sin alterar la skin en reposo ni el DSP. INPUT/OUTPUT 0–10; ATTACK/RELEASE 1–9; RATIO 4/8/12/20/ALL o formato N:1. INPUT también admite OFF. Coma y punto decimal. Enter aplica con gesto begin/end hacia el host; Escape o pérdida de foco cancela. Los valores numéricos fuera de rango se limitan; texto inválido no altera el parámetro y permanece marcado en rojo.

El doble clic ya no restaura el valor predeterminado. La edición usa un único campo compartido, sigue la escala de la interfaz y no añade parámetros. Los tiempos físicos siguen apareciendo en los popups informativos de los knobs; el campo de edición usa la escala 1–9.

Se añadió una regresión de componentes JUCE: apertura, formato de ratio, coma decimal, límites, ataque/release, ratios y ALL, texto NaN rechazado, Escape, cancelación al perder foco e INPUT OFF. No sustituye una prueba interactiva del foco/teclado dentro de FL Studio. La aceptación dentro de ese DAW sigue pendiente por el fallo del control de Windows.

## Comprobaciones locales

Compilación correcta y CTest 3/3, cero fallos; `NUMERIC_EDITOR ... PASS` en Local-CTest.log. La suite también conserva las pruebas de DSP, FIX, SC FILTER y sidechain externo. Esta compilación local verificó el código final del editor antes de incrementar únicamente la versión CMake de 1.2.2 a 1.2.3. El binario de distribución se generará por separado con versión 1.2.3.

No hay cambios en PluginProcessor.cpp/.h respecto a 1.2.2. Los nuevos campos no se guardan como controles ni alteran parámetros al cancelar. Se mantiene la apariencia en reposo; los formatos informativos de los sliders se instalan después del attachment de JUCE para que RATIO muestre su razón real, no su índice interno.

## Distribución e instalación verificadas

SUCCESS: [GitHub Actions 43760819656](https://github.com/papielguapo/FET67/actions/runs/43760819656), commit de código `67c16b27f0758da44722766f743d7eff846d16b4`. Windows 2022, Visual Studio 2022, JUCE 9.0.5, Release x64, versión 1.2.3. Compilación, CTest 3/3 (incluida NUMERIC_EDITOR) y pluginval 1.0.4 nivel 10 con dos repeticiones superados. Registros CI.log, CI-CTest.log y CI-pluginval.txt.

Los binarios del ZIP y la instalación proceden de ese artefacto CI. Instalado en `C:\Program Files\Common Files\VST3\FET 67.vst3`; copia de usuario sincronizada. Hash comprobado idéntico al artefacto, escaneo confirma versión 1.2.3 e instanciación offline correcta. El ensayo externo de sidechain sobre el VST3 instalado sigue pasando (Installed-smoke.txt).

SHA-256 DLL: `88CBA62984F81F715F949719CE1DC47E358A8C57E131B7437AF3301569E4D194`.

Los commits posteriores de documentación no cambian el binario. Firma Authenticode, aceptación interactiva dentro de FL Studio y demás condiciones comerciales siguen pendientes. No se declara validación comercial completa.
