# FET 67 1.2.2 — sidechain externo sin cambios de skin

Entrada auxiliar Sidechain, mono o estéreo, desactivada por defecto. El host activa el bus y enruta su fuente; no hay nuevo parámetro ni control gráfico. Un key externo activo y silencioso produce cero reducción después de la recuperación, sin conmutación por detección de silencio. Sin bus auxiliar activo se conserva el detector interno.

La señal externa se sanea y sobremuestrea a 8× antes del detector. Se aplica INPUT y una atenuación virtual de realimentación para mantener la ley del controlador; SC FILTER actúa también sobre esa señal. El audio principal sigue la ruta limpia 1.2.1. La señal externa no se suma a la salida. FIX permanece después de la compresión: apagarlo si se busca ducking, ya que su compensación lenta puede contrarrestar la reducción media.

La GUI no cambia. Se conservan IDs y valores iniciales de los parámetros, el bus auxiliar está desactivado por defecto y la latencia sigue siendo de 65 muestras. El estado de rutas/buses corresponde al proyecto del host.

Las nuevas regresiones cubren todas las combinaciones de principal mono/estéreo y key mono/estéreo; key estéreo solo derecho; key silencioso; filtrado a 40 Hz; ausencia de fuga a salida; buffer 4097 tras prepare de 16; NaN en el key y asignaciones en callback. Los resultados se registrarán tras ejecutarlas. La validación de routing/reapertura en FL Studio y la escucha siguen pendientes.
