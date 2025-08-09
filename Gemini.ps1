# Fichero: ./recopilador_codigo_v4.ps1
# Fecha: 07/08/2024 - 10:30
# Último cambio: Añadida la recopilación de ficheros .yml.
# Descripción: Script de PowerShell que recopila el código fuente y ficheros de
#              configuración (.c, .h, .yml, CMakeLists.txt) de un proyecto.
#              Busca de forma no recursiva en la raíz y recursiva solo en
#              directorios específicos ('main', 'components').

# Detiene la ejecución del script si ocurre algún error.
$ErrorActionPreference = "Stop"

# --- Sección de Configuración ---
# Define los nombres de los ficheros de salida.
$cmakeOutFile = "GeminicCMake.txt"
$cOutFile = "GeminiC.txt"
$hOutFile = "GeminiH.txt"
$ymlOutFile = "GeminiYML.txt"

# Define el tamaño máximo de fichero a incluir (en bytes).
$maxSizeBytes = 102400 # 100 KB

# Define los directorios que se excluirán de la búsqueda recursiva.
$excludeDirs = @("build", ".vscode")

# --- Encabezado de Interfaz y Limpieza de Ficheros ---
Clear-Host
Write-Host "=============================================================" -ForegroundColor Green
Write-Host "      Recopilador de Codigo PowerShell para Gemini" -ForegroundColor Green
Write-Host "============================================================="
Write-Host
Write-Host "Buscando ficheros de proyecto con las siguientes reglas:"
Write-Host " - Raíz ('.')      : No recursivo"
Write-Host " - 'components/', 'main/': Búsqueda recursiva"
Write-Host
Write-Host "[EXCLUYENDO]: Directorios en búsqueda recursiva: '$($excludeDirs -join "', '")'" -ForegroundColor Yellow
Write-Host "[FILTRANDO] : Ficheros > $($maxSizeBytes / 1kb) KB" -ForegroundColor Yellow
Write-Host
Write-Host "Salidas generadas:" -ForegroundColor Green
Write-Host " - CMakeLists.txt -> $cmakeOutFile"
Write-Host " - Ficheros C     -> $cOutFile"
Write-Host " - Ficheros H     -> $hOutFile"
Write-Host " - Ficheros YML   -> $ymlOutFile"
Write-Host
Write-Host "-------------------------------------------------------------"
Write-Host

# Elimina los ficheros de salida anteriores si existen.
if (Test-Path $cmakeOutFile) { Remove-Item $cmakeOutFile }
if (Test-Path $cOutFile) { Remove-Item $cOutFile }
if (Test-Path $hOutFile) { Remove-Item $hOutFile }
if (Test-Path $ymlOutFile) { Remove-Item $ymlOutFile }

# --- Lógica Principal de Recopilación ---

# Función auxiliar para procesar cada fichero encontrado y evitar repetir código.
function Process-FoundFile {
    param (
        [Parameter(Mandatory=$true)]
        [System.IO.FileInfo]$file
    )
    $outFile = $null

    # Determina el fichero de salida según la extensión o el nombre.
    switch ($file.Extension) {
        ".c"   { $outFile = $cOutFile }
        ".h"   { $outFile = $hOutFile }
        ".yml" { $outFile = $ymlOutFile }
        default {
            if ($file.Name -eq "CMakeLists.txt") {
                $outFile = $cmakeOutFile
            }
        }
    }

    if ($outFile) {
        if ($file.Length -lt $maxSizeBytes) {
            Write-Host ("  INCLUIDO : " + $file.FullName + " (" + $file.Length + " bytes)")
            Add-Content -Path $outFile -Value "--- START OF FILE $($file.FullName) ---"
            Add-Content -Path $outFile -Value (Get-Content -Path $file.FullName -Raw)
            Add-Content -Path $outFile -Value ""
        } else {
            Write-Host ("  OMITIDO  : " + $file.FullName + " (Tamaño > $maxSizeBytes bytes)") -ForegroundColor Gray
        }
    }
}

# Define los tipos de fichero a buscar.
$includePatterns = "*.c", "*.h", "CMakeLists.txt", "*.yml"

# 1. Búsqueda NO RECURSIVA en el directorio raíz.
Write-Host "PROCESANDO: Directorio raíz (no recursivo)" -ForegroundColor Cyan
# Usamos -Depth 0 para evitar que entre en subdirectorios.
Get-ChildItem -Path . -Depth 0 -File -Include $includePatterns -ErrorAction SilentlyContinue | ForEach-Object {
    Process-FoundFile -file $_
}

# 2. Búsqueda RECURSIVA en los directorios 'components' y 'main'.
$recursiveSearchPaths = @("components", "main")
Write-Host "PROCESANDO: Directorios '$($recursiveSearchPaths -join "', '")' (recursivo)" -ForegroundColor Cyan
foreach ($path in $recursiveSearchPaths) {
    if (Test-Path $path) {
        Get-ChildItem -Path $path -Recurse -File -Include $includePatterns -Exclude $excludeDirs -ErrorAction SilentlyContinue | ForEach-Object {
            Process-FoundFile -file $_
        }
    } else {
        Write-Host "  AVISO    : La ruta '$path' no existe. Omitiendo." -ForegroundColor Yellow
    }
}

# --- Pie de Página ---
Write-Host
Write-Host "-------------------------------------------------------------"
Write-Host
Write-Host "=============================================================" -ForegroundColor Green
Write-Host "      Proceso completado." -ForegroundColor Green
Write-Host "============================================================="
Write-Host

Read-Host "Presiona Enter para salir"