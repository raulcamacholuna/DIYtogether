# CMakeLists.txt para el componente diymon_bsp
# Versión final: Modular, robusta y a prueba de futuro.

# -----------------------------------------------------------------------------
# PASO 1: PUNTO ÚNICO DE CONFIGURACIÓN
# -----------------------------------------------------------------------------
set(BOARD_SUBDIR "WS1.9TS")

# -----------------------------------------------------------------------------
# PASO 2: DETECCIÓN AUTOMÁTICA DE FUENTES
# -----------------------------------------------------------------------------
set(BSP_PATH ${CMAKE_CURRENT_LIST_DIR}/${BOARD_SUBDIR})
file(GLOB BSP_SOURCES "${BSP_PATH}/*.c")

# -----------------------------------------------------------------------------
# PASO 3: LISTA COMPLETA DE DEPENDENCIAS (LA CLAVE DE LA SOLUCIÓN)
# -----------------------------------------------------------------------------
# Aquí declaramos TODAS las herramientas que CUALQUIER placa pueda necesitar.
set(BSP_DEPS
    # Drivers específicos de ESP-IDF
    esp_driver_i2c
    esp_driver_gpio
    esp_driver_spi
    esp_driver_ledc
    sdmmc
    
    # --- [LA CORRECCIÓN ESTÁ AQUÍ] ---
    # Al añadir "esp_lcd", le decimos a CMake: "Oye, mi componente necesita
    # los archivos de cabecera (.h) y las librerías del driver LCD de Espressif".
    # Esto resuelve el error "esp_lcd.h: No such file or directory".
    esp_lcd
    esp_lcd_touch
    esp_adc
    log
    nvs_flash
    fatfs
    lwip
    esp_wifi

    # Tus otros componentes personalizados
    lvgl
    esp_lvgl_port
    esp_lcd_jd9853
    esp_lcd_touch_axs5106
)

# -----------------------------------------------------------------------------
# PASO 4: REGISTRAR EL COMPONENTE
# -----------------------------------------------------------------------------
# Finalmente, registramos el componente. La directiva "REQUIRES" consume
# nuestra lista de dependencias y las enlaza correctamente.
idf_component_register(
    SRC_DIRS "." ${BOARD_SUBDIR}

    # Los directorios de inclusión están bien.
    INCLUDE_DIRS
        "include"
        ${BOARD_SUBDIR}
        
    # La lista de dependencias se consume aquí.
    REQUIRES ${BSP_DEPS}
)