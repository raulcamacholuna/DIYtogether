# Fichero: components/zigbee_comm/CMakeLists.txt
# Último cambio: 24/08/2025 - 16:10
# Descripción: Diagnóstico de Causa Raíz Definitivo: El error de compilación 'Failed to resolve component 'esp_zigbee_core'' se debe a que la directiva 'REQUIRES' en este fichero apuntaba a nombres de componentes inexistentes ('esp_zigbee_core', 'esp_zigbee_ha'). Estos son artefactos internos, no componentes registrables. El componente real, según la estructura de directorios, es 'esp-zigbee-lib'. Solución Definitiva: Se ha reemplazado la lista de 'REQUIRES' con el nombre de componente correcto y único: 'esp-zigbee-lib'. Esto permite a CMake encontrar el componente, procesar sus dependencias y propagar correctamente sus directorios de inclusión a este componente ('zigbee_comm'), resolviendo el error de enlazado de forma definitiva.
idf_component_register(
    SRCS
        "zigbee_comm.c"
    INCLUDE_DIRS
        "include"
        "${CMAKE_CURRENT_LIST_DIR}/../../components_dependencies/esp-zigbee-lib/include"
    REQUIRES
        esp-zigbee-lib
        esp_common
        log
        nvs_flash
    PRIV_REQUIRES
        driver
)

