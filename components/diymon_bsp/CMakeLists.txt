# CMakeLists.txt para el componente padre diymon_bsp
# Este es el único jefe. Él decide qué drivers se compilan.

# 1. Determinamos en qué subcarpeta están los ficheros de la placa elegida.
if(CONFIG_DIYMON_BOARD_ORIGINAL)
    set(BSP_SUBDIR ${CMAKE_CURRENT_LIST_DIR}/bsp_original)
elseif(CONFIG_DIYMON_BOARD_WAVESHARE_C6)
    set(BSP_SUBDIR ${CMAKE_CURRENT_LIST_DIR}/bsp_waveshare_c6)
endif()

# 2. Buscamos TODOS los ficheros .c que haya en esa subcarpeta.
file(GLOB BSP_SOURCES "${BSP_SUBDIR}/*.c")

# 3. Registramos ESTE componente (diymon_bsp) usando los ficheros que hemos encontrado.
#    El hijo ya no se registra, solo aporta sus ficheros.
idf_component_register(
    SRCS ${BSP_SOURCES}
    INCLUDE_DIRS 
        "include"
        ${BSP_SUBDIR} # Añadimos la carpeta del hijo a las rutas de inclusión
    
    # Lista completa de permisos que cualquiera de los drivers pueda necesitar.
    REQUIRES 
        "log"
        "nvs_flash"
        "lvgl"
        "esp_lvgl_port"
        "esp_lcd"
        "driver"
        "esp_adc"
        "fatfs"
        "lwip"
        "esp_wifi"
        "esp_lcd_jd9853"
        "esp_lcd_touch_axs5106"
)