# Fecha: 15/08/2025 - 03:42 
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\iot_button\CMakeLists.txt
# Último cambio: Eliminadas las llamadas a 'package_manager' y 'cu_pkg_define_version' para resolver el error de compilación en PlatformIO.
# Descripción: Registro del componente 'iot_button'. Se han eliminado las funciones de gestión de paquetes que requerían un fichero 'idf_component.yml' inexistente, causando el fallo de CMake. El componente ahora se registra de forma estándar, permitiendo que PlatformIO lo compile correctamente.

set(PRIVREQ esp_timer)
set(REQ driver)
set(SRC_FILES "button_gpio.c" "iot_button.c" "button_matrix.c")

if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
    list(APPEND REQ esp_adc)
    if(CONFIG_SOC_ADC_SUPPORTED)
        list(APPEND SRC_FILES "button_adc.c")
    endif()
endif()

idf_component_register(SRCS ${SRC_FILES}
                        INCLUDE_DIRS include interface
                        REQUIRES ${REQ}
                        PRIV_REQUIRES ${PRIVREQ})

# [CORRECCIÓN] Se eliminaron las siguientes líneas que causaban el error de CMake,
# ya que este componente es local y no necesita gestionar su versión a través
# del registro de componentes de Espressif.
# include(package_manager)
# cu_pkg_define_version(${CMAKE_CURRENT_LIST_DIR})
