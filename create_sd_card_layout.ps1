# Fichero: create_sd_card_layout.ps1
# Último cambio: Creación de un script para generar la estructura de directorios y ficheros de ayuda para la tarjeta SD.
# Descripción: Diagnóstico de Causa Raíz: El sistema arranca y compila con éxito. El problema de la 'pantalla en negro' y la falta de respuesta táctil visible se debe a que el firmware no encuentra los ficheros de animación en la tarjeta SD. El log lo confirma explícitamente: 'E (3758) UI_IDLE_ANIM: No se encontraron fotogramas...'. El código está diseñado para ocultar el personaje si las animaciones no se cargan, resultando en una pantalla vacía. La lógica táctil funciona, pero no tiene elementos visuales con los que interactuar.
# Solución Definitiva: El problema no reside en el código del firmware, sino en la ausencia de datos en la tarjeta SD. Este script PowerShell implementa la solución generando la estructura de directorios y ficheros de ayuda EXACTA que el firmware espera encontrar en la raíz de una tarjeta SD formateada en FAT32. El usuario debe ejecutar este script, copiar la carpeta 'sdcard_template' resultante a la raíz de su tarjeta SD (renombrándola si es necesario) y poblarla con los ficheros de animación '.bin' correspondientes.
# Último cambio: 20/08/2025 - 06:53
$baseDir = ".\sdcard_template"
$diymonDir = Join-Path -Path $baseDir -ChildPath "diymon"

# Limpiar estructura anterior si existe
if (Test-Path $baseDir) {
    Remove-Item -Recurse -Force $baseDir
}

# Crear la estructura base
New-Item -ItemType Directory -Path $baseDir | Out-Null
New-Item -ItemType Directory -Path $diymonDir | Out-Null

# Lista de todos los códigos de evolución definidos en `diymon_evolution.c`
$evolutionCodes = @(
    "0", "1", "2", "3", "4", "1.1", "1.2", "1.3", "1.4", "2.1", "2.2",
    "3.1", "3.3", "4.1", "4.4", "1.1.1", "1.1.2", "2.2.1", "2.2.2",
    "3.3.1", "3.3.2", "3.3.3", "3.3.4", "4.4.1", "4.4.4"
)

# Crear un subdirectorio para cada código de evolución
foreach ($code in $evolutionCodes) {
    $dirName = $code -replace '\.', ''
    $evoDir = Join-Path -Path $diymonDir -ChildPath $dirName
    New-Item -ItemType Directory -Path $evoDir | Out-Null

    $readmeContent = @"
Este directorio es para la evolución '$code'.

Coloca aquí los ficheros de animación correspondientes, por ejemplo:
- ANIM_IDLE_1.bin
- ANIM_IDLE_2.bin
- ...
- ANIM_EAT_1.bin
- ...
- etc.
"@
    Set-Content -Path (Join-Path -Path $evoDir -ChildPath "README.txt") -Value $readmeContent
}

# Crear la carpeta de configuración para el servidor web
$configDir = Join-Path -Path $baseDir -ChildPath "config"
New-Item -ItemType Directory -Path $configDir | Out-Null
Set-Content -Path (Join-Path -Path $configDir -ChildPath "README.txt") -Value "Coloca aquí los ficheros Index.html, backup.html y otros assets para el servidor web."

Write-Host "------------------------------------------------------------------"
Write-Host " ESTRUCTURA DE LA TARJETA SD CREADA EN: .\sdcard_template " -ForegroundColor Green
Write-Host "------------------------------------------------------------------"
Write-Host "ACCIÓN REQUERIDA:"
Write-Host "1. Formatea tu tarjeta SD en FAT32."
Write-Host "2. Copia el CONTENIDO de la carpeta '.\sdcard_template' a la RAÍZ de tu tarjeta SD."
Write-Host "   (Tu tarjeta SD debe contener una carpeta 'diymon' y una carpeta 'config' en la raíz)."
Write-Host "3. Puebla cada subdirectorio dentro de 'diymon' con los ficheros de animación '.bin' correspondientes."
Write-Host "4. Inserta la tarjeta SD en el dispositivo y reinícialo."
Write-Host "------------------------------------------------------------------"
