# Fichero: C_I8_Converter.ps1
# Fecha: 15/08/2025 - 11:00
# Último cambio: Adaptado para generar ficheros C en formato I8.
# Descripción: Script de PowerShell que procesa cada fichero .png y lo convierte
#              en un fichero de código C (.c) usando el formato de color indexado (I8).
#              Incluye la lógica para cambiar el directorio de trabajo y asegurar que
#              herramientas como pngquant.exe sean encontradas y ejecutadas correctamente.

# --- INICIO DEL SCRIPT ---

Clear-Host

# --- ENCABEZADO VISUAL ---
Write-Host "----------------------------------------------------" -ForegroundColor Green
Write-Host "--- Conversor Individual LVGL (.PNG -> .C)       ---" -ForegroundColor Green
Write-Host "----------------------------------------------------"

# --- CONFIGURACIÓN DE RUTAS ---
$currentFolder = $PSScriptRoot
$lvglConverterScript = Join-Path $currentFolder "..\components_dependencies\lvgl\scripts\LVGLImage.py"

Write-Host "Carpeta de trabajo: $currentFolder"
Write-Host "Ruta al conversor: $lvglConverterScript"

# --- VERIFICACIÓN DE ARCHIVOS ---
$pngFiles = Get-ChildItem -Path $currentFolder -Filter "*.png"

if (-not $pngFiles) {
    Write-Host "ADVERTENCIA: No se encontraron archivos .png en esta carpeta." -ForegroundColor Yellow
    Read-Host "Presiona Enter para salir"
    exit
}

Write-Host "Se encontraron $($pngFiles.Count) archivos .png. Iniciando conversión a formato C..." -ForegroundColor Cyan
Write-Host "----------------------------------------------------"

# --- BUCLE DE CONVERSIÓN INDIVIDUAL ---
foreach ($file in $pngFiles) {
    $inputFile = $file.FullName
    $outputFileName = $file.BaseName
    $expectedOutputFile = Join-Path $currentFolder "$($outputFileName).c" # <-- Salida ahora es .c

    Write-Host "Procesando: $($file.Name)"
    
    # --- LA SOLUCIÓN DEFINITIVA PARA PNGQUANT ---
    # 1. Obtenemos la ruta a la carpeta donde residen los scripts de LVGL.
    $scriptsPath = Split-Path -Path $lvglConverterScript -Parent

    # 2. Guardamos la ubicación actual y nos movemos temporalmente a la carpeta de scripts.
    Push-Location -Path $scriptsPath
    
    # 3. Construimos el comando. Las rutas deben ser absolutas.
    #    --ofmt C -> Para generar un fichero de código C.
    #    --cf I8  -> Para usar color indexado de 8-bit (256 colores).
    #    --out    -> Debe apuntar al ARCHIVO de salida .c completo.
    $commandToRun = "python `"$lvglConverterScript`" `"$inputFile`" --out `"$expectedOutputFile`" --ofmt C --cf RGB565A8"
    
    Write-Host "-> Comando: $commandToRun" -ForegroundColor Gray
    
    # 4. Ejecutamos el comando.
    Invoke-Expression $commandToRun
    
    # 5. Regresamos a la carpeta de trabajo original.
    Pop-Location
    # ------------------------------------------------

    if (Test-Path $expectedOutputFile) {
        Write-Host "  -> ÉXITO: Archivo `"$($outputFileName).c`" creado." -ForegroundColor Green
    } else {
        Write-Host "  -> ERROR: No se pudo crear el archivo `"$($outputFileName).c`"." -ForegroundColor Red
    }
    Write-Host ""
}

# --- FINALIZACIÓN ---
Write-Host "----------------------------------------------------" -ForegroundColor Green
Write-Host "¡Conversión completada!" -ForegroundColor Green
