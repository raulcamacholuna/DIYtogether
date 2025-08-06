# Este componente no tiene fuentes, solo gestiona a sus hijos.
idf_component_register(INCLUDE_DIRS "include")

# Aquí está la magia: dependiendo de la opción del menú,
# le decimos al compilador que incluya una u otra carpeta de drivers.
if(CONFIG_DIYMON_BOARD_ORIGINAL)
    set(SUBPROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/bsp_original)
elseif(CONFIG_DIYMON_BOARD_WAVESHARE_C6)
    set(SUBPROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/bsp_waveshare_c6)
endif()

# Registramos la carpeta elegida como un sub-componente que debe ser compilado.
idf_component_add_subproject(REQUIRES ${SUBPROJECT_DIR})