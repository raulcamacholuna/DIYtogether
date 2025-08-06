@echo off
setlocal enabledelayedexpansion

REM --- Script de Diagnóstico para el Proyecto DIYMON ---
REM --- Creado por Raúl y su IA de confianza. ¡Por DIYMON! ---

REM Nombre del fichero de salida
set "OUT_FILE=informe_diymon.txt"

REM Limpiamos el informe anterior si existe
if exist %OUT_FILE% del %OUT_FILE%

echo =================================================== >> %OUT_FILE%
echo        INFORME DEL PROYECTO DIYMON                  >> %OUT_FILE%
echo =================================================== >> %OUT_FILE%
echo. >> %OUT_FILE%


REM --- Lista de ficheros a incluir en el informe ---
set "FILES_TO_REPORT="
set "FILES_TO_REPORT=!FILES_TO_REPORT! CMakeLists.txt"
set "FILES_TO_REPORT=!FILES_TO_REPORT! main\CMakeLists.txt"
set "FILES_TO_REPORT=!FILES_TO_REPORT! main\main.c"
set "FILES_TO_REPORT=!FILES_TO_REPORT! main\hardware_manager.c"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_core\CMakeLists.txt"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_core\include\diymon_evolution.h"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_core\diymon_evolution.c"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\CMakeLists.txt"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\ui.h"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\ui.c"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\screens.h"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\screens.c"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\actions.h"
set "FILES_TO_REPORT=!FILES_TO_REPORT! components\diymon_ui\actions.c"
set "FILES_TO_REPORT=!FILES_TO_REPORT! main\lv_conf.h"


REM --- Bucle para procesar cada fichero ---
for %%F in (%FILES_TO_REPORT%) do (
    if exist "%%F" (
        echo --- START OF FILE %%F --- >> %OUT_FILE%
        type "%%F" >> %OUT_FILE%
        echo. >> %OUT_FILE%
    ) else (
        echo --- FILE NOT FOUND: %%F --- >> %OUT_FILE%
        echo. >> %OUT_FILE%
    )
)

echo. >> %OUT_FILE%
echo =================================================== >> %OUT_FILE%
echo                   FIN DEL INFORME                   >> %OUT_FILE%
echo =================================================== >> %OUT_FILE%

echo.
echo ^> ^> ^> El informe '%OUT_FILE%' ha sido generado con exito.
echo ^> ^> ^> Por favor, copia y pega su contenido.
echo.
pause