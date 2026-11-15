# FET 67 1.2.1 Clean

Revisión solicitada para reducir la distorsión, sin añadir controles.

- Drive de entrada: 0.06 → 0.015.
- Coeficiente par FET: 0.10 → 0.015.
- Coeficiente impar FET: 0.008 → 0.001.
- Saturación de salida: umbral −4 → 0 dBFS, así se evita colorear salidas que todavía tienen margen digital. Sigue sin ser un limitador true peak y puede entregar picos por encima de 0 dBFS.
- Se conservan detector, ratios, constantes temporales, oversampling 8×, latencia, SC FILTER y FIX. Al cambiar la no linealidad dentro de la realimentación, la envolvente puede variar, sobre todo con INPUT extremo.

Las sesiones antiguas conservan los parámetros, pero esta revisión cambia su sonido. Las cifras y aprobaciones de versiones anteriores son históricas; las mediciones y verificaciones de este build se añadirán después de ejecutarlas. La prueba dentro de FL Studio y la evaluación auditiva siguen pendientes.
