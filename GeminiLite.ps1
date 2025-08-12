# Fichero: ./GeminiLite.ps1
# Fecha: 12/08/2025 - 02:15 pm
# Último cambio: Corregido el anidamiento de here-strings y forzada la codificación UTF-8.
# Descripción: Versión optimizada que recopila solo los ficheros más relevantes del proyecto
#              (código fuente, APIs públicas y configuraciones clave) para un análisis
#              eficiente. Excluye ficheros de licencia, configuraciones de editor y
#              componentes obsoletos para centrarse en la lógica del programa.

# Detiene la ejecución del script si ocurre algún error.
$ErrorActionPreference = "Stop"

# --- Sección de Configuración ---
# Define el nombre del fichero de salida unificado.
$outputFile = "GeminiLite.txt"

# Define el tamaño máximo de fichero a incluir (en bytes).
$maxSizeBytes = 102400 # 100 KB

# Define los directorios que se excluirán de la búsqueda recursiva.
$excludeDirs = @(
    "build", 
    ".vscode", 
    "test_apps",            # Excluye directorios de pruebas de componentes
    "diymon_ui_panels",     # Excluye componentes obsoletos o no utilizados
    "iot_button"            # Excluye componentes de terceros que no son el foco
)

# Define los patrones de fichero a incluir en la recopilación.
$includePatterns = @(
    "*.c", "*.h",
    "CMakeLists.txt",
    "sdkconfig",
    "*.csv",
    "README.md"             # El README principal es muy valioso para el contexto
)

# Define patrones de ficheros específicos a excluir globalmente.
$excludePatterns = @(
    "sdkconfig.old",        # Excluye copias de seguridad de configuraciones
    "LICENSE*", "COPYING*"  # Excluye ficheros de licencia extensos
)

# --- Encabezado de Interfaz y Limpieza de Ficheros ---
Clear-Host
Write-Host "=============================================================" -ForegroundColor Green
Write-Host "      Recopilador de Codigo Unificado para Gemini (Lite)" -ForegroundColor Green
Write-Host "============================================================="
Write-Host
Write-Host "Buscando ficheros de proyecto con las siguientes reglas optimizadas:"
Write-Host " - Raíz ('.')      : No recursivo"
Write-Host " - 'components/', 'main/': Búsqueda recursiva"
Write-Host
Write-Host "[INCLUYENDO]: Patrones: '$($includePatterns -join "', '")'" -ForegroundColor Cyan
Write-Host "[EXCLUYENDO]: Directorios: '$($excludeDirs -join "', '")'" -ForegroundColor Yellow
Write-Host "[EXCLUYENDO]: Ficheros: '$($excludePatterns -join "', '")'" -ForegroundColor Yellow
Write-Host "[FILTRANDO] : Ficheros > $($maxSizeBytes / 1kb) KB" -ForegroundColor Yellow
Write-Host
Write-Host "Salida generada:" -ForegroundColor Green
Write-Host " - Ficheros relevantes -> $outputFile"
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
    # Se comprueba que el fichero no exceda el tamaño máximo permitido.
    if ($file.Length -lt $maxSizeBytes) {
        Write-Host ("  INCLUIDO : " + $file.FullName + " (" + $file.Length + " bytes)")
        
        # [CORRECCIÓN] Se construye el encabezado con concatenación de cadenas para evitar here-strings anidados.
        $header = "
# =================================================================================================
" +
                  "# Fichero: $($file.FullName)
" +
                  "# =================================================================================================
"

        # Se añade el encabezado al fichero de salida, especificando la codificación UTF-8 sin BOM.
        Add-Content -Path $outputFile -Value $header -Encoding utf8NoBOM
        
        # Se añade el contenido del fichero. -Raw lo lee tal cual y -Encoding utf8NoBOM lo escribe de forma compatible.
        Add-Content -Path $outputFile -Value (Get-Content -Path $file.FullName -Raw) -Encoding utf8NoBOM
    } else {
        Write-Host ("  OMITIDO  : " + $file.FullName + " (Tamaño > $maxSizeBytes bytes)") -ForegroundColor Gray
    }
}

# 1. Búsqueda NO RECURSIVA en el directorio raíz.
Write-Host "PROCESANDO: Directorio raíz (no recursivo)" -ForegroundColor Cyan
Get-ChildItem -Path . -Depth 0 -File -Include $includePatterns -Exclude $excludePatterns -ErrorAction SilentlyContinue | ForEach-Object {
    Process-FoundFile -file $_
}

# 2. Búsqueda RECURSIVA en los directorios 'components' y 'main'.
$recursiveSearchPaths = @("components", "main")
Write-Host "PROCESANDO: Directorios '$($recursiveSearchPaths -join "', '")' (recursivo)" -ForegroundColor Cyan
foreach ($path in $recursiveSearchPaths) {
    if (Test-Path $path) {
        # Combina las exclusiones de ficheros y directorios para la búsqueda recursiva.
        Get-ChildItem -Path $path -Recurse -File -Include $includePatterns -Exclude ($excludePatterns + $excludeDirs) -ErrorAction SilentlyContinue | ForEach-Object {
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
