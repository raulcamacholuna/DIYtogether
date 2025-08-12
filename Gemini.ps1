# Fichero: ./Gemini.ps1
# Fecha: 12/08/2025 - 01:51 PM
# Último cambio: Corregido el generador de encabezados para evitar errores de análisis.
# Descripción: Script de PowerShell que recopila el código fuente y todos los ficheros de
#              configuración relevantes del proyecto en un único fichero de texto.
#              Busca de forma no recursiva en la raíz y recursiva solo en
#              directorios específicos ('main', 'components').

# Detiene la ejecución del script si ocurre algún error.
$ErrorActionPreference = "Stop"

# --- Sección de Configuración ---
# Define el nombre del fichero de salida unificado.
$outputFile = "Gemini.txt"

# Define el tamaño máximo de fichero a incluir (en bytes).
$maxSizeBytes = 102400 # 100 KB

# Define los directorios que se excluirán de la búsqueda recursiva.
$excludeDirs = @("build", ".vscode")

# Define los patrones de fichero a incluir en la recopilación.
$includePatterns = @(
    "*.c", "*.h", "*.cpp", "*.hpp",
    "*.yml", "*.yaml",
    "*.csv",
    "*.md",
    "CMakeLists.txt",
    "sdkconfig*", "*kconfig*",
    ".gitignore", ".gitattributes",
    ".clang-format", ".clangd",
    ".cproject", ".project",
    "LICENSE*", "COPYING*"
)

# --- Encabezado de Interfaz y Limpieza de Ficheros ---
Clear-Host
Write-Host "=============================================================" -ForegroundColor Green
Write-Host "      Recopilador de Codigo Unificado para Gemini" -ForegroundColor Green
Write-Host "============================================================="
Write-Host
Write-Host "Buscando ficheros de proyecto con las siguientes reglas:"
Write-Host " - Raíz ('.')      : No recursivo"
Write-Host " - 'components/', 'main/': Búsqueda recursiva"
Write-Host
Write-Host "[EXCLUYENDO]: Directorios en búsqueda recursiva: '$($excludeDirs -join "', '")'" -ForegroundColor Yellow
Write-Host "[FILTRANDO] : Ficheros > $($maxSizeBytes / 1kb) KB" -ForegroundColor Yellow
Write-Host
Write-Host "Salida generada:" -ForegroundColor Green
Write-Host " - Todos los ficheros -> $outputFile"
Write-Host
Write-Host "-------------------------------------------------------------"
Write-Host

# Elimina el fichero de salida anterior si existe.
if (Test-Path $outputFile) { Remove-Item $outputFile }

# --- Lógica Principal de Recopilación ---

# Función auxiliar para procesar cada fichero encontrado.
function Process-FoundFile {
    param (
        [Parameter(Mandatory=$true)]
        [System.IO.FileInfo]$file
    )
    if ($file.Length -lt $maxSizeBytes) {
        Write-Host ("  INCLUIDO : " + $file.FullName + " (" + $file.Length + " bytes)")
        # Construye el encabezado del fichero para evitar problemas con heredoc anidados.
        $header = "
# =================================================================================================
" +
                  "# Fichero: $($file.FullName)
" +
                  "# =================================================================================================
"
        Add-Content -Path $outputFile -Value $header
        # Añade el contenido del fichero.
        Add-Content -Path $outputFile -Value (Get-Content -Path $file.FullName -Raw)
    } else {
        Write-Host ("  OMITIDO  : " + $file.FullName + " (Tamaño > $maxSizeBytes bytes)") -ForegroundColor Gray
    }
}

# 1. Búsqueda NO RECURSIVA en el directorio raíz.
Write-Host "PROCESANDO: Directorio raíz (no recursivo)" -ForegroundColor Cyan
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
Write-Host "      Proceso completado. Fichero de salida: $outputFile" -ForegroundColor Green
Write-Host "============================================================="
Write-Host
