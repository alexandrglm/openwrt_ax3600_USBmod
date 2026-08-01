- [X] - Fase 1: Análisis de shell_config.c (Comandos SSDK)

    [X] - Extraer TODOS los comandos y su sintaxis real
    [X] - Identificar qué comandos están compilados (basado en #ifdef)
    [X] - Documentar parámetros y valores

- [ ] - Fase 2: Análisis de hppe_*.c (Implementación HPPE para IPQ8074)

    [ ] - Entender cómo se implementa el offload
    [X] - Identificar puntos de control
    [Parcial] - Ver si hay opciones de bypass no expuestas en la shell

- [ ] - Fase 3: Análisis de nss_*.c (Driver NSS)

    [ ] - Ver cómo se comunican SSDK y NSS
    [ ] - Identificar opciones de configuración
    [ ] - Ver soporte ACL (si existe)

- [ ] - Fase 4: Análisis de ECM

    [ ] - Entender clasificación de flujos
    [ ] - Identificar opciones de offload selectivo
    [ ] - Ver scripts de disable_offloads.sh

- [ ] - Fase 5: Herramientas Userspace

    [ ] - Analizar nssinfo y ppecfg
    [ ] - Ver qué configuraciones permiten
