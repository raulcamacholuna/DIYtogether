# Fecha: 16/08/2025 - 09:55  */
# Fichero: ./GeminiLite.ps1 */
# Último cambio: Añadida una regla de exclusión explícita para el directorio `components/ui/assets` para omitir los ficheros de recursos gráficos. */
# Descripción: Versión optimizada que recopila los ficheros más relevantes del proyecto
#             (código fuente de la aplicación, APIs públicas y configuraciones clave) para un análisis
#             eficiente. Excluye componentes de bajo nivel (BSP), librerías de terceros,
#             ficheros de licencia y configuraciones de editor para centrarse en la lógica propia del programa. */

<#
.SYNOPSIS
    Recopila el código fuente y la configuración esenciales de un proyecto ESP-IDF/PlatformIO en un único fichero de texto.
.DESCRIPTION
    Este script analiza la estructura del proyecto, excluyendo directorios y ficheros irrelevantes
    (como 'build', '.pio', 'bsp', librerías de terceros, assets) para generar un fichero de contexto
    optimizado, ideal para análisis por parte de una IA o para archivado.
#>
# Detiene la ejecución del script si ocurre algún error.
$ErrorActionPreference = "Stop"

# --- Sección de Configuración ---
# Define el nombre del fichero de salida unificado.
$outputFile = "GeminiLite.txt"

# Define el tamaño máximo de fichero a incluir (en bytes).
$maxSizeBytes = 102400 # 100 KB

# Define los directorios que se excluirán de la búsqueda recursiva.
# Se excluyen directorios de compilación, IDE, librerías de bajo nivel (bsp) y componentes específicos.
$excludeDirs = @(
    "build", ".vscode", ".pio",
    "test_apps",
    "bsp",                  # Excluye Board Support Package para centrarse en la lógica de la app
    "ui_panels",            # Excluye componentes de UI obsoletos o secundarios
    "iot_button"            # Excluye componentes de terceros que no son el foco
)

# Define los patrones de fichero a incluir en la recopilación.
# Incluye ficheros de configuración de PlatformIO y Kconfig que son cruciales.
$includePatterns = @(
    "*.c", "*.h",
    "CMakeLists.txt",
    "sdkconfig",
    "platformio.ini",       # Fichero de configuración principal de PlatformIO
    "Kconfig*",             # Ficheros de configuración del menú
    "*.csv",
    "README.md"
)

# Define patrones de ficheros específicos a excluir globalmente por nombre.
$excludePatterns = @(
    "sdkconfig.old",
    "LICENSE*", "COPYING*",
    "*.cproject", "*.project", # Excluye ficheros de proyecto de Eclipse
    "dependencies.lock"
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
Write-Host "[EXCLUYENDO]: Directorios (por nombre): '$($excludeDirs -join "', '")'" -ForegroundColor Yellow
Write-Host "[EXCLUYENDO]: Ficheros (por nombre): '$($excludePatterns -join "', '")'" -ForegroundColor Yellow
Write-Host "[EXCLUYENDO]: Rutas (por patrón): '*/components/ui/assets/*'" -ForegroundColor Yellow
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
    # Comprueba que el fichero no exceda el tamaño máximo permitido.
    if ($file.Length -lt $maxSizeBytes) {
        $relativePath = Resolve-Path -Path $file.FullName -Relative
        Write-Host ("  INCLUIDO : " + $relativePath + " (" + $file.Length + " bytes)")
        
        # Construye un encabezado y pie de página más compactos para ahorrar espacio.
        $header = "--- START OF FILE: $relativePath ---"
        $footer = "--- END OF FILE: $relativePath ---`n"
        $fileContent = Get-Content -Path $file.FullName -Raw

        # Añade el bloque completo (encabezado, contenido, pie) al fichero de salida.
        Add-Content -Path $outputFile -Value ($header, $fileContent, $footer) -Encoding utf8NoBOM
    } else {
        Write-Host ("  OMITIDO  : " + $relativePath + " (Tamaño > $maxSizeBytes bytes)") -ForegroundColor Gray
    }
}

# 1. Búsqueda NO RECURSIVA en el directorio raíz.
Write-Host "PROCESANDO: Directorio raíz (no recursivo)" -ForegroundColor Cyan
Get-ChildItem -Path . -Depth 0 -File -Include $includePatterns | Where-Object {
    $_.Name -notin $excludePatterns
} | ForEach-Object {
    Process-FoundFile -file $_
}

# 2. Búsqueda RECURSIVA en los directorios 'components' y 'main'.
$recursiveSearchPaths = @("components", "main")
Write-Host "PROCESANDO: Directorios '$($recursiveSearchPaths -join "', '")' (recursivo)" -ForegroundColor Cyan
foreach ($path in $recursiveSearchPaths) {
    if (Test-Path $path) {
        Get-ChildItem -Path $path -Recurse -File -Include $includePatterns -Exclude $excludeDirs -ErrorAction SilentlyContinue | Where-Object {
            # Normaliza los separadores de ruta para un match consistente en cualquier OS
            $normalizedPath = $_.FullName.Replace('\', '/')
            # Aplica filtros de exclusión por nombre de fichero y por ruta específica
            ($_.Name -notin $excludePatterns) -and
            ($normalizedPath -notlike '*/components/ui/assets/*')
        } | ForEach-Object {
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
