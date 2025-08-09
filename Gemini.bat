@echo off
:: Habilita la expansion retardada, crucial para la fiabilidad del IF/ELSE.
setlocal enabledelayedexpansion

:: -------------------------------------------------
:: Configuracion
:: -------------------------------------------------
set "OUT_FILE=gemini.txt"
set "MAX_SIZE_BYTES=102400"

:: -------------------------------------------------
:: Inicio
:: -------------------------------------------------
cls
echo.
echo =============================================================
echo       Recopilador de Codigo para Gemini (v6 - Final)
echo =============================================================
echo.
echo Buscando ficheros .c, .h, CMakeLists.txt, .yml
echo.
echo [EXCLUYENDO]: Directorios '.vscode' y 'build'
echo [FILTRANDO] : Ficheros > 100 KB (%MAX_SIZE_BYTES% bytes).
echo [GUARDANDO EN]: %OUT_FILE%
echo.
echo -------------------------------------------------------------
echo.

:: Borra el fichero de salida si ya existe
if exist "%OUT_FILE%" del "%OUT_FILE%" > nul

:: -------------------------------------------------------------------
:: Bucle Principal - Usa DIR y FINDSTR para maxima fiabilidad y control
:: -------------------------------------------------------------------
:: Este es el comando mas robusto:
:: 1. dir /s /b /a-d *.*             -> Lista todos los ficheros, recursivamente, en formato simple.
:: 2. findstr /i /e ".c .h .yml"      -> Filtra para incluir solo los que terminan en .c, .h, .yml.
:: 3. findstr /i "CMakeLists.txt"    -> Incluye tambien los CMakeLists.txt.
:: 4. findstr /i /v /c:"\build\" /c:"\.vscode\" -> Excluye las carpetas especificadas.
::
:: El bucle 'for /f' procesa el resultado de esta cadena de comandos.

for /f "delims=" %%F in (
    'dir /s /b /a-d *.* ^| findstr /i /v /c:"\build\" /c:"\.vscode\" ^| findstr /i /r /c:"\\.c$" /c:"\\.h$" /c:"\\.yml$" /c:"CMakeLists.txt"'
) do (
    
    set "filePath=%%F"
    set "fileSize=%%~zF"

    :: La sintaxis !variable! gracias a enabledelayedexpansion evita el error del IF/ELSE.
    if !fileSize! LSS %MAX_SIZE_BYTES% (
        echo INCLUIDO  : !filePath! (!fileSize! bytes)
        
        (
            echo --- START OF FILE !filePath! ---
            type "!filePath!"
            echo.
        ) >>"%OUT_FILE%"
        
    ) else (
        echo OMITIDO   : !filePath! (!fileSize! bytes)
    )
)

:: -------------------------------------------------
:: Fin
:: -------------------------------------------------
echo.
echo -------------------------------------------------------------
echo.
echo =============================================================
echo      Proceso completado.
echo =============================================================
echo.

endlocal
pause