# Fichero: C_Asset_Converter.ps1
# Fecha: 15/08/2025 - 11:30
# Último cambio: Adaptado para generar assets de color (RGB565) y máscaras (L8).
# Descripción: Script de PowerShell que procesa cada fichero .png y lo convierte
#              en un fichero de código C (.c). Detecta automáticamente si un
#              fichero es una máscara (nombre terminado en '_mask.png') para
#              aplicar el formato L8, o si es una imagen de color para aplicar RGB565.

# --- INICIO DEL SCRIPT ---

Clear-Host

# --- ENCABEZADO VISUAL ---
Write-Host "----------------------------------------------------" -ForegroundColor Green
Write-Host "--- Conversor de Assets LVGL (PNG -> C)          ---" -ForegroundColor Green
Write-Host "--- Detecta 'nombre_mask.png' para formato L8    ---" -ForegroundColor Green
Write-Host "--- y el resto para formato RGB565               ---" -ForegroundColor Green
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

Write-Host "Se encontraron $($pngFiles.Count) archivos .png. Iniciando conversión..." -ForegroundColor Cyan
Write-Host "----------------------------------------------------"

# --- BUCLE DE CONVERSIÓN INDIVIDUAL ---
foreach ($file in $pngFiles) {
    $inputFile = $file.FullName
    $outputFileName = $file.BaseName
    $expectedOutputFile = Join-Path $currentFolder "$($outputFileName).c"

    Write-Host "Procesando: $($file.Name)"
    
    # --- MODIFICADO: Lógica para decidir el formato de color ---
    $colorFormat = ""
    if ($file.BaseName -like "*_mask") {
        $colorFormat = "L8"
        Write-Host "  -> Detectada MÁSCARA. Usando formato de color: $colorFormat" -ForegroundColor Yellow
    } else {
        $colorFormat = "RGB565"
        Write-Host "  -> Detectada imagen de COLOR. Usando formato de color: $colorFormat" -ForegroundColor Cyan
    }
    # -----------------------------------------------------------
    
    # --- Lógica para ejecutar pngquant.exe (sin cambios) ---
    $scriptsPath = Split-Path -Path $lvglConverterScript -Parent
    Push-Location -Path $scriptsPath
    
    # --- MODIFICADO: El comando ahora usa la variable $colorFormat ---
    $commandToRun = "python `"$lvglConverterScript`" `"$inputFile`" --out `"$expectedOutputFile`" --ofmt C --cf $colorFormat"
    
    Write-Host "-> Comando: $commandToRun" -ForegroundColor Gray
    
    Invoke-Expression $commandToRun
    
    Pop-Location
    # -----------------------------------------------------------

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
