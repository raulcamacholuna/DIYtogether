# Fichero: RGB565A8.bin.ps1
# Fecha: 10/08/2025 - 06:00
# Último cambio: Corregido el parámetro --out para generar ficheros en lugar de carpetas.
# Descripción: Script de PowerShell que procesa cada fichero .png de forma individual. Se ha
#              corregido el comando de conversión para que especifique solo la carpeta de
#              salida, permitiendo que la herramienta de LVGL nombre el fichero .bin
#              automáticamente y evitando la creación de directorios no deseados.

# --- INICIO DEL SCRIPT ---

Clear-Host

# --- ENCABEZADO VISUAL ---
Write-Host "----------------------------------------------------" -ForegroundColor Green
Write-Host "--- Conversor Individual LVGL (.PNG -> .BIN)     ---" -ForegroundColor Green
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

Write-Host "Se encontraron $($pngFiles.Count) archivos .png. Iniciando conversión individual..." -ForegroundColor Cyan
Write-Host "----------------------------------------------------"

# --- BUCLE DE CONVERSIÓN INDIVIDUAL ---
foreach ($file in $pngFiles) {
    $inputFile = $file.FullName
    $expectedOutputFile = [System.IO.Path]::ChangeExtension($inputFile, '.bin')

    Write-Host "Procesando: $($file.Name)"
    
    # --- LA SOLUCIÓN ---
    # Al parámetro --out solo le pasamos la carpeta de destino.
    # El script de LVGL se encargará de nombrar el fichero de salida correctamente.
    $commandToRun = "python `"$lvglConverterScript`" `"$inputFile`" --out `"$currentFolder`" --ofmt BIN --cf RGB565A8"
    
    Write-Host "-> Comando: $commandToRun" -ForegroundColor Gray
    
    Invoke-Expression $commandToRun
    
    if (Test-Path $expectedOutputFile) {
        Write-Host "  -> ÉXITO: Archivo `"$($file.BaseName).bin`" creado." -ForegroundColor Green
    } else {
        Write-Host "  -> ERROR: No se pudo crear el archivo `"$($file.BaseName).bin`"." -ForegroundColor Red
    }
    Write-Host ""
}

# --- FINALIZACIÓN ---
Write-Host "----------------------------------------------------" -ForegroundColor Green
Write-Host "¡Conversión completada!" -ForegroundColor Green
