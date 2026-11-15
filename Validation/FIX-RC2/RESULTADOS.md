# FET 67 1.2.3 RC2 — orientación de FIX

FIX: ON a la izquierda, OFF a la derecha, apagado por defecto. Se invierten tanto las etiquetas como el dibujo de la palanca. No cambian valores, IDs, estado ni procesamiento de audio. El cambio de código se limita a dos líneas de PluginEditor.cpp respecto a RC1.

Se conserva 1.2.3 como versión interna; la distribución se identifica como RC2. Las pruebas y hashes de ValueEntry-1.2.3 corresponden a RC1; este informe registra la procedencia del binario RC2. La aceptación dentro de FL Studio sigue pendiente.

Compilación local correcta; snapshot del editor inspeccionado: etiquetas ON izquierda / OFF derecha y palanca a la derecha con FIX apagado. Preview-v1.2.png actualizado.

SUCCESS: [GitHub Actions 43762424280](https://github.com/papielguapo/FET67/actions/runs/43762424280), código `88fac146e245f7d49b210c5bfa8d122f223706de`. Windows 2022, Visual Studio 2022, JUCE 9.0.5, Release x64. Compilación, CTest 3/3 y pluginval 1.0.4 nivel 10, dos repeticiones, correctos. Registros CI-CTest.log y CI-pluginval.txt.

Se descargaron sus binarios para el ZIP y la instalación en `C:\Program Files\Common Files\VST3\FET 67.vst3`. La copia de usuario también se sincronizó. Instalador finalizó correctamente y verificó identidad por SHA-256 con el artefacto.

SHA-256 DLL RC2: `30FDCD74559D8784677E69BFECEB2D179019373B1E848DFF7F772D937180F392`.

Los commits posteriores de imagen/documentación no cambian el código del binario. Reabrir instancias antiguas para cargarlo. Binarios sin firma; las condiciones de validación comercial pendientes se mantienen.
